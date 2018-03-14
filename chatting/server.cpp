#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <string.h> 
#include <string>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <fstream>
  
#define MYPORT 9000     // the port users will be connecting to  
  
#define BACKLOG 10      // how many pending connections queue will hold  
  
#define BUFFER_SIZE 1024

using namespace std;
  
int fd_accepted[BACKLOG];    // accepted connection fd  
int connection_amount;        // current connection amount

vector<string> user;
vector<string> pass;
vector<int> available;
vector<string> offline[BACKLOG];
string online[BACKLOG];

vector<string> split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;
     
    while(i != s.size()) {
        int flag = 0;
        while(i != s.size() && flag == 0) {
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;
                }
        }
         
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[j] == seperator[x]){
                    flag = 1;
                    break;
            }
            if(flag == 0)
                ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

void get_data() {
    ifstream infile;
    infile.open("data.txt");
    string s;
    bool tag = false;
    while(getline(infile, s)) {
        tag = !tag;
        if (tag) 
            user.push_back(s.substr(0, s.length()-1));
        else
            pass.push_back(s.substr(0, s.length()-1));
        // cout<<s<<endl;
    }
    infile.close();

    for (int index = 0;index<BACKLOG;index++) 
        online[index] = " ";
}

void showclient() {  
    int i;  
    printf("client amount: %d\n", connection_amount);  
    for (i = 0; i < BACKLOG; i++) {  
        printf("[%d]:%d  ", i, fd_accepted[i]);  
    }  
    printf("\n\n");  
}  
  
