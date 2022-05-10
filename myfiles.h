#pragma once
class Myfiles {
private:
	FILE* stream1, * stream2;
    errno_t err;
	char login[10] = "login.txt";
	char message[12] = "message.txt";
    char roughmessage[18] = "rough_message.txt";
    int wait = 0;
    int wait1 = 0;
public: 
	void getmessage() {
		
	}
	void setmessage() {

	}
    void fetchmessageswhileoffline(char thistoname[15], SOCKET thisClientSocket) {
        while (this->wait1) {
            cout << "fetchmessageswhileoffline waiting...." << endl;
            Sleep(50);
        }
        this->wait1 = 1;
        char buff[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4] = "\0";
        int wordscount = 0;
        int boolean = 0;
        err = fopen_s(&stream1, this->message, "r");
        if (err == 0) {
            // empty rough message file
            err = fopen_s(&stream2, this->roughmessage, "w");
            char emty[2] = "\0";
            if (err == 0)  fprintf(stream2, emty);
            if (stream2) fclose(stream2);
            int i;
            // open rough message file in append mode
            err = fopen_s(&stream2, this->roughmessage, "a");
            if (err == 0) {
                int whilecount = 0;
                while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                    //cout << buff << endl;
                    whilecount++;
                    int j = 0;
                    char message1[DEFAULT_BUFLEN] = "\0",
                        message2[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4] = "\0";
                    char fromname[15] = "\0", toname[15] = "\0";
                    int count = 0, frombool = 0, tobool = 0;
                    int i_i = 0;
                    if (whilecount != 1) {
                        message2[0] = ' ';
                        i_i = 1;
                    }
                    for (i = 0; i < strlen(buff); i++) {
                        message2[i + i_i] = buff[i];
                        if (buff[i] == '_') {
                            count++;
                            j = 0;
                            if (count == 3) {
                                break;
                            }
                            continue;
                        }
                        if (count == 0 && j < 15) {
                            toname[j] = buff[i];
                            toname[j + 1] = '\0';
                            j++;
                        }
                        else if (count == 1 && j < 15) {
                            fromname[j] = buff[i];
                            fromname[j + 1] = '\0';
                            j++;
                        }
                        else if (count == 2 && j < DEFAULT_BUFLEN) {
                            if (buff[i] == '*') buff[i] = ' ';
                            message1[j] = buff[i];
                            message1[j + 1] = '\0';
                            j++;
                        }
                    }

                    //cout << "from: " << fromname << "to: " << toname << "message: " << message1 << endl;
                    if (strcmp(toname, thistoname) == 0) {
                        if (buff[i + 1] == 'n' && buff[i + 2] == 'o' && buff[i + 3] == 't') {
                            cout << "from: " << fromname << " to: " << toname << " message: " << message1 << endl;
                            j = strlen(message1);
                            message1[j] = '(';
                            j++;
                            int storei = i;
                            for (i = 0; i < strlen(fromname); i++) {
                                message1[j + i] = fromname[i];
                            }
                            message1[j + i] = ')';
                            j++;
                            message1[j + i] = '\0';
                            int value = sendd(thisClientSocket, message1, toname);
                            //cout << value << endl;
                            i = storei;
                            if (value) {
                                message2[i + 1 + i_i] = 's';
                                message2[i + 2 + i_i] = 'e';
                                message2[i + 3 + i_i] = 'n';
                            }
                            else {
                                message2[i + 1 + i_i] = 'n';
                                message2[i + 2 + i_i] = 'o';
                                message2[i + 3 + i_i] = 't';
                            }
                        }
                    }
                    else {
                        message2[i + 1 + i_i] = buff[i + 1];
                        message2[i + 2 + i_i] = buff[i + 2];
                        message2[i + 3 + i_i] = buff[i + 3];
                    }
                    message2[i + 4 + i_i] = '\0';
                    cout << whilecount << ") " << message2 << endl;
                    fprintf(stream2, message2);
                }
            }
            if (stream2) fclose(stream2);
            if (stream1) fclose(stream1);
            boolean = 1;
        }
        else {

        }
        // roughmessage to message
        if (boolean) {
            err = fopen_s(&stream1, this->roughmessage, "r");
            if (err == 0) {
                // empty message file
                err = fopen_s(&stream2, this->message, "w");
                char emty[2] = "\0";
                if (err == 0) fprintf(stream2, emty);
                if (stream2) fclose(stream2);

                err = fopen_s(&stream2, this->message, "a");
                if (err == 0) {
                    int whilecount = 0;
                    while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                        char newbuff[DEFAULT_BUFLEN] = "\0";
                        if (whilecount) {
                            sprintf_s(newbuff, " %s\0", buff);
                        }
                        else {
                            sprintf_s(newbuff, "%s\0", buff);
                        }
                        fprintf(stream2, newbuff);
                        whilecount++;
                    }
                }
            }
        }
        if (stream2) fclose(stream2);
        if (stream1) fclose(stream1);

        Sleep(100);
        this->wait1 = 0;
    }
    int sendMessageUsingName(char thisname[15], char message[DEFAULT_BUFLEN], char fromname[15]) {
        if (strcmp(thisname, fromname) == 0) {
            return 1;
        }
        while (this->wait) {
            cout << "getname waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        char name[15];
        int i;
        char finalmessage[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4];
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            char buff[15 + 1 + 15] = "\0";
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                for (i = 0; i < strlen(buff) && i < 15; i++) {
                    if (buff[i] == '_') {
                        break;
                    }
                    name[i] = buff[i];
                }
                name[i] = '\0';
                if (strcmp(name, thisname) == 0) {
                    boolean = 1;
                    int j;
                    for (j = 0; j < strlen(message); j++) {
                        if (message[j] == ' ') {
                            message[j] = '*';
                        }
                    }
                    message[j] = '\0';
                    sprintf_s(finalmessage, " %s_%s_%s_not\0", thisname, fromname, message);
                    //cout << finalmessage << endl;
                    //cout << endl;
                    break;
                }
            }
            //cout << "The file '" << login << "' was opened\n";
        }
        else {
            cout << "SendMessageUsingName " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        if (boolean) {
            while (this->wait1) {
                cout << "getname waiting...." << endl;
                Sleep(50);
            }
            this->wait1 = 1;
            err = fopen_s(&stream1, this->message, "a");
            if (err == 0) {
                cout << "stored in file: " << finalmessage << endl;
                fprintf(stream1, finalmessage);
            }
            else {
                cout << "SendMessageUsingName " << this->message << "' not opened\n";
            }
            if (stream1)
            {
                err = fclose(stream1);
            }
            Sleep(100);
            this->wait1 = 0;
        }
        return boolean;
    }
    int getname(char thisname[15]) {
        while (this->wait) {
            cout << "getname waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        char name[15];
        int i;
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            char buff[15 + 1 + 15] = "\0";
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                for (i = 0; i < strlen(buff) && i < 15; i++) {
                    if (buff[i] == '_') {
                        break;
                    }
                    name[i] = buff[i];
                }
                name[i] = '\0';
                if (strcmp(name, thisname) == 0) {
                    boolean = 1;
                    //cout << buff;
                    //cout << endl;
                    break;
                }
            }
            //cout << "The file '" << login << "' was opened\n";
        }
        else {
            cout << "GetName " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        return boolean;
    }
	int getid(char getbuff[15 + 1 + 15]) {
        while (this->wait) {
            cout << "getid waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            char buff[15 + 1 + 15] = "\0";
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                if (strcmp(getbuff, buff) == 0) {
                    boolean = 1;
                    //cout << buff;
                    //cout << endl;
                    break;
                }
            }
            //cout << "The file '" << login << "' was opened\n";
        }
        else {
            cout << "GetID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        return boolean;
	}
	void setid(char buff[15 + 1 + 15]) {
        while (this->wait) {
            cout << "setid waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        char newbuff[1 + 15 + 1 + 15];
        sprintf_s(newbuff, " %s\0", buff);
        err = fopen_s(&stream1, login, "a");
        if (err == 0)
        {
            fprintf(stream1, newbuff);
            //cout << "The file '" << login << "' was opened\n";
        }
        else
        {
            cout << "SetID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
	}
};