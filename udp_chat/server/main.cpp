#include "server.h"
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

Server* server;

int main() {
	server = new Server();
	server.bind();
	server.run();
}