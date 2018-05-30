#include <iostream>
#include <cstdio>
using namespace std;

void BM_LSRF(int *s, int n) {
	int l[n], c[n][n], m, d = 0;
	for (int i = 0; i < n; i++) {
		l[i] = 0;
		for (int j = 0; j < n; j++)
			c[i][j] = 0;
	}
	c[0][0] = 1;
	for (int i = 0; i < n; i++) {
		d = 0;
		for (int j = 0; j <= l[i]; j++)
			d = d ^ (c[i][j] * s[i - j]);
		if (d == 0) {
			l[i + 1] = l[i];
			for (int h = 0; h < n; h++)
				c[i + 1][h] = c[i][h];
		}
		else {
			int k = i;
			while (k >= 0 && l[k] == 0)
				k--;
			if (k == -1) {
				for (int h = 0; h < n; h++)
					c[i + 1][h] = c[i][h];
				c[i + 1][i + 1] = 1;
				l[i + 1] = i + 1;
			} else {
				k = i - 1;
				while (l[k] == l[k + 1])
					k--;
				for (int h = 0; h < n; h++) 
					if (h - (i - k) >= 0)
						c[i + 1][h] = c[k][h - (i - k)] ^ c[i][h];
					else
						c[i + 1][h] = c[i][h];
				l[i + 1] = l[i] > i + 1 - l[i] ? l[i] : i + 1 - l[i];
			}
		}
		
	}

	cout << "f(x) = 1";
	for (int j = 1; j < n; j++) {
		if (c[n][j])
			cout << " + x^" << j;
	}
	cout << endl << "l = " << l[n] << endl;
}

int main(int argc, char **argv) {
	int s = 0;
	int b[14] = {0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1};
	BM_LSRF(b, 14);
}