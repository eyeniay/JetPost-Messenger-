#include <stdio.h>
#include <string.h>
#include <json/json.h> // for json
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>

// #### Variables ####
static int socket_desc;
pthread_mutex_t lock;
char command[30];                   //commands from server
char username[20];                  //username from server
char from[20];                      //sender
char text[100];                     //return message from server
char message[1000], client_message[2000];;    
char tempEverything[100];           //to json object
char tempEverything2[1000];
char tempEverything5[1000];

//#### Function prototypes ####
void menu();                              //Using functions shortcut
void *listening(void *desc);              //thread create function
void json_parse(json_object * jobj);      //parse json
void sendMessage(char touser[20],char txt_message[1000]);   //send server message
void getList(char tempEverything4[2000]);     //get user list
void Login(char tempEverything3[100]);        //login
void Logout(char tempEverything4[100]);       //logout
void controlSendLog(char tempEverything1[100]);       //to control before send message


void json_parse(json_object * jobj)
{
  enum json_type type;

  json_object_object_foreach(jobj, key, val)        //traverse json and get value
  {
    type=json_object_get_type(val);
    switch(type)
    {
    case json_type_string:
      if(strcmp(key,"command")==0)
      {
        strcpy(command,json_object_get_string(val));
      }

      else if(strcmp(key,"from")==0)
      {
        strcpy(from,json_object_get_string(val));
      }
      else if(strcmp(key,"text")==0)
      {
        strcpy(text,json_object_get_string(val));
      }
      break;
    }
  }
}
void *listening(void *desc)                         //create thread function
{
  char server_reply[2000];

  //Receive a reply from the server

  int read_size;

  while( (read_size = recv(socket_desc, client_message , 2000 , 0)) > 0 )         //listen port
  {
    json_object * jobj = json_tokener_parse(client_message);

    json_parse(jobj);

    if(strcmp(command,"answerLogin")==0)
    {
      puts(text);
    }

    else if(strcmp(command,"answerList")==0)
    {
      puts(text);
    }

    else if(strcmp(command,"LoginFault")==0)
    {
      puts(text);
    }

    else if(strcmp(command,"permissionDenied")==0)
    {
      puts(text);
    }


    else if(strcmp(command,"answerLogout")==0)
    {
      puts(text);
    }

    else if(strcmp(command,"notFoundUser")==0)
    {
      puts(text);
    }

    else if(strcmp(command,"newMessage")==0)            //save state for user screen
    {
      pthread_mutex_lock(&lock);
      char result[1000];
      strcat(result,from);
      strcat(result," sent you new message.Message is --- ");
      strcat(result,text);
      puts(result);
      memset(result,'\0',sizeof(result));
      pthread_mutex_unlock(&lock);
    }
    memset(client_message, '\0', sizeof(client_message));

  }
  if(read_size == 0)
  {
    puts("Client disconnected");
    fflush(stdout);
  }
  else if(read_size == -1)
  {
    perror("recv failed");
  }
  //Free the socket pointer

  free(desc);
  return 0;
}

