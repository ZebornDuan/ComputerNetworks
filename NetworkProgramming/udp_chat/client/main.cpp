#include "client.h"
#include <cstdlib>
#include <signal.h>

using namespace std;

Client* client;

void exit_signal(int signal_number) {
	client->quit();
	exit(0);
}

int main(int argc, char** argv) {
	client = new Client(atoi(argv[1]));
	signal(SIGINT, exit_signal);
	client->bind_address();
	client->run();
}