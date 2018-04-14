#include "server.h"

using namespace std;

Server::Server() {
	count = 0;
	socket_s = socket(AF_INET, SOCK_DGRAM, 0);
	socket_b = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_s < 0 || socket_b < 0) {
		printf("fail to create sockets!\n");
		exit(-1);
	}
}

Server::~Server() {}

void Server::bind() {
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(8000);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	bzero(&beat, sizeof(beat));
	beat.sin_family = AF_INET;
	beat.sin_port = htons(9000);
	beat.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(socket_s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("fail to bind\n");
		exit(-1);
	}

	if (bind(socket_b, (struct sockaddr*)&beat, sizeof(beat)) < 0) {
		printf("fail to bing\n");
		exit(-1);
	}
}

void* Server::test_beat(void* arguments) {
	socklen_t length = sizeof(client);
	int port;
	string ip, name;
	char buffer[4096];
	while (1) {
		int value = recvfrom(socket_b, &buffer, 4096, 0, (struct sockaddr*)&client, &length);
		if (value < 0)
			continue;
		memset(buffer + value, '\0', 1);
		stringstream stream;
		stream << buffer;
		stream >> name >> ip >> port;

		user_list[name].ip = ip;
		user_list[name].port = port;
		user_list[name].time = time(0);
	}
}

void* Server::test_online(void* arguments) {
	Sleep(15);
	for (unordered_map<string, user>::iterator i = user_list.begin(); i != user_list.end(); i++) {
		time_t current = time(0);
		if (current - i->second.time > 10) {
			//offline
		} 
	}
}

void Server::run() {

}