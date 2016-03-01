#include <stdio.h>
#include <string.h>    // for strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <pthread.h>   // for threading, link with lpthread
#include <strings.h>
#include <json/json.h> // for strlen
#include <stdbool.h>
#include <time.h>


// #### Variables ####

struct mess                 //  user's messagebox
{
	char sender[20];
	char sender_text[1000];
	char date[100];
};



struct user                 // contain all necessary input
{
	char user_name[20];
	char state[100];
	int socket_id;
	struct mess messlist[20];
	int mess_count;
};

struct user user_list[20];

static int user_count;
char cmd[15];               //  to command which sending from client
char username[20]; 			//  to username which sending from client
char to[20];				//  to to which sending from client
char text[1000];			//  to text which sending from client
char tempEverything[2000];  //  temporary variable
char sendClient[2000];      //  send message to client


//#### Function prototypes ####

void LoginFault(char fault_command[1000],int a);      // to find invalid login
void answerLogin(char accept_login[100]);             //  send login state
void answerList(char listuserlist[2000]);             //  send all of user
void newMessage(char username[20],char nMessage[1000],char message_accept[1000]);  // to send message another user
void answerLogout(char accept_logout[100]);           //  send logout state
void notFoundUser(char not_found[100]);               //  is user available?
void permissionDenied(char permission[100]);          //  user not login
void answerControlSendFault(char answerControlSendF[100]);    // fault login
void answerControlSendCorrect(char answerControlSendC[100]);  // correct login
void logout_control(int sock);                         // to logout control
void AddUserList(int sock);                            // add user
char *getTime();                                       // return system's date
int online_control(int sock);                          // is user online?
int ctrlSendLog(int sock);                             // send correctly
int get_login_control(int sock);

void json_parse(json_object * jobj)   // to parse json object
{
	enum json_type type;
	json_object_object_foreach(jobj, key, val)
	{
		type=json_object_get_type(val);
		switch(type)
		{
		case json_type_string:
			if(strcmp(key,"command")==0)
			{strcpy(cmd,json_object_get_string(val));}
			else if(strcmp(key,"username")==0)
			{strcpy(username,json_object_get_string(val));}
			else if(strcmp(key,"to")==0)
			{strcpy(to,json_object_get_string(val));}
			else if(strcmp(key,"text")==0)
			{strcpy(text,json_object_get_string(val));}
			break;

		}
	}
}
int online_control(int sock)  // find user online or not with socket
{
	int i=0;
	for (i = 0; i < 20; i++)
	{
		if(user_list[i].socket_id==sock)
		{
			if(strcmp(user_list[i].state,"online")==0)

			{
				return 0;
			}
		}
	}
	return 1;
}
void AddUserList(int sock)    // add user
{
	strcpy(user_list[user_count].user_name,username);
	strcpy(user_list[user_count].state,"online");
	user_list[user_count].socket_id=sock;
	user_count++;
}


