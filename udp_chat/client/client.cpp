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

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(8000);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	bzero(&test_beat, sizeof(test_beat));
	test_beat.sin_family = AF_INET;
	test_beat.sin_port = htons(9000);
	test_beat.sin_addr.s_addr = inet_addr("127.0.0.1");
}

Client::~Client() {}

void Client::bind() {
	bzero(&client, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port_c);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");

	bzero(&beat, sizeof(beat));
	beat.sin_family = AF_INET;
	beat.sin_port = htons(port_b);
	beat.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(socket_c, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("fail to bind\n");
		exit(-1);
	}

	if (bind(socket_b, (struct sockaddr*)&beat, sizeof(beat)) < 0) {
		printf("fail to bind\n");
		exit(-1);
	}
}

void Client::print_command() {
	cout << "Supported commands are as follow:" << endl;
	cout << "chat <name> <message> (send the message to the user named 'name')" << endl;
	cout << "help                  (show the sopported commands)" << endl;
	cout << "user                  (show the online users)" << endl;
	cout << "quit                  (exit the system)" << endl;
	cout << endl;
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
			sendto(socket_c, message.c_str(), message.length(), 0, (struct sockaddr*)&test_beat, sizeof(struct sockaddr_in));
		} else
			break;
	}

	return 0;
}

void* Client::receive(void* arguments) {
	char buffer[4096];
	struct sockaddr_in source;
	socklen_t length = sizeof(server);
	while (1) {
		if (on) {
			int value = recvfrom(socket_c, &buffer, 4096, 0, (struct sockaddr*)&source, &length);
			if (value < 0) 
				continue;
			else {
				stringstream stream;
				stream << buffer;
				string what, who;
				stream >> what >> who;
				if (what == "chat") {
					string mail = stream.str().substr(6 + who.length());
					cout << "message from " << who << ":" << mail << endl;
				} else if (what == "on") {
					user new_user;
					stream >> new_user.ip >> new_user>>port;
					user_list[who] = new_user;
					cout << "User named " << who << " has logged in." << endl;
				} else if (what == "offline") {
					cout << "User named " << who << " has logged out." << endl;
					user_list.erase(who);
				}
			}
		}
	}
}

void Client::run() {
	cout << "Welcome, please input your nickname first!" << endl;
	print_command();
	while (1) {
		cin >> name;
		string message = "login " + name + " " + ip + " " + port;
		sendto(socket_c, message.c_str(), message.length(), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
		char buffer[4096];
		socklen_t length = sizeof(server);
		int value = recvfrom(socket_c, &buffer, 4096, 0, (struct sockaddr*)&server, &length);
		if (value < 0)
			cout << "Fail to connect to server, please input your nickname and try again!" << endl;
		else {
			memset(buffer + value, '\0', 1);
			stringstream stream;
			stream << buffer;
			string response;
			stream >> response;
			if (response == "ok") {
				string user_name, user_ip;
				int user_port;
				while (stream >> user_name >> user_ip >> user_port) {
					user new_user;
					new_user.ip = user_ip;
					new_user.port = user_port;
					user_list[user_name] = new_user;
				}
				cout << "Sign in successfully!" << endl;
				print_user();
				on = true;
				break;
			} else if (response == "no")
				cout << "The name has been used by others, choose another name and try again!" << endl;
		}
	}

	if (pthread_create(&beat_p, NULL, beat, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}

	if (pthread_create(&receive_p, NULL, receive, NULL) != 0) {
		printf("fail to create thread\n");
		exit(-1);
	}

	string command;
	while (cin >> command) {
		stringstream stream;
		stream << command;
		stream >> command;
		if (command == "chat") {
			string who;
			stream >> who;
			struct sockaddr_in destination;
			bzero(destination, sizeof(destination));
			destination.sin_family = AF_INET;
			destination.sin_port = htons(user_list[who].port);
			destination.sin_addr.s_addr = inet_addr(user_list[who].ip);
			string mail = stream.str().substr(5 + who.length());
			string message = "chat " + name + mail;
			sendto(socket_c, message.c_str(), message.length(), 0, (struct sockaddr*)&destination, sizeof(struct sockaddr_in));
			cout << "send to " << who << ":" << mail << endl;
		} else if (command == "help")
			print_command();
		else if (command == "user")
			print_user();
		else if (command == "quit") {
			cout << "" << endl;
			on = false;
			pthread_join(beat_p);
			pthread_join(receive_p);
			break;
		} else
			cout << "Error, undefined command!" << endl;
	}
}
