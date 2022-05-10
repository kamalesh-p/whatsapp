#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>

using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

struct addrinfo* result = NULL;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

const char* sendbuf;

/**********
    
    kamalesh

**********/ 

thread* t, * t1, * t2;
// Commands for Client
const int command_len = 4;
char commands[command_len][20] = {
    "help",
    "disconnect",
    "name",
    "others"
};
char statement[DEFAULT_BUFLEN + 19] = "";

const int namelistlength = 5;
char namelist[namelistlength + 1][15] = {
    "kamalesh\0",
    "bye\0",
    "jolly\0",
    "send\0",
    "name\0",
};
char passlist[namelistlength + 1][15] = {
    "thisissample\0",
    "thisissample\0",
    "thisissample\0",
    "thisissample\0",
    "password\0",
};

static int threadcount = 0;
int CountClientNumber = 0;

int receivee(SOCKET ClientSocketReceive, int ClientNumber, char ReceiveName[15]);
int sendd(SOCKET ClientSocketSend, char SendStatement[DEFAULT_BUFLEN], char SendName[15]);
void accept_client();
void client_left(SOCKET ClientSocketLeft);


void threadcountfunc() {
    //cout << "******" << std::thread::hardware_concurrency << "******" << endl;
}


// Linked List
#include "LinkedList.h"
Clients* client;

// Myfiles
#include "myfiles.h"
Myfiles* myfiles;

class threadreceive {
public:
    SOCKET ClientSocketReceive;
    int ClientNumber;
    char name[15];
    threadreceive(SOCKET ClientSocketReceive, int ClientNumber, char name[15]) {
        this->ClientSocketReceive = ClientSocketReceive; 
        this->ClientNumber = ClientNumber;
        strcpy_s(this->name, name);
    }
    void operator()(int num) {
        cout << "Started receiving from " << name << endl;
        while (receivee(ClientSocketReceive, ClientNumber, name));
    }
};

char namestore[15];

class threadGetClient {
public:
    void operator()(int num) {
        while (1) {
            accept_client();
            statement[0] = '\0';
            t2 = new thread(threadreceive(ClientSocket, CountClientNumber, statement), 1);
            threadcount++;
            threadcountfunc();

            t1 = new thread(threadGetClient(), 1);
            threadcount++;
            threadcountfunc();

            t1->join();
            threadcount--;
            threadcountfunc();

            t2->join();
            threadcount--;
            threadcountfunc();
        }
    }
};

/*
    
    char buff[] = "new_pass";
    myfiles = new Myfiles;
    myfiles->setid(buff);

*/