int main(int argc, char *argv[])                      //main function and create communication
{
  struct sockaddr_in server;
  //Create socket

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
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

  //Connect to remote server

  if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    puts("Connection error");
    return 1;
  }
  puts("Connected");

  pthread_t thread;
  int err;

  err = pthread_create(&thread, NULL, &listening, (void*)socket_desc);
  if (err != 0)
    printf("\ncan't create thread :[%s]", strerror(err));

  while(1)
  {
    memset(message, '\0', sizeof(message));

    menu();
    sleep(1);
    printf("%s\n",message);
    printf("%s\n","Enter selection menu number : ");
    scanf("%s" , message);


    char sendServer[100];

    if (strcmp(message,"1")==0)                     //login to user
    {
      pthread_mutex_lock(&lock);
      printf("%s",tempEverything);
      printf("Write username : ");
      scanf("%s" , tempEverything);

      Login(&tempEverything);                       //check user state
      strcpy(sendServer,tempEverything);
      memset(tempEverything, '\0', sizeof(tempEverything));
      pthread_mutex_unlock(&lock);
    }
    else if (strcmp(message,"2")==0)                //get user list
    {
      getList(&tempEverything); 
      strcpy(sendServer,tempEverything);
      memset(tempEverything, '\0', sizeof(tempEverything));
    }
    else if (strcmp(message,"3")==0)                //send new message to others
    {

      pthread_mutex_lock(&lock);
      controlSendLog(&tempEverything);              //user login state check
      strcpy(sendServer,tempEverything);
      write(socket_desc, sendServer , strlen(sendServer));
      sleep(3);

      memset(tempEverything, '\0', sizeof(tempEverything));
      memset(sendServer, '\0', sizeof(sendServer));

      if(strcmp(text,"true")==0)
      {

        printf("%s",tempEverything);
        printf("Enter your friends name to send message : ");
        scanf("%s" , tempEverything);

        char c;
        int i;
        printf("%s", "Write a message : ");
        scanf("%s", tempEverything2);
        i = strlen(tempEverything2);      // length of user input till first space
        do
        {
          scanf("%c", &c);
          tempEverything2[i++] = c;       // reading characters after first space (including it)
        } while (c != '\n');     // until user hits Enter
        tempEverything2[i - 1] = '\0';       // string terminating

        sendMessage(&tempEverything,&tempEverything2);
        strcpy(sendServer,tempEverything2);

        memset(tempEverything, '\0', sizeof(tempEverything));
        memset(tempEverything2, '\0', sizeof(tempEverything2));
      }
      else
      {
        puts(text);
      }
      pthread_mutex_unlock(&lock);
    }
    else if (strcmp(message,"4")==0)                //logout user 
    {
      Logout(&tempEverything);                      //check state and logout
      strcpy(sendServer,tempEverything);

      memset(tempEverything, '\0', sizeof(tempEverything));
    }
    else
    {
      puts("Please enter correctly menu number");
    }
    if( send(socket_desc , sendServer , strlen(sendServer) , 0) < 0)
    {
      puts("Send failed");
      memset(sendServer, '\0', sizeof(sendServer));
      return 1;
    }
    memset(sendServer, '\0', sizeof(sendServer));
  }
  close(socket_desc);

  return 0;
}
void menu()
{
  printf("%s\n","1) Login ");
  printf("%s\n","2) Get User List");
  printf("%s\n","3) Send Message");
  printf("%s\n","4) Logout");
}
void sendMessage(char touser[20],char txt_message[1000])    //json function to send message server
{
  json_object * jobj = json_object_new_object();

  json_object *sendMessage = json_object_new_string("sendMessage");

  json_object_object_add(jobj, "command", sendMessage);

  json_object *jstring_to = json_object_new_string(touser);

  json_object_object_add(jobj, "to", jstring_to);

  json_object *jstring_message = json_object_new_string(txt_message);

  json_object_object_add(jobj, "text", jstring_message);

  memset(txt_message, '\0', sizeof(txt_message));
  strcpy(txt_message,json_object_to_json_string(jobj));
}
void getList(char tempEverything4[2000])            //json function to send user list send server
{
  json_object * jobj = json_object_new_object();

  json_object *list = json_object_new_string("getList");

  json_object_object_add(jobj, "command", list);

  strcpy(tempEverything4,json_object_to_json_string(jobj));
}

void Login(char tempEverything3[100])           //json function to send login state to server
{
  json_object * jobj = json_object_new_object();

  json_object *login = json_object_new_string("login");

  json_object_object_add(jobj, "command", login);

  json_object *jstring_username = json_object_new_string(tempEverything3);

  json_object_object_add(jobj, "username", jstring_username);


  strcpy(tempEverything3,json_object_to_json_string(jobj));



}
void Logout(char tempEverything2[100])          //json function to send logout user for server
{
  json_object * jobj = json_object_new_object();

  json_object *logout = json_object_new_string("logout");

  json_object_object_add(jobj, "command", logout);

  strcpy(tempEverything2,json_object_to_json_string(jobj));

}
void controlSendLog(char tempEverything1[100])        //check control send login info and state
{
  json_object * jobj = json_object_new_object();

  json_object *controlSendLogg = json_object_new_string("controlSendLog");

  json_object_object_add(jobj, "command", controlSendLogg);

  strcpy(tempEverything1,json_object_to_json_string(jobj));

}

