#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

static const int S_Box[16][16] = {
/*      0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */ 
/*0*/  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 
/*1*/  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 
/*2*/  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 
/*3*/  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 
/*4*/  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 
/*5*/  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 
/*6*/  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 
/*7*/  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 
/*8*/  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 
/*9*/  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 
/*a*/  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 
/*b*/  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 
/*c*/  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 
/*d*/  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 
/*e*/  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 
/*f*/  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
};

static const int S_Box_i[16][16] = {
/*	    0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */ 
/*0*/  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 
/*1*/  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 
/*2*/  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 
/*3*/  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 
/*4*/  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 
/*5*/  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 
/*6*/  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 
/*7*/  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 
/*8*/  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 
/*9*/  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 
/*a*/  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 
/*b*/  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 
/*c*/  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 
/*d*/  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 
/*e*/  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 
/*f*/  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d 
};

inline static int get_left_4bits(int n) {
    int left = n & 0x000000f0;
    return left >> 4;
}

inline static int get_right_4bits(int n) {
    return n & 0x0000000f;
}

static int query_SBox(int index) {
    int r = get_left_4bits(index);
    int c = get_right_4bits(index);
    return S_Box[r][c];
}

inline static int char2int(char c) {
    int result = (int) c;
    return result & 0x000000ff;
}

static void to_array(char *s, int array[4][4]) {
    int k = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            array[j][i] = char2int(s[k]);
            k++;
        }
}

static void print_array(int a[4][4]) {
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++)
            printf("a[%d][%d] = 0x%x ", i, j, a[i][j]);
        printf("\n");
    }
    printf("\n");
}

static void print_ASSCI(char *s, int length) {
    for (int i = 0; i < length; i++)
        printf("0x%x ", char2int(s[i]));
    printf("\n");
}

static int string2word(char *s) {
    int w1 = char2int(s[0]);
    w1 = w1 << 24;
    int w2 = char2int(s[1]);
    w2 = w2 << 16;
    int w3 = char2int(s[2]);
    w3 = w3 << 8;
    int w4 = char2int(s[3]);
    return w1 | w2 | w3 | w4;
}


static void int2array(int n, int array[4]) {
    int b1 = n >> 24;
    array[0] = b1 & 0x000000ff;
    int b2 = n >> 16;
    array[1] = b2 & 0x000000ff;
    int b3 = n >> 8;
    array[2] = b3 & 0x000000ff;
    array[3] = n & 0x000000ff;
}


static void left_loop(int array[4], int step) {
    int t[4];
    for (int i = 0; i < 4; i++)
        t[i] = array[i];

    int index = step % 4 == 0 ? 0 : step % 4;
    for (int i = 0; i < 4; i++){
        array[i] = t[index];
        index++;
        index = index % 4;
    }
}

static int array2int(int array[4]) {
    int b1 = array[0] << 24;
    int b2 = array[1] << 16;
    int b3 = array[2] << 8;
    int b4 = array[3];
    return b1 | b2 | b3 | b4;
}

static const int Rcon[10] = { 
	0x01000000, 0x02000000,
    0x04000000, 0x08000000,
    0x10000000, 0x20000000,
    0x40000000, 0x80000000,
    0x1b000000, 0x36000000 
};

static int T(int n, int round) {
    int nArray[4];
    int2array(n, nArray);
    left_loop(nArray, 1);

    for (int i = 0; i < 4; i++)
        nArray[i] = query_SBox(nArray[i]);

    int result = array2int(nArray);
    return result ^ Rcon[round];
}

static int w[44];

static void extend_key(char *key) {
    for (int i = 0; i < 4; i++)
        w[i] = string2word(key + i * 4);

    for (int i = 4, j = 0; i < 44; i++) {
        if (i % 4 == 0) {
            w[i] = w[i - 4] ^ T(w[i - 1], j);
            j++;
        } else
            w[i] = w[i - 4] ^ w[i - 1];
    }
}


static void add_round_key(int array[4][4], int round) {
    int warray[4];
    for (int i = 0; i < 4; i++) {
        int2array(w[round * 4 + i], warray);

        for (int j = 0; j < 4; j++) {
            array[j][i] = array[j][i] ^ warray[j];
        }
    }
}


static void substitute_bytes(int array[4][4]){
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            array[i][j] = query_SBox(array[i][j]);
}

static void shift_rows(int array[4][4]) {
    int r2[4], r3[4], r4[4];
    for (int i = 0; i < 4; i++) {
        r2[i] = array[1][i];
        r3[i] = array[2][i];
        r4[i] = array[3][i];
    }

    left_loop(r2, 1);
    left_loop(r3, 2);
    left_loop(r4, 3);

    for (int i = 0; i < 4; i++) {
        array[1][i] = r2[i];
        array[2][i] = r3[i];
        array[3][i] = r4[i];
    }
}


static const int column_mix_matrix[4][4] = { 
	2, 3, 1, 1,
    1, 2, 3, 1,
    1, 1, 2, 3,
    3, 1, 1, 2 
};

static int GF_x2(int s) {
    int result = s << 1;
    int a = result & 0x00000100;

    if (a != 0) {
        result = result & 0x000000ff;
        result = result ^ 0x1b;
    }

    return result;
}

static int GF_x3(int s) {
    return GF_x2(s) ^ s;
}

static int GF_x4(int s) {
    return GF_x2(GF_x2(s));
}