int receivee(SOCKET ClientSocketReceive, int CountClientNumber, char ReceiveName[15]) {
    iResult = recv(ClientSocketReceive, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        int j, i;
        cout << "From " << ReceiveName << " Received: ";
        sprintf_s(statement, "%s\0", ""); // empty
        // one to one message 
        char namechar[] = "name_";
        int namebool = 1;
        int name_k = strlen(namechar);
        // one to many message 
        char nameschar[] = "names_";
        int namesbool = 1;
        int names_k = strlen(nameschar);
        // login 
        char loginchar[] = "login_";
        int loginbool = 1;
        int login_k = strlen(loginchar);
        // signup
        char signupchar[] = "signup_";
        int signupbool = 1;
        int signup_k = strlen(signupchar);
        for (i = 0; i < iResult; i++) {
            cout << recvbuf[i];
            statement[i] = recvbuf[i];
            // one to one message 
            if (iResult < name_k) {
                namebool = 0;
            }
            else if (i < name_k && namebool) {
                if (namechar[i] != recvbuf[i]) {
                    namebool = 0;
                }
            }
            // one to many message 
            if (iResult < names_k) {
                namesbool = 0;
            }
            else if (i < names_k && namesbool) {
                if (nameschar[i] != recvbuf[i]) {
                    namesbool = 0;
                }
            }
            // login 
            if (iResult < login_k) {
                loginbool = 0;
            }
            else if (i < login_k && loginbool) {
                if (loginchar[i] != recvbuf[i]) {
                    loginbool = 0;
                }
            }
            // signup
            if (iResult < signup_k) {
                signupbool = 0;
            }
            else if (i < signup_k && signupbool) {
                if (signupchar[i] != recvbuf[i]) {
                    signupbool = 0;
                }
            }
        }
        statement[i] = '\0';
        cout << endl;
        if (!signupbool && !loginbool && !strlen(ReceiveName) && ReceiveName[0] == '\0') {
            char message[] = "#Login";
            char tempname[15];
            sprintf_s(tempname, "Client-%d\0", CountClientNumber);
            return sendd(ClientSocketReceive, message, tempname);
        }
        /*
            signup_name_passsword
        */
        if (signupbool) {
            signup_k = strlen(signupchar);
            char name[15], name_password[15 + 1 + 15];
            signupbool = 1;
            j = 0;
            int l = 0;
            for (i = signup_k; i < iResult; i++) {
                name_password[l] = recvbuf[i];
                l++;
                if (signupbool && j < 15) {
                    if (recvbuf[i] == '_') {
                        signupbool = 0;
                    }
                    else {
                        name[j] = recvbuf[i];
                        j++;
                    }
                }
            }
            name[j] = '\0';
            name_password[l] = '\0';
            namebool = myfiles->getname(name);
            if (!namebool) {
                myfiles->setid(name_password);
                //cout << name << " " << password;
                for (i = 0; i < strlen(name); i++) {
                    ReceiveName[i] = name[i];
                }
                ReceiveName[i] = '\0';
                client->changename(ClientSocketReceive, ReceiveName);
                myfiles->fetchmessageswhileoffline(ReceiveName, ClientSocketReceive);
            }
            else {
                char message[] = "#User Already Exist";
                return sendd(ClientSocketReceive, message, message);
            }
        }
        /*
            login_name_passsword
        */
        else if (loginbool) {
            login_k = strlen(loginchar);
            char name[15], name_password[15 + 1 + 15];
            loginbool = 1;
            j = 0;
            int l = 0;
            for (i = login_k; i < iResult; i++) {
                name_password[l] = recvbuf[i];
                l++;
                if (loginbool && j < 15) {
                    if (recvbuf[i] == '_') {
                        loginbool = 0;
                    }
                    else {
                        name[j] = recvbuf[i];
                        j++;
                    }
                }
            }
            name[j] = '\0';
            name_password[l] = '\0';
            namebool = myfiles->getid(name_password);
            if (namebool) {
                //cout << name << " " << password;
                for (i = 0; i < strlen(name); i++) {
                    ReceiveName[i] = name[i];
                }
                ReceiveName[i] = '\0';
                client->changename(ClientSocketReceive, ReceiveName);
                myfiles->fetchmessageswhileoffline(ReceiveName, ClientSocketReceive);
            }
            else {
                char message[] = "#Fail";
                return sendd(ClientSocketReceive, message, message);
            }
        }
        // other
        else if (strcmp(statement, "others") == 0) {
            client->getonline(ClientSocketReceive, ReceiveName);
        }
        /*
            names_(client name),(client name),(client name)..._(statement)
            names_Client-1,Client-2,Client-3,_hello world
        */
        else if (namesbool) {
            names_k = strlen(nameschar);
            char message[DEFAULT_BUFLEN - 15*5];
            namesbool = 1;
            j = 0;
            int l = 0;
            int count = 0;
            char names[10][15];
            for (i = names_k; i < iResult; i++) {
                if (namesbool) {
                    if (recvbuf[i] == '_') {
                        namesbool = 0;
                    }
                    else {
                        if (recvbuf[i] == ',') {
                            names[count][j] = '\0';
                            j = 0;
                            count++;
                        }
                        else {
                            names[count][j] = recvbuf[i];
                            j++;
                        }
                    }
                }
                else {
                    message[l] = recvbuf[i];
                    l++;
                }
            }
            message[l] = '\0';
            for (i = 0; i < count; i++) {
                cout << names[i] << endl; 
                int value = client->sendoneperson(ClientSocketReceive, names[i], message, ReceiveName);
                //cout << value << endl;
                if (!value)
                {
                    //cout << "here" << endl;
                    value = myfiles->sendMessageUsingName(names[i], message, ReceiveName);
                    //cout << value << endl;
                    if (value) {
                        char newmessage[50];
                        sprintf_s(newmessage, "message sent to %s\0", names[i]);
                        sendd(ClientSocketReceive, newmessage, ReceiveName);
                    }
                }
                else{
                    value = myfiles->sendMessageUsingName(names[i], message, ReceiveName);
                }
            }
        }
        /*
            name_(client name)_(statement)
            name_Client-1_hello world
        */
        else if (namebool) {
            name_k = strlen(namechar);
            char name[15], message[DEFAULT_BUFLEN];
            namebool = 1;
            j = 0;
            int l = 0;
            for (i = name_k; i < iResult; i++) {
                if (namebool) {
                    if (recvbuf[i] == '_') {
                        namebool = 0;
                    }
                    else {
                        name[j] = recvbuf[i];
                        j++;
                    }
                }
                else {
                    message[l] = recvbuf[i];
                    l++;
                }
            }
            name[j] = '\0';
            message[l] = '\0';
            int value = client->sendoneperson(ClientSocketReceive, name, message, ReceiveName);
            if (!value)
            {
                //cout << "here" << endl;
                value = myfiles->sendMessageUsingName(name, message, ReceiveName);
                //cout << value << endl;
                if (value) {
                    char newmessage[50];
                    sprintf_s(newmessage, "message sent to %s\0", name);
                    sendd(ClientSocketReceive, newmessage, ReceiveName);
                }
            }
        }
    }
    else if (iResult == 0) {
        printf("Connection closing...\n");
        client_left(ClientSocketReceive);
    }
    else {
        client_left(ClientSocketReceive);
        // left online
        return 0;
    }
    return 1;
}


void client_left(SOCKET ClientSocketLeft) {
    client->remove(ClientSocketLeft);
    cout << "client left the clan" << endl;
}

int sendd(SOCKET ClientSocketSend, char SendStatement[DEFAULT_BUFLEN], char SendName[15]) {
    // Send
    const char* sendbuf = SendStatement;
    iSendResult = send(ClientSocketSend, sendbuf, strlen(sendbuf), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("ERROR: send failed\n");
        client_left(ClientSocketSend);
        return 0;
        // left online
    }
    cout << "sent to " << SendName << ": "<<sendbuf << endl;
    return 1;
}

void accept_client() {
    // Accept a client socket
    cout << "Server Listening for new Client" << endl;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }
    cout << "Client Accepted" << endl;
    CountClientNumber++;
    sprintf_s(statement, "%s\0", namestore);
    client->append(ClientSocket, statement);
}


int __cdecl main(void)
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    printf("Server start\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    //SOMAXCONN
    iResult = listen(ListenSocket, 1);
    //cout << "iresult" << iResult << endl;
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

/**********

    kamalesh

**********/
    myfiles = new Myfiles();
    client = new Clients();
    t = new thread(threadGetClient(), 1);
    //threadcount++;
    //threadcountfunc();
    t->join();
    threadcount--;
    threadcountfunc();
    delete(t);
    return 100;
}