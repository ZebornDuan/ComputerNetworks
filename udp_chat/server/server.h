#ifndef SERVER_H
#define SERVER_H

#include <cstdio>
#include <assert.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream> 
#include <algorithm>
#include <unordered_map>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
using namespace std;

typedef struct User {
	string ip;
	int port;
	time_t time;
}user;

class Server {
public:
	Server();
	~Server();

	void bind_address();
	void run();

private:
	int socket_s;
	int socket_b;
	sockaddr_in server, beat;
	sockaddr_in client;
	pthread_t beat_p, test_beat_p;

	unordered_map<string, user> user_list;

	static void* test_beat(void* arguments);
	static void* test_online(void* arguments);

	void inform(string message);
	
};

#endif