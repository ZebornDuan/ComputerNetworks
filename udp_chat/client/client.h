#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include <assert.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <iostream> 
#include <algorithm>
#include <unordered_map>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

typedef struct User {
	string ip;
	int port;
}user;

class Client {
public:
	Client(int port);
	~Client();

	void run();
	void bind_address();
	void print_command();
	void print_user();

private:
	bool on;
	int socket_c, socket_b;
	int port_c, port_b;
	pthread_t beat_p, receive_p;
	string ip, name;
	sockaddr_in server, beat, client, test_beat;
	unordered_map<string, user> user_list;

	static void* send_beat(void* arguments);
	static void* receive(void* arguments);
};

#endif