int get_login_control(int sock)
{
	memset(tempEverything, '\0', sizeof(tempEverything));
	int i=0;
	for (i = 0; i < 20; i++)
	{
		if(strcmp(user_list[i].user_name,username)==0)
		{
			if (strcmp(user_list[i].state,"online")==0)  // user already online
			{
				LoginFault(&tempEverything,1);
				strcpy(sendClient,tempEverything);
				puts(sendClient);
				memset(tempEverything, '\0', sizeof(tempEverything));
			}
			else
			{
				strcpy(user_list[i].state,"online");  // user just online
				user_list[i].socket_id=sock;
				answerLogin(&tempEverything);
				strcpy(sendClient,tempEverything);
				puts(sendClient);
				int a=0;
				for (a = 0; a < user_list[i].mess_count;a++ )  //  print message on screen , message receivered when user offline
				{
					puts(user_list[i].messlist[a].sender_text);
					write(sock , user_list[i].messlist[a].sender_text , strlen(user_list[i].messlist[a].sender_text));
				}
				user_list[i].mess_count=0;
				memset(tempEverything, '\0', sizeof(tempEverything));
			}
			return 1;
		}
		else if(user_list[i].socket_id==sock)  // if user didn't logout and user want a new username, print error message
		{
			LoginFault(&tempEverything,2);
			strcpy(sendClient,tempEverything);
			puts(sendClient);
			memset(tempEverything, '\0', sizeof(tempEverything));
			return 1;
		}
	}
	return 0;
}
int ctrlSendLog(int sock)  // if user logout and user want a send another user, find socketıd and print error message
{
	int j=0;
	for (j= 0; j < 20; j++)
	{
		if(user_list[j].socket_id==sock)
		{
			return 1;
		}
	}
	return 0;
}
char *getTime()
{
	time_t curtime;
	time(&curtime);

	return(ctime(&curtime));
}
void logout_control(int sock)    // add lastseen time on user's state
{
	int i=0;
	for (i = 0; i < 20; i++)
	{
		if(user_list[i].socket_id==sock)
		{
			user_list[i].socket_id=-1500;
			strcpy(user_list[i].state,strtok(getTime(), "\n"));
			answerLogout(&tempEverything);
			strcpy(sendClient,tempEverything);
			puts(sendClient);
			memset(tempEverything, '\0', sizeof(tempEverything));
		}
	}
}
void *connection_handler(void *socket_desc)					//create thread function
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char *message , client_message[2000];

	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )		//listen port
	{
		//Send the message back to client
		json_object * jobj = json_tokener_parse(client_message);

		json_parse(jobj);

		if(strcmp(cmd,"login")==0)
		{
			int a=get_login_control(sock);				//update user and check user
			if (a==0)
			{
				AddUserList(sock);					//add user to list
				answerLogin(&tempEverything);		//check login state
				strcpy(sendClient,tempEverything);
				puts(sendClient);

				memset(tempEverything, '\0', sizeof(tempEverything));
			}
			write(sock , sendClient , strlen(sendClient));
		}
		else if(strcmp(cmd,"getList")==0)
		{
			int onlineControl=online_control(sock);
			if(onlineControl==0)
			{
				answerList(&tempEverything);					//get user list and controls
				strcpy(sendClient,tempEverything);
				puts(sendClient);
			}
			else
			{
				permissionDenied(&tempEverything);
				strcpy(sendClient,tempEverything);
				puts(sendClient);
			}
			memset(tempEverything, '\0', sizeof(tempEverything));
			write(sock , sendClient , strlen(sendClient));
		}
		else if(strcmp(cmd,"controlSendLog")==0)			//check login state and send message
		{

			int f=ctrlSendLog(sock);
			if (f==0)
			{

				answerControlSendFault(&tempEverything);
				strcpy(sendClient,tempEverything);

			}

			else {

				answerControlSendCorrect(&tempEverything);
				strcpy(sendClient,tempEverything);
			}
			puts(sendClient);

			write(sock , sendClient , strlen(sendClient));
			memset(tempEverything, '\0', sizeof(tempEverything));
		}

		else if (strcmp(cmd,"logout")==0)						//logout user and controls
		{
			int onlineControl=online_control(sock);
			if(onlineControl==0)
			{
				logout_control(sock);
				write(sock , sendClient , strlen(sendClient));
			}
			else
			{
				permissionDenied(&tempEverything);
				strcpy(sendClient,tempEverything);
				puts(sendClient);
				memset(tempEverything, '\0', sizeof(tempEverything));
				write(sock , sendClient , strlen(sendClient));
			}
		}
		else if(strcmp(cmd,"sendMessage")==0)					//send message for user
		{
			int onlineControl=online_control(sock);
			if(onlineControl==0)
			{
				newMessage(to,text,&tempEverything);			//create json and send message
				strcpy(sendClient,tempEverything);
				puts(sendClient);
				memset(tempEverything, '\0', sizeof(tempEverything));
				int i=0;
				for (i = 0; i < user_count; ++i)
				{
					if(user_list[i].socket_id==sock)
					{
						strcpy(username,user_list[i].user_name);
						break;
					}
				}
				i=0;
				bool flag=true;
				for (i = 0; i < user_count; ++i)				//user messagebox and write message
				{
					if(strcmp(user_list[i].user_name,to)==0)
					{
						newMessage(username,text,&tempEverything);
						strcpy(sendClient,tempEverything);
						memset(tempEverything, '\0', sizeof(tempEverything));
						if(strcmp(user_list[i].state,"online")==0)
						{
							write(user_list[i].socket_id , sendClient , strlen(sendClient));
						}
						else
						{
							strcpy(user_list[i].messlist[user_list[i].mess_count].sender_text,sendClient);
							user_list[i].mess_count++;
						}
						flag=false;
					}
				}
				if (flag==true)
				{
					notFoundUser(&tempEverything);
					strcpy(sendClient,tempEverything);
					puts(sendClient);
					memset(tempEverything, '\0', sizeof(tempEverything));
					write(sock , sendClient , strlen(sendClient));
				}
			}
			else
			{
				permissionDenied(&tempEverything);
				strcpy(sendClient,tempEverything);
				puts(sendClient);
				memset(tempEverything, '\0', sizeof(tempEverything));
				write(sock , sendClient , strlen(sendClient));
			}
		}
		memset(sendClient, '\0', sizeof(sendClient));
		memset(client_message, '\0', sizeof(client_message));
	}

	if(read_size == 0)
	{
		puts("Client disconnected");					//incompatible state and logout user
		int b=0;
		for (b = 0; b < user_count; b++)
		{
			if(sock==user_list[b].socket_id)
			{
				strcpy(user_list[b].state,getTime());
			}
		}
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}

	//Free the socket pointer
	free(socket_desc);

	return 0;
}

