#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {;
	// char s[10];
	// cout<<to_string(124);
	string a = "chat abc 1 2 3 5235";
	string h = "abc";
	int b,c,d;
	stringstream stream;
	stream<<a;
	stream>>b>>c>>d;
	cout<< stream.str().substr(6 + h.length());

	// cout << b<<endl;
	// cout<<c<<endl;
	// cout<<d<<endl;
}