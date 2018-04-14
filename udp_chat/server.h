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
#include <sys/select>

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

private:
	int socket_s;
	int socket_b;
	struct sockaddr_in server, beat, client;

	unordered_map<string, user> user_list;

	void* test_beat(void* arguments);
	void* test_online(void* arguments);
	void run();
};

#endif