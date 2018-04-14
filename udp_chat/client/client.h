#ifndef CLIENT_H
#define CLIENT_H

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

class Client {
public:
	Client(int port);
	~Client();

	void run();
	void bind();
	void instruction();

private:
	bool on;
	int socket_c, socket_b;
	int port_c, port_b;
	string ip;
	unordered_map<string, user> user_list;

	void* beat(void* arguments);

};

#endif