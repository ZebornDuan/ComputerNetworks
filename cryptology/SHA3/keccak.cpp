#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
using namespace std;

const int bitrate_bits = 1088;

const unsigned long long RC[24] = {
	0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
    0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
    0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
    0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
    0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

const unsigned long long r[5][5] = {
	{ 0, 36,  3, 41, 18},
    { 1, 44, 10, 45,  2},
    {62,  6, 43, 15, 61},
    {28, 55, 25, 21, 56},
    {27, 20, 39,  8, 14}
};
                                   
inline unsigned char bit2char(const string& binary_string) {
	int r = 0;
	for (int i = 0; i < binary_string.size(); i++) {
		r <<= 1;
		r += binary_string[i] - '0';
	}
	return (unsigned char)r;
}

string padding(const string& message) {
	string padded = message;
	int remaining = bitrate_bits - ((message.size() * 8 ) % bitrate_bits);
	if (remaining) {
		if (remaining == 8)
			padded += bit2char("10000110");
		else {
			padded += bit2char("00000110");
			padded.append((remaining - 16 ) / 8, '\0');
			padded += bit2char("10000000");
		}
	}
	return padded;
}

inline string ull2bit(const unsigned long long& n){
	string r;
	unsigned long long n_ = n;
	int ull_size = 64;
	while (ull_size--) {
		r += (char)((n_ & 1) + '0');
		n_ >>= 1;
	}
	reverse(r.begin(), r.end());
	return r;
}

inline unsigned long long string2ull(const string& s) {
	vector<bool> s_binary;
	for ( int i = 0; i < s.size(); i++) {
		string r;
		unsigned int c_ = s[i];
		int char_length = 8;
		while (char_length--) {
			r += (char)((c_ & 1) + '0');
			c_ >>= 1;
		}
		reverse(r.begin(), r.end());
		for (int i = 0; i < (int)r.size(); i++)
			s_binary.push_back( r[i] == '1' ? true : false );
	}
	unsigned long long r = 0;
	for (int i = 0; i < s_binary.size(); i++) {
		r <<= 1;
		r += s_binary[i];
	}
	return r;
}

inline string reverse_bytes(const string& ull) {
	string ull_ = ull;
	for (int i = 0; i < 4; i++)
		swap(ull_[i], ull_[7 - i]);
	return ull_;
}

inline unsigned long long reverse_bytes(const unsigned long long& ull) {
	string ull_binary = ull2bit(ull);
	string ull_chars;
	for (int i = 0; i < 64; i += 8)
		ull_chars += bit2char(ull_binary.substr(i, 8));
	ull_chars = reverse_bytes(ull_chars);
	return string2ull(ull_chars);
}

inline unsigned long long rotate_left(const unsigned long long &x,int n) {
    return ((x) << (n) | ((x) >> (64 - (n))));
}

void Round(vector<vector<unsigned long long> >& S, const int& RC_index) {
	//theta
	vector<unsigned long long> C(5);
	for (int i = 0; i < 5; i++)
		C[i] = S[i][0] ^ S[i][1] ^ S[i][2] ^ S[i][3] ^ S[i][4];
	vector<unsigned long long> D(5);
	for (int i = 0; i < 5; i++)
		D[i] = C[( i - 1 + 5 ) % 5] ^ rotate_left(C[( i + 1 ) % 5], 1);
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			S[i][j] ^= D[i];

	//rho and pi
	vector<vector<unsigned long long> > B(5, vector<unsigned long long>(5, 0));
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			B[j][( 2 * i + 3 * j ) % 5] = rotate_left(S[i][j], r[i][j]);

	//chi
	for (int i = 0;i < 5; i++)
		for (int j = 0; j < 5; j++)
			S[i][j] = B[i][j] ^ ((~B[( i + 1 ) % 5][j]) & B[( i + 2 ) % 5][j]);

	//iota
	S[0][0] ^= RC[RC_index];
}

string Keccak_256(string message) {
	message = padding(message);
	unsigned long long block_number = (message.size() * 8) / bitrate_bits;
	vector<vector<unsigned long long> > State(5, vector<unsigned long long>(5, 0));
	for ( int i = 0; i < block_number; i++) {
		string block = message.substr(i * 136, 136 );
		for ( int y = 0; y < 5; y++)
			for ( int x = 0; x < 5; x++)
				if (x + 5 * y < 17)
					State[x][y] ^= string2ull(reverse_bytes(block.substr((x + 5 * y ) * 8, 8)));
		for (int i = 0; i < 24; i++)
			Round(State, i);
	}
	string digest;
	for (int y = 0, k = 0; y < 5; y++)
		for (int x = 0; x < 5; x++)
			if (x + 5 * y < 17 && k < 4) {
				digest += ull2bit(reverse_bytes(State[x][y]));
				k++;
			}
	return digest;
}

void test_performance() {
	char *test_string = new char[2500000];
	for (int i = 0; i < 2500000; i++)
		test_string[i] = 'a';
	string t = test_string;
	clock_t start = clock();
	Keccak_256(t);
    clock_t ends = clock();
    cout <<"Running Time : "<<(double)(ends - start)/ CLOCKS_PER_SEC << endl;
    delete test_string;
}

int main() {
	// test_performance();
	string s = "hello world";
	string digest = Keccak_256(s);
	cout << digest;    
    return 0;
}
