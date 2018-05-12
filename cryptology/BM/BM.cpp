#include <iostream>
#include <cstdio>
using namespace std;

void BM(int s, int n) {
	int l[15], m, d, c[15];
	int n0;
	c[0] = 1;
	for (int i = 0; i < n; i++) {
		if (s >> i & 1) {
			n0 = i;
			l[i + 1] = i + 1;
			break;
		} else {
			l[i + 1] = 0;
			c[i + 1] = 1;
		}
	}

	c[n0 + 1] = 1 ^ (1 << (n0 + 1));
	m = n0 - 1;

	for (int i = n0 + 1; i < n; i++) {
		d = s >> i & 1;
		for (int j = 1; j < l[i]; j++)
			d = d ^ ((c[i] >> j & 1) & (s >> (i - j) & 1));
		if (d == 0) {
			c[i + 1] = c[i];
			l[i + 1] = l[i];
		} else {
			if (l[i] < i + 1 - l[i])
				l[i + 1] = i + 1 - l[i];
			else
				l[i + 1] = l[i];
			if (l[i] > l[i - 1])
				m = i - 1;
			c[i + 1] = c[i] ^ (c[m] << (i - m));
		}
	}

	cout << "f(x) = 1";
	for (int j = 1; j < n; j++) {
		if (c[n] >> j & 1)
			cout << " + x^" << j;
	}
}

int main(int argc, char **argv) {
	int s = 0;
	int b[14] = {0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1};
	int length = 14;
	for (int i = 0 ; i < length; i++) 
		s = s ^ (b[i] << i);
	BM(s, length);
}