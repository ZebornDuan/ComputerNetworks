#include "server.h"
#include <vector>

using namespace std;

Server::Server() {
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
		printf("fail to bind\n");
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
	vector<string> offline_list;
	for (unordered_map<string, user>::iterator i = user_list.begin(); i != user_list.end(); i++) {
		time_t current = time(0);
		if (current - i->second.time > 10) {
			offline_list.push_back(i->first);
			user_list.erase(i);			
		}	
	}

	string message = "offline";
	for (int i = 0; i < offline_list.size(); i++)
		message = message + " " + offline_list[i];

	inform(message);
}

void Server::inform(string message) {
	for (unordered_map<string, user>::iterator i = user_list.begin(); i != user_list.end(); i++) {
		struct sockaddr_in destination;
		bzero(destination, sizeof(destination));
		destination.sin_family = AF_INET;
		destination.sin_port = htons(i->second.port);
		destination.sin_addr.s_addr = inet_addr(i->second.ip);
		sendto(socket_s, message.c_str(), message.length(), 0, (struct sockaddr*)&destination, sizeof(struct sockaddr_in));
	}
}

void Server::run() {
	if (pthread_creat(&beat_p, NULL, test_beat, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}

	if (pthread_creat(&test_beat_p, NULL, test_online, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}

	char buffer[4096];
	while (1) {
		int value = recvfrom(socket_s, &buffer, 4096, 0, (struct sockaddr*)&client, &length);
		if (value < 0)
			continue;
		memset(buffer + value, '\0', 1);
		stringstream stream;
		stream << buffer;
		string message_type;
		stream >> message_type;

		if (message_type == "login") {
			string user_name, user_ip;
			int user_port;
			stream >> user_name >> user_ip >> user_port;
			struct sockaddr_in destination;
			bzero(destination, sizeof(destination));
			destination.sin_family = AF_INET;
			destination.sin_port = htons(user_port);
			destination.sin_addr.s_addr = inet_addr(user_ip);
			if (user_list.find(user_name) != user_list.end()) {
				string message = "ok";
				for (unordered_map<string, user>::iterator i = user_list.begin(); i != user_list.end(); i++)
					message = message + " " + i->first + " " + i->second.ip + " " + to_string(i->second.port);
				sendto(socket_s, message.c_str(), message.length(), 0, (struct sockaddr*)&destination, sizeof(struct sockaddr_in));
				user new_user;
				new_user.ip = user_ip;
				new_user.port = user_port;
				new_user.time = time(0);
				user_list[user_name] = new_user;
				message = "on " + user_name + " " + user_ip + " " + to_string(user_port);
				inform(message);
			} else
				sendto(socket_s, "no", 2, 0, (struct sockaddr*)&destination, sizeof(struct sockaddr_in));
			
		} else if (message_type == "bye") {
			string user_name;
			stream >> user_name;
			if (user_list.find(user_name) != user_list.end()) {
				user_list.erase(user_name);
				string message = "offline " + user_name;
				inform(message);
			}
		}
	}
}