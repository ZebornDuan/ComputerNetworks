#ifndef SERVER_H
#define SERVER_H

#include <cstdio>
#include <assert>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <pthread>
#include <sstream>
#include <iostream> 
#include <algorithm>
#include <unordered_map>
#include <sys/socket>
#include <sys/types>
#include <sys/time>

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

	void bind();
	void run();

private:
	int socket_s;
	int socket_b;
	struct sockaddr_in server, beat, client;
	pthread_t beat_p, test_beat_p;

	unordered_map<string, user> user_list;

	void* test_beat(void* arguments);
	void* test_online(void* arguments);

	void inform(string message);
	
};

#endif