int main(int argc, char *argv[])				//create socket,thread and partition for threads
{
	int socket_desc, new_socket, socket_address_length, *new_sock;
	struct sockaddr_in server, client;


	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		puts("Could not create socket");
		return 1;
	}
	if(argc<2)
	{
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(8888); // server will listen to 8888 port
	}
	else
	{
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(argv[1]);
		server.sin_port = htons(atoi(argv[2]));
	}
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("Binding failed");
		return 1;
	}

	listen(socket_desc, 3);

	puts("Waiting for incoming connections...");
	socket_address_length = sizeof(struct sockaddr_in);
	while((new_socket =
			accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&socket_address_length)) )
	{

		puts("Connection accepted");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = new_socket;

		if(pthread_create(&sniffer_thread, NULL, connection_handler,(void*)new_sock) < 0)
		{
			puts("Could not create thread");
			return 1;
		}

		puts("Handler assigned");
	}


	return 0;
}
void notFoundUser(char not_found[100])  				//json function user not found check
{
	json_object * jobj = json_object_new_object();
	json_object *not_found_us = json_object_new_string("notFoundUser");
	json_object_object_add(jobj, "command", not_found_us);
	char *result;
	result="User Not Found. You do not send message";
	json_object *jstring_fault = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_fault);
	strcpy(not_found,json_object_to_json_string(jobj));
}
void newMessage(char username[20],char nMessage[1000],char message_accept[1000]) //json function send message to client
{
	json_object * jobj = json_object_new_object();
	json_object *newmessage = json_object_new_string("newMessage");
	json_object_object_add(jobj, "command", newmessage);
	json_object *jstring_username = json_object_new_string(username);
	json_object_object_add(jobj, "from", jstring_username);
	json_object *jstring_message = json_object_new_string(nMessage);
	json_object_object_add(jobj, "text", jstring_message);
	strcpy(message_accept,json_object_to_json_string(jobj));
}
void answerLogin(char accept_login[100])						//json function check login state
{
	json_object * jobj = json_object_new_object();
	json_object *login = json_object_new_string("answerLogin");
	json_object_object_add(jobj, "command", login);
	char *result = ("Login is accepted");
	json_object *jstring_accept = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_accept);
	strcpy(accept_login,json_object_to_json_string(jobj));
}
void LoginFault(char fault_command[100],int a)                    //json function to send login state is not
{
	json_object * jobj = json_object_new_object();
	json_object *fault = json_object_new_string("LoginFault");
	json_object_object_add(jobj, "command", fault);
	char *result;
	if(a==1)
	{
		result = ("This user already online");
	}
	else if(a==2)
	{
		result = ("You already login different user name. Please logout and try again. ");
	}
	json_object *jstring_fault = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_fault);
	strcpy(fault_command,json_object_to_json_string(jobj));
}
void answerList(char listuserlist[2000])					//json function to list user and state
{
	json_object * jobj = json_object_new_object();
	json_object *answer = json_object_new_string("answerList");
	json_object_object_add(jobj, "command", answer);
	char result[2000];
	int i=0;
	for (i = 0; i < user_count; i++)
	{
		strcat(result,user_list[i].user_name);
		strcat(result," ");
		strcat(result,user_list[i].state);
		strcat(result,"\n");
	}
	json_object *jstring_lst = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_lst);
	strcpy(listuserlist,json_object_to_json_string(jobj));
	memset(result,'\0',sizeof(result));
}
void answerLogout(char accept_logout[100])				//json function logout control to send client
{
	json_object * jobj = json_object_new_object();
	json_object *logout = json_object_new_string("answerLogout");
	json_object_object_add(jobj, "command", logout);
	char *result = ("You are logout anymore");
	json_object *jstring_llogout = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_llogout);
	strcpy(accept_logout,json_object_to_json_string(jobj));
}
void permissionDenied(char permission[100])					//json function to access data control
{
	json_object * jobj = json_object_new_object();
	json_object *ppermission = json_object_new_string("permissionDenied");
	json_object_object_add(jobj, "command", ppermission);
	char *result = ("Permision Denied");
	json_object *jstring_ppermission = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_ppermission);
	strcpy(permission,json_object_to_json_string(jobj));
}
void answerControlSendFault(char answerControlSendF[100])		//json function to send client false
{
	json_object * jobj = json_object_new_object();
	json_object *answerCtrlSend = json_object_new_string("answerControlSendFault");
	json_object_object_add(jobj, "command", answerCtrlSend);
	char *result = ("But first,let you join");
	json_object *jstring_answerCtrlSend = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_answerCtrlSend);
	strcpy(answerControlSendF,json_object_to_json_string(jobj));
}

void answerControlSendCorrect(char answerControlSendC[100])			//json function to send client true
{
	json_object * jobj = json_object_new_object();
	json_object *answerCtrlSendC = json_object_new_string("answerControlSendCorrect");
	json_object_object_add(jobj, "command", answerCtrlSendC);
	char *result = ("true");
	json_object *jstring_answerCtrlSendC = json_object_new_string(result);
	json_object_object_add(jobj, "text", jstring_answerCtrlSendC);
	strcpy(answerControlSendC,json_object_to_json_string(jobj));
}
