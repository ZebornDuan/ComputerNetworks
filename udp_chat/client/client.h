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
	void bind();
	void print_command();
	void print_user();

private:
	bool on;
	int socket_c, socket_b;
	int port_c, port_b;
	pthread_t beat_p, receive_p;
	string ip, name;
	struct sockaddr_in server, beat, client, test_beat;
	unordered_map<string, user> user_list;

	void* beat(void* arguments);
	void* receive(void* arguments);
};

#endif