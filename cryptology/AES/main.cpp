#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>

#include "AES.h"
using namespace std;

int main() {
	char t[200000], k[17];
	string p_, k_;
	cout << "input the plain text" << endl;
	cin >> p_;
	cout << "input the 128-bit-long cipher key" << endl;
	cin >> k_;
	if (k_.size() != 16) {
		printf("%s\n", "error: length of cipher key is not 128.");
		exit(0);
	} else {
		for (int i = 0; i < k_.size(); i++)
			k[i] = k_[i];
		memset(k + k_.size(), '\0', 1);
	}
	int to_add_length = 16 - p_.size() % 16;
	for (int i = 0; i < (p_.size() / 16 + 1) * 16; i++) 
		if (i < p_.size())
			t[i] = p_[i];
		else
			t[i] = char(to_add_length);
	memset(t + (p_.size() / 16 + 1) * 16, '\0', 1);

	aes_encrypt(t, strlen(t), k);
	aes_decrypt(t, strlen(t), k);
}