# Socket Programming

# Client
client.cpp

# Server
LinkedList.h    
Myfiles.h
functions.h     
server.cpp    
message.txt    
login.txt    
rough_message.txt    
rough_login.txt     

# Client
1) Initialise Socket   
2) Connect to Server   
3) Thread - Receive from server
4) Thread - Send to server


# Server
1) Initialise, Bind, Listen Socket
2) Thread - Connect to client   
3) Thread - Receive from client - 1,    
    Thread - Receive from client - 2,   
     Thread - Receive from client - 3,    
      ......    
       Thread - Receive from client - n     

4) LinkedList - Class   
Append newly added clients (online)   
Remove left clients        (offline)    

5) Myfiles - Class    
Database to store login details and messages history