int main(void) {  
    int socket_fd, new_fd;                // listen on socket_fd, new connection on new_fd  
    struct sockaddr_in server_address;    // server address information  
    struct sockaddr_in client_address;    // connector's address information  
    socklen_t address_size;
    int yes = 1;  
    char buffer[BUFFER_SIZE];  
    int return_value;  
    int i; 

    get_data(); 
  
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
        perror("socket");  
        exit(1);  
    }  
  
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {  
        perror("setsockopt");  
        exit(1);  
    }  
      
    server_address.sin_family = AF_INET;         // host byte order  
    server_address.sin_port = htons(MYPORT);     // short, network byte order  
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // automatically fill with my IP  
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));  
  
    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {  
        perror("bind");  
        exit(1);  
    }  
  
    if (listen(socket_fd, BACKLOG) == -1) {  
        perror("listen");  
        exit(1);  
    }  
  
    printf("listen port %d\n", MYPORT);  
  
    fd_set file_description;  
    int max_socket;  
    struct timeval tv;  
  
    connection_amount = 0;  
    address_size = sizeof(client_address);  
    max_socket = socket_fd;  
    while (1) {  
        // initialize file descriptor set  
        FD_ZERO(&file_description);  
        FD_SET(socket_fd, &file_description);  
  
        // timeout setting  
        tv.tv_sec = 30;  
        tv.tv_usec = 0;  
  
        // add active connection to fd set  
        for (i = 0; i < BACKLOG; i++) {  
            if (fd_accepted[i] != 0) {  
                FD_SET(fd_accepted[i], &file_description);  
            }  
        }  
  
        return_value = select(max_socket + 1, &file_description, NULL, NULL, &tv);  
        if (return_value < 0) {  
            perror("select");  
            break;  
        } else if (return_value == 0) {  
            printf("timeout\n");  
            continue;  
        }  
  
        // check every fd in the set  
        for (i = 0; i < BACKLOG; i++) {  
            if (FD_ISSET(fd_accepted[i], &file_description)) {  
                return_value = recv(fd_accepted[i], buffer, sizeof(buffer), 0);  
                  
                if (return_value <= 0) {                          // client close  
                    printf("client[%d] close\n", i);  
                    close(fd_accepted[i]);  
                    FD_CLR(fd_accepted[i], &file_description);  
                    fd_accepted[i] = 0;
                    string change = "off " + online[i];
                    online[i] = " ";
                    available.push_back(i);
                    connection_amount--;
                    for (int d = 0;d<BACKLOG;d++)
                        if (online[d].compare(" ") != 0)
                            send(fd_accepted[d],change.c_str(),change.length(), 0);
                } else {                                          // receive data  
                    if (return_value < BUFFER_SIZE)  
                        memset(&buffer[return_value], '\0', 1);
                    string data = buffer;
                    vector<string> v = split(data, " ");
                    if (v[0].compare("login") == 0) {
                        bool tag = true;
                        for (int index = 0;index < user.size();index++) {
                            if (v[1].compare(user[index]) == 0) {
                                tag = false;
                                if (v[2].compare(pass[index]) == 0) {
                                    string change = "on " + v[1];
                                    for (int d = 0;d<BACKLOG;d++)
                                        if (online[d].compare(" ") != 0)
                                            send(fd_accepted[d],change.c_str(),change.length(), 0);
                                    string message = "ok\n";
                                    online[i] = v[1];
                                    for (int d = 0;d<BACKLOG;d++)
                                        if (online[d].compare(" ") != 0)
                                            message = message + online[d] + "\n";
                                    message = message.substr(0, message.length()-1);
                                    send(fd_accepted[i],message.c_str(),message.length(), 0);
                                } else {
                                    string message = "wrong\n";
                                    send(fd_accepted[i],message.c_str(),message.length(), 0);
                                }
                                break;
                            }
                        }

                        if (tag) {
                            string message = "wrong\n";
                            send(fd_accepted[i],message.c_str(),message.length(), 0);
                        }
                    }

                    if (v[0].compare("search") == 0) {
                        string message = "search ";
                        for (int index = 0;index<user.size();index++) {
                            message += user[index];
                            message += " ";
                        }
                        send(fd_accepted[i],message.c_str(),message.length(), 0);
                    }

                    if (v[0].compare("add") == 0) {
                        bool tag = true;
                        string toAdd;
                        for (int index = 1;index<v.size();index++)
                            toAdd = toAdd + v[index] + " ";
                        toAdd = toAdd.substr(0, toAdd.length()-1);
                        for (int index = 0;index<user.size();index++) {
                            if (user[index].compare(toAdd) == 0) {
                                tag = false;
                                string message = "add ok " + toAdd;
                                send(fd_accepted[i],message.c_str(),message.length(), 0);
                                break;
                            }
                        }
                        if (tag) {
                            string message = "add no";
                            send(fd_accepted[i],message.c_str(),message.length(), 0);
                        }
                    }

                    if (v[0].compare("send") == 0) {
                        bool tag = true;
                        string message;
                        string toSend;
                        for (int index = 2;index<v.size();index++)
                            toSend = toSend + v[index] + " ";
                        toSend = toSend.substr(0, toSend.length()-1);
                        message = "get " + online[i] + " " + toSend;
                        for (int number = 0;number<BACKLOG;number++) {
                            if (online[number].compare(v[1]) == 0) {
                                tag = false;
                                send(fd_accepted[number],message.c_str(),message.length(), 0);
                                break;
                            }
                        }

                        if (tag) {
                            for (int d = 0;d<user.size();d++)
                                if (user[d].compare(v[1]) == 0) {
                                    offline[d].push_back(message);
                                    cout<<message;
                                    break;
                                }
                        }
                    }

                    if (v[0].compare("file") == 0) {
                        bool tag = true;
                        string message = "file " + online[i] + " " + v[1];
                        for (int number = 0;number<BACKLOG;number++) {
                            if (online[number].compare(v[2]) == 0) {
                                send(fd_accepted[number],message.c_str(),message.length(), 0);
                                recv(fd_accepted[number], buffer, sizeof(buffer), 0);
                                tag = false;
                                while(true) {
                                    return_value = recv(fd_accepted[i], buffer, sizeof(buffer), 0);
                                    if (return_value < BUFFER_SIZE) {
                                        send(fd_accepted[number],buffer,return_value, 0);
                                        break;
                                    } else {
                                        send(fd_accepted[number],buffer,return_value, 0);
                                        memset(buffer,0,sizeof(char)*BUFFER_SIZE);
                                    }
                                }
                                break;
                            }
                        }

                        if (tag) {
                            string path = "./download/" + v[1];
                            FILE* file = fopen(path.c_str(), "a+b");
                            while(true) {
                                return_value = recv(fd_accepted[i], buffer, sizeof(buffer), 0);
                                if (return_value < BUFFER_SIZE) {
                                    fwrite(buffer,sizeof(char),return_value,file);
                                    break;
                                } else {
                                    fwrite(buffer,sizeof(char),return_value,file);
                                    memset(buffer,0,sizeof(char)*BUFFER_SIZE);
                                }
                            }
                            fclose(file);
                            for (int d = 0;d<user.size();d++)
                                if (user[d].compare(v[2]) == 0) {
                                    offline[d].push_back(message);
                                    break;
                                }
                        }
                    }

                    if (v[0].compare("get") == 0) {
                        bool tag = false;
                        for (int d = 0;d<user.size();d++)
                            if (user[d].compare(online[i]) == 0) {
                                if (!offline[d].empty()) {
                                // for (int c = 0;c<offline[d].size();c++) {
                                    tag = true;
                                    // cout<<offline[d][c];
                                    send(fd_accepted[i],offline[d][0].c_str(),offline[d][0].length(), 0);
                                    if (offline[d][0][0] == 'f') {
                                        return_value = recv(fd_accepted[i], buffer, sizeof(buffer), 0);
                                        string filename = split(offline[d][0]," ")[2];
                                        string path = "./download/" + filename;
                                        FILE* file = fopen(path.c_str(), "rb");
                                        while(true) {
                                            int value = fread(buffer,sizeof(char),BUFFER_SIZE,file);
                                            if (value < BUFFER_SIZE) {
                                                send(fd_accepted[i],buffer,value, 0);
                                                break;
                                            } else {
                                                send(fd_accepted[i],buffer,value, 0);
                                                memset(buffer,0,sizeof(char)*BUFFER_SIZE);
                                            }
                                        }
                                        fclose(file);
                                        remove(path.c_str());
                                    }
                                    offline[d].erase(offline[d].begin());
                                }
                                break;    
                            }
                        if (!tag) {
                            string message = "above ";
                            send(fd_accepted[i],message.c_str(),message.length(), 0);
                        }
                    }
                    
                    printf("client[%d] send:%s\n", i, data.c_str());
                }  
            }  
        }  
  
        // check whether a new connection comes  
        if (FD_ISSET(socket_fd, &file_description)) {  
            new_fd = accept(socket_fd, (struct sockaddr *)&client_address, &address_size);  
            if (new_fd <= 0) {  
                perror("accept");  
                continue;  
            }  
  
            // add to fd queue  
            if (connection_amount < BACKLOG) {
                int position;
                if (!available.empty()) {
                    position = available[available.size()-1];
                    available.pop_back();
                    fd_accepted[position] = new_fd;
                    connection_amount++;
                } else {
                    position = connection_amount;
                    fd_accepted[connection_amount++] = new_fd;
                }
                printf("new connection client[%d] %s:%d\n", position,
                        inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                if (new_fd > max_socket)  
                    max_socket = new_fd;
            }  
            
            else {  
                printf("max connections arrive, exit\n");  
                send(new_fd, "bye", 4, 0);  
                close(new_fd);  
                break;
            }  
        }  
        showclient();  
    }  
  
    // close other connections  
    for (i = 0; i < BACKLOG; i++) {  
        if (fd_accepted[i] != 0) {  
            close(fd_accepted[i]);  
        }  
    }  
  
    exit(0);  
}  

