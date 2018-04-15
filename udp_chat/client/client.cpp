#include "client.h"
#include <iostream>
#include <string>

using namespace std;

Client::Client(int port) {
	on = false;
	ip = "127.0.0.1";
	port_c = port;
	port_b = port + 1;

	socket_c = socket(AF_INET, SOCK_DGRAM, 0);
	socket_b = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_c < 0 || socket_b < 0) {
		printf("fail to create sockets!\n");
		exit(-1);
	}
}

Client::~Client() {}

void Client::bind() {

}

void Client::print_command() {
	cout << "" << endl;
	
}

void Client::print_user() {
	cout << "Online users are as follow: <name> <ip> <port>" << endl;
	for (unordered_map<string, user>::iterator i = user_list.begin(); i != user_list.end(); i++)
		cout << i->first << " " << i->second.ip << " " << i->second.port << endl;
	cout << endl;
}

void* Client::beat(void* arguments) {
	while (1) {
		if (on) {
			Sleep(5);
			string message = name + " " + ip + " " + to_string(port_c);
			//sendto the server
		} else
			break;
	}

	return 0;
}

void* Client::receive(void* arguments) {
	while (1) {

	}
}

void Client::run() {
	//
	while (1) {
		cout << "Welcome, please input your nickname first!" << endl;
		cin >> name;
	}

	if (pthread_create(&beat_p, NULL, beat, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}

	if (pthread_create(&receive_p, NULL, receive, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}



}