static int GF_x8(int s) {
    return GF_x2(GF_x4(s));
}

static int GF_x9(int s) {
    return GF_x8(s) ^ s;
}

static int GF_x11(int s) {
    return GF_x9(s) ^ GF_x2(s);
}

static int GF_x12(int s) {
    return GF_x8(s) ^ GF_x4(s);
}

static int GF_x13(int s) {
    return GF_x12(s) ^ s;
}

static int GF_x14(int s) {
    return GF_x12(s) ^ GF_x2(s);
}

static int GF_x(int n, int s) {
    int result;

    if (n == 1)
        result = s;
    else if (n == 2)
        result = GF_x2(s);
    else if (n == 3)
        result = GF_x3(s);
    else if (n == 0x9)
        result = GF_x9(s);
    else if (n == 0xb)          //11
        result = GF_x11(s);
    else if (n == 0xd)          //13
        result = GF_x13(s);
    else if (n == 0xe)          //14
        result = GF_x14(s);

    return result;
}

static void mix_columns(int array[4][4]) {
    int t[4][4];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            t[i][j] = array[i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            array[i][j] = GF_x(column_mix_matrix[i][0],t[0][j]) ^ GF_x(column_mix_matrix[i][1],t[1][j]) 
                ^ GF_x(column_mix_matrix[i][2],t[2][j]) ^ GF_x(column_mix_matrix[i][3], t[3][j]);
        }
}

static void array2string(int array[4][4], char *s) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            *s++ = (char)array[j][i]; 
}

void aes_encrypt(char *p, int length, char *key) {
    extend_key(key);
    int pArray[4][4];

    for (int k = 0; k < length; k += 16) { 
        to_array(p + k, pArray);
        add_round_key(pArray, 0);

        for (int i = 1; i < 10; i++) {
            substitute_bytes(pArray);
            shift_rows(pArray);
            mix_columns(pArray);
            add_round_key(pArray, i);

        }

        substitute_bytes(pArray);
        shift_rows(pArray);
        add_round_key(pArray, 10);
        array2string(pArray, p + k);
    }
    print_ASSCI(p, length);
}


static int query_SBox_i(int index) {
    int r = get_left_4bits(index);
    int c = get_right_4bits(index);
    return S_Box_i[r][c];
}

static void substitute_bytes_i(int array[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            array[i][j] = query_SBox_i(array[i][j]);
}

static void right_loop(int array[4], int step) {
    int t[4];
    for (int i = 0; i < 4; i++)
        t[i] = array[i];

    int index = step % 4 == 0 ? 0 : step % 4;
    index = 3 - index;
    for (int i = 3; i >= 0; i--) {
        array[i] = t[index];
        index--;
        index = index == -1 ? 3 : index;
    }
}

static void shift_rows_i(int array[4][4]) {
    int r2[4], r3[4], r4[4];
    for (int i = 0; i < 4; i++) {
        r2[i] = array[1][i];
        r3[i] = array[2][i];
        r4[i] = array[3][i];
    }

    right_loop(r2, 1);
    right_loop(r3, 2);
    right_loop(r4, 3);

    for (int i = 0; i < 4; i++) {
        array[1][i] = r2[i];
        array[2][i] = r3[i];
        array[3][i] = r4[i];
    }
}

static const int inversed_column_mix_matrix[4][4] = { 
    0xe, 0xb, 0xd, 0x9,
    0x9, 0xe, 0xb, 0xd,
    0xd, 0x9, 0xe, 0xb,
    0xb, 0xd, 0x9, 0xe 
};

static void columns_mix_i(int array[4][4]) {
    int t[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            t[i][j] = array[i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++){
            array[i][j] = GF_x(inversed_column_mix_matrix[i][0],t[0][j]) ^ GF_x(inversed_column_mix_matrix[i][1],t[1][j]) 
                ^ GF_x(inversed_column_mix_matrix[i][2],t[2][j]) ^ GF_x(inversed_column_mix_matrix[i][3], t[3][j]);
        }
}

static void add_round_warray(int aArray[4][4],int bArray[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            aArray[i][j] = aArray[i][j] ^ bArray[i][j];
}

static void get_array(int i, int array[4][4]) {
    int index = i * 4;
    int c1[4], c2[4], c3[4], c4[4];
    int2array(w[index], c1);
    int2array(w[index + 1], c2);
    int2array(w[index + 2], c3);
    int2array(w[index + 3], c4);

    for (int i = 0; i < 4; i++) {
        array[i][0] = c1[i];
        array[i][1] = c2[i];
        array[i][2] = c3[i];
        array[i][3] = c4[i];
    }
}

void aes_decrypt(char *c, int length, char *key) {
    extend_key(key);
    int cArray[4][4];
    for (int k = 0; k < length; k += 16) {
        to_array(c + k, cArray);
        add_round_key(cArray, 10);
        int wArray[4][4];
        for (int i = 9; i >= 1; i--) {
            substitute_bytes_i(cArray);
            shift_rows_i(cArray);
            columns_mix_i(cArray);
            get_array(i, wArray);
            columns_mix_i(wArray);
            add_round_warray(cArray, wArray);
        }
        substitute_bytes_i(cArray);
        shift_rows_i(cArray);
        add_round_key(cArray, 0);
        array2string(cArray, c + k);
    }
    int added_length = int(c[length - 1]);
    memset(c + length - added_length, '\0', 1);
    printf("%s\n", c);
}