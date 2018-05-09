#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <cassert>

#include "AES.h"
using namespace std;

inline static int char2int(char c) {
    int result = (int) c;
    return result & 0x000000ff;
}

void print_ASSCI(char *s, int length) {
    for (int i = 0; i < length; i++)
        printf("0x%x ", char2int(s[i]));
    printf("\n");
}

void test_performance() {
	char *test_string = new char[25000000];
	for (int i = 0; i < 25000000; i++)
		test_string[i] = 'a';
	char key[] = "abcdabcdabcdabcd";
	clock_t start = clock();
	aes_encrypt(test_string, 25000000, key);
    clock_t ends = clock();
    cout <<"Running Time : "<<(double)(ends - start)/ CLOCKS_PER_SEC << endl;
    delete test_string;
}

int main() {
	// test_performance();
	char t[20000], k[17];
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
    print_ASSCI(t, strlen(t));
	aes_decrypt(t, strlen(t), k);
}