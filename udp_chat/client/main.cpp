#include "client.h"
#include <cstdlib>

using namespace std;

Client* client;

int main(int argc, char** argv) {
	client = new Client(atoi(argv[1]));
	client.bind();
	client.run();
}