# JetPost-Messenger-
In this application users will connect to the server, get logged in/ logged out, browse the whole user list and can send a message to any of the users.

SERVER SIDE 
===
Server has to keep a user list and messages. Any user that will be logged in for the first time must be added to this list.  When a user wants to see the user list, server will respond with the whole list including if the user is online or lastSeenTime.

CLIENT SIDE 
===
There are four commands for users that are login, getList, logout and message.Basing on these client message-format design and implement server side message protocol(Protocols.txt).

-to run server side:

![ScreenShot](http://imageshack.com/a/img921/3077/4eLYZL.png)
![ScreenShot](http://imageshack.com/a/img924/38/m11Zut.png)

to run client side:

![ScreenShot](http://imageshack.com/a/img923/9059/yXgUlx.png)
![ScreenShot](http://imageshack.com/a/img921/816/McCI7q.png)

Login
===

![ScreenShot](http://imageshack.com/a/img922/9175/rCmjQc.png)

Send and receive a message(If user is offline,message added user's message box and user can see it when online)
===

![ScreenShot](http://imageshack.com/a/img921/1001/DZ19Jt.png)
![ScreenShot](http://imageshack.com/a/img922/8870/sEmZvw.png)

-user list:

![ScreenShot](http://imageshack.com/a/img921/9946/ggWVg3.png)

License
===

    Copyright (C) 2013 Ethem Yeniay <ethemyeniay@gmail.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
