

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "tables4.h"

#define VERBOSE

#define tabsize(t) (sizeof(t)/sizeof((t)[0]))

/***********************************/
/* T-tables based AES              */
/***********************************/

#define N 4
#define R 6


typedef uint16_t word;
typedef word state[4];
#define MASK 0xf
#define M0 0xf000
#define M1 0x0f00
#define M2 0x00f0
#define M3 0x000f

#define N0 0x0fff
#define N1 0xf0ff
#define N2 0xff0f
#define N3 0xfff0







//result of MC SB and its inverse
word Table[65536];
word Tableinv[65536];

//result of SB and its inverse
word Tab[65536];
word Tabinv[65536];



void PreTable()
{
    uint8_t k0, k1, k2, k3;
    for (k0 = 0; k0 < 16; k0++)
        for (k1 = 0; k1 < 16; k1++)
            for (k2 = 0; k2 < 16; k2++)
                for (k3 = 0; k3 < 16; k3++)
                {
                    Table[(k0 << 12) ^ (k1 << 8) ^ (k2 << 4) ^ k3] = Te0[k0] ^ Te1[k1] ^ Te2[k2] ^ Te3[k3];
                    Tableinv[(k0 << 12) ^ (k1 << 8) ^ (k2 << 4) ^ k3] = Td0[k0] ^ Td1[k1] ^ Td2[k2] ^ Td3[k3];
                    Tab[(k0 << 12) ^ (k1 << 8) ^ (k2 << 4) ^ k3] = (Te4[k0] & M0) ^ (Te4[k1] & M1) ^ (Te4[k2] & M2) ^ (Te4[k3] & M3);
                    Tabinv[(k0 << 12) ^ (k1 << 8) ^ (k2 << 4) ^ k3] = (Td4[k0] & M0) ^ (Td4[k1] & M1) ^ (Td4[k2] & M2) ^ (Td4[k3] & M3);
                }
}



void Round1(state x, state k, state y) {
    word index0, index1, index2, index3;
    //SR
    index0 = (x[0] & M0) ^ (x[1] & M1) ^ (x[2] & M2) ^ (x[3] & M3);
    index1 = (x[1] & M0) ^ (x[2] & M1) ^ (x[3] & M2) ^ (x[0] & M3);
    index2 = (x[2] & M0) ^ (x[3] & M1) ^ (x[0] & M2) ^ (x[1] & M3);
    index3 = (x[3] & M0) ^ (x[0] & M1) ^ (x[1] & M2) ^ (x[2] & M3);
    //MC SB
    y[0] = Table[index0];
    y[1] = Table[index1];
    y[2] = Table[index2];
    y[3] = Table[index3];

    //AK
    y[0] ^= k[0];
    y[1] ^= k[1];
    y[2] ^= k[2];
    y[3] ^= k[3];
}

void Roundinv1(state x, state k, state y) {
    word index0, index1, index2, index3;

    //SR^{-1}
    index0 = (x[0] & M0) ^ (x[3] & M1) ^ (x[2] & M2) ^ (x[1] & M3);
    index1 = (x[1] & M0) ^ (x[0] & M1) ^ (x[3] & M2) ^ (x[2] & M3);
    index2 = (x[2] & M0) ^ (x[1] & M1) ^ (x[0] & M2) ^ (x[3] & M3);
    index3 = (x[3] & M0) ^ (x[2] & M1) ^ (x[1] & M2) ^ (x[0] & M3);


    //MC^{-1}SB^{-1}
    y[0] = Tableinv[index0];
    y[1] = Tableinv[index1];
    y[2] = Tableinv[index2];
    y[3] = Tableinv[index3];

    //AK^{-1}
    y[0] ^= k[0];
    y[1] ^= k[1];
    y[2] ^= k[2];
    y[3] ^= k[3];
    //print_state(y);


}

void Roundfinal1(state x, state k, state y) {
    word index0, index1, index2, index3;
    //SR
    index0 = (x[0] & M0) ^ (x[1] & M1) ^ (x[2] & M2) ^ (x[3] & M3);
    index1 = (x[1] & M0) ^ (x[2] & M1) ^ (x[3] & M2) ^ (x[0] & M3);
    index2 = (x[2] & M0) ^ (x[3] & M1) ^ (x[0] & M2) ^ (x[1] & M3);
    index3 = (x[3] & M0) ^ (x[0] & M1) ^ (x[1] & M2) ^ (x[2] & M3);
    //SB
    y[0] = Tab[index0];
    y[1] = Tab[index1];
    y[2] = Tab[index2];
    y[3] = Tab[index3];
    //AK
    y[0] ^= k[0];
    y[1] ^= k[1];
    y[2] ^= k[2];
    y[3] ^= k[3];
}

void Roundfinalinv1(state x, state k, state y) {
    word index0, index1, index2, index3;
    //SR^{-1}
    index0 = (x[0] & M0) ^ (x[3] & M1) ^ (x[2] & M2) ^ (x[1] & M3);
    index1 = (x[1] & M0) ^ (x[0] & M1) ^ (x[3] & M2) ^ (x[2] & M3);
    index2 = (x[2] & M0) ^ (x[1] & M1) ^ (x[0] & M2) ^ (x[3] & M3);
    index3 = (x[3] & M0) ^ (x[2] & M1) ^ (x[1] & M2) ^ (x[0] & M3);

    //SB^{-1}
    y[0] = Tabinv[index0];
    y[1] = Tabinv[index1];
    y[2] = Tabinv[index2];
    y[3] = Tabinv[index3];

    //AK
    y[0] ^= k[0];
    y[1] ^= k[1];
    y[2] ^= k[2];
    y[3] ^= k[3];
}

void Encrypt(state x, state key[R + 1], state y) {
    uint8_t i;
    state z;
    memcpy(z, x, sizeof(state));

    z[0] ^= key[0][0];
    z[1] ^= key[0][1];
    z[2] ^= key[0][2];
    z[3] ^= key[0][3];



    for (i = 1; i < 6; i++)
    {
        Round1(z, key[i], y);
        memcpy(z, y, sizeof(state));

    }

    Roundfinal1(z, key[R], y);

}

void OneRoundEncrypt(state x1, state x2, state key[R + 1], state y) {
    state z1, z2, y1, y2;
    memcpy(z1, x1, sizeof(state));
    z1[0] ^= key[0][0];
    z1[1] ^= key[0][1];
    z1[2] ^= key[0][2];
    z1[3] ^= key[0][3];
    Round1(z1, key[1], y1);
    memcpy(z2, x2, sizeof(state));
    z2[0] ^= key[0][0];
    z2[1] ^= key[0][1];
    z2[2] ^= key[0][2];
    z2[3] ^= key[0][3];
    Round1(z2, key[1], y2);
    y[0] = y1[0] ^ y2[0];
    y[1] = y1[1] ^ y2[1];
    y[2] = y1[2] ^ y2[2];
    y[3] = y1[3] ^ y2[3];
}



void Decrypt(state x, state key[R + 1], state y) {
    uint8_t i;
    state z;

    memcpy(z, x, sizeof(state));
    //AK
    z[0] ^= key[R][0];
    z[1] ^= key[R][1];
    z[2] ^= key[R][2];
    z[3] ^= key[R][3];



    for (i = 5; i > 0; i--)
    {
        Roundinv1(z, key[i], y);
        memcpy(z, y, sizeof(state));
    }

    Roundfinalinv1(z, key[0], y);

}

void OneRoundDecrypt(state x1, state x2, state key[R + 1], state y) {
    state z1, z2, y1, y2;
    memcpy(z1, x1, sizeof(state));
    z1[0] ^= key[R][0];
    z1[1] ^= key[R][1];
    z1[2] ^= key[R][2];
    z1[3] ^= key[R][3];
    Roundinv1(z1, key[R-1], y1);

    memcpy(z2, x2, sizeof(state));
    z2[0] ^= key[R][0];
    z2[1] ^= key[R][1];
    z2[2] ^= key[R][2];
    z2[3] ^= key[R][3];
    Roundinv1(z2, key[R-1], y2);
    y[0] = y1[0] ^ y2[0];
    y[1] = y1[1] ^ y2[1];
    y[2] = y1[2] ^ y2[2];
    y[3] = y1[3] ^ y2[3];
}




void generationRoundKey(state masterkey, state key[R + 1]) {
    memcpy(key[0], masterkey, sizeof(state));

    uint64_t* random;
    random = (uint64_t*)malloc(sizeof(uint64_t));
    uint8_t i;

    for (i = 1; i <= R; i++)
    {
        _rdrand64_step(random);
        memcpy(key[i], random, sizeof(state));
    }
}

int weight(state x, state y)
{
    state z;
    for (uint8_t i = 0; i < 4; i++)
    {
        z[i] = x[i] ^ y[i];
    }
    return
        !!((z[0] & M0) ^ (z[1] & M1) ^ (z[2] & M2) ^ (z[3] & M3)) +
        !!((z[1] & M0) ^ (z[2] & M1) ^ (z[3] & M2) ^ (z[0] & M3)) +
        !!((z[2] & M0) ^ (z[3] & M1) ^ (z[0] & M2) ^ (z[1] & M3)) +
        !!((z[3] & M0) ^ (z[0] & M1) ^ (z[1] & M2) ^ (z[2] & M3));
}

int weight_inver(state x, state y)
{
    state z;
    for (uint8_t i = 0; i < 4; i++)
    {
        z[i] = x[i] ^ y[i];
    }
    return
        !!((z[0] & M0) ^ (z[3] & M1) ^ (z[2] & M2) ^ (z[1] & M3)) +
        !!((z[1] & M0) ^ (z[0] & M1) ^ (z[3] & M2) ^ (z[2] & M3)) +
        !!((z[2] & M0) ^ (z[1] & M1) ^ (z[0] & M2) ^ (z[3] & M3)) +
        !!((z[3] & M0) ^ (z[2] & M1) ^ (z[1] & M2) ^ (z[0] & M3));
}

int exchange(state x, state y, uint8_t i, state z, state w)
{
    if (i == 0) {
        z[0] = (x[0] & N0) ^ (y[0] & M0);
        z[1] = (x[1] & N3) ^ (y[1] & M3);
        z[2] = (x[2] & N2) ^ (y[2] & M2);
        z[3] = (x[3] & N1) ^ (y[3] & M1);

        w[0] = (y[0] & N0) ^ (x[0] & M0);
        w[1] = (y[1] & N3) ^ (x[1] & M3);
        w[2] = (y[2] & N2) ^ (x[2] & M2);
        w[3] = (y[3] & N1) ^ (x[3] & M1);
        return 0;
    }
    if (i == 1) {
        z[0] = (x[0] & N1) ^ (y[0] & M1);
        z[1] = (x[1] & N0) ^ (y[1] & M0);
        z[2] = (x[2] & N3) ^ (y[2] & M3);
        z[3] = (x[3] & N2) ^ (y[3] & M2);

        w[0] = (y[0] & N1) ^ (x[0] & M1);
        w[1] = (y[1] & N0) ^ (x[1] & M0);
        w[2] = (y[2] & N3) ^ (x[2] & M3);
        w[3] = (y[3] & N2) ^ (x[3] & M2);
        return 0;
    }
    if (i == 2) {
        z[0] = (x[0] & N2) ^ (y[0] & M2);
        z[1] = (x[1] & N1) ^ (y[1] & M1);
        z[2] = (x[2] & N0) ^ (y[2] & M0);
        z[3] = (x[3] & N3) ^ (y[3] & M3);

        w[0] = (y[0] & N2) ^ (x[0] & M2);
        w[1] = (y[1] & N1) ^ (x[1] & M1);
        w[2] = (y[2] & N0) ^ (x[2] & M0);
        w[3] = (y[3] & N3) ^ (x[3] & M3);
        return 0;
    }
    if (i == 3) {
        z[0] = (x[0] & N3) ^ (y[0] & M3);
        z[1] = (x[1] & N2) ^ (y[1] & M2);
        z[2] = (x[2] & N1) ^ (y[2] & M1);
        z[3] = (x[3] & N0) ^ (y[3] & M0);

        w[0] = (y[0] & N3) ^ (x[0] & M3);
        w[1] = (y[1] & N2) ^ (x[1] & M2);
        w[2] = (y[2] & N1) ^ (x[2] & M1);
        w[3] = (y[3] & N0) ^ (x[3] & M0);
        return 0;
    }
}

void SRSB(state x) {
    state y;
    y[0] = (Te4[(x[0] >> (3 * N)) & MASK] & M0) ^
        (Te4[(x[1] >> (2 * N)) & MASK] & M1) ^
        (Te4[(x[2] >> (1 * N)) & MASK] & M2) ^
        (Te4[(x[3] >> (0 * N)) & MASK] & M3);
    y[1] = (Te4[(x[1] >> (3 * N)) & MASK] & M0) ^
        (Te4[(x[2] >> (2 * N)) & MASK] & M1) ^
        (Te4[(x[3] >> (1 * N)) & MASK] & M2) ^
        (Te4[(x[0] >> (0 * N)) & MASK] & M3);
    y[2] = (Te4[(x[2] >> (3 * N)) & MASK] & M0) ^
        (Te4[(x[3] >> (2 * N)) & MASK] & M1) ^
        (Te4[(x[0] >> (1 * N)) & MASK] & M2) ^
        (Te4[(x[1] >> (0 * N)) & MASK] & M3);
    y[3] = (Te4[(x[3] >> (3 * N)) & MASK] & M0) ^
        (Te4[(x[0] >> (2 * N)) & MASK] & M1) ^
        (Te4[(x[1] >> (1 * N)) & MASK] & M2) ^
        (Te4[(x[2] >> (0 * N)) & MASK] & M3);

    memcpy(x, y, sizeof(state));
}


void RoundInv(state x, state y) {
    y[0] = Td0[(x[0] >> (3 * N)) & MASK] ^
        Td1[(x[3] >> (2 * N)) & MASK] ^
        Td2[(x[2] >> (1 * N)) & MASK] ^
        Td3[(x[1] >> (0 * N)) & MASK];
    y[1] = Td0[(x[1] >> (3 * N)) & MASK] ^
        Td1[(x[0] >> (2 * N)) & MASK] ^
        Td2[(x[3] >> (1 * N)) & MASK] ^
        Td3[(x[2] >> (0 * N)) & MASK];
    y[2] = Td0[(x[2] >> (3 * N)) & MASK] ^
        Td1[(x[1] >> (2 * N)) & MASK] ^
        Td2[(x[0] >> (1 * N)) & MASK] ^
        Td3[(x[3] >> (0 * N)) & MASK];
    y[3] = Td0[(x[3] >> (3 * N)) & MASK] ^
        Td1[(x[2] >> (2 * N)) & MASK] ^
        Td2[(x[1] >> (1 * N)) & MASK] ^
        Td3[(x[0] >> (0 * N)) & MASK];
}

void MCInv(state x) {
  state y;
  SRSB(x);
  RoundInv(x, y);  
  memcpy(x, y, sizeof(state));
}


void print_state(state x) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            printf("%01x", (x[j] >> (4 * (3 - i))) & 0xf);
        printf("\n");
    }

    printf("\n");
}

int equal(state x, state y)
{
    if ((x[0] == y[0]) && (x[1] == y[1]) && (x[2] == y[2]) && (x[3] == y[3]))
        return 1;
    else
        return 0;
}

void cyc_shift(unsigned long long* a, int r)   //a循环左移r个比特后的结果
{
    unsigned long long temp1, temp2;
    temp1 = a[0] << r;
    temp2 = a[1] << r;
    //printf("%16x\n", temp1);
    //printf("%16x\n", temp2);
    //printf("%16x\n", a[0] >> (64 - r));
    //printf("%16x\n", a[1] >> (64 - r));
    temp1 = temp1 ^ (a[1] >> (64 - r));
    temp2 = temp2 ^ (a[0] >> (64 - r));
    a[0] = temp1;
    a[1] = temp2;
}

void shift(unsigned long long* a, int r)   //a左移r个比特后的结果
{
    unsigned long long temp1, temp2;
    temp1 = a[0] << r;
    temp2 = a[1] << r;
    temp1 = temp1 ^ (a[1] >> (64 - r));
    a[0] = temp1;
    a[1] = temp2;
}

void m_seq(unsigned long long* a)
{
    unsigned long long temp1 = a[2], temp2 = a[3];
    int r1 = 5, r2 = 17;
    cyc_shift(a, r1);
    shift(&a[2], r2);
    a[2] = a[0] ^ temp1 ^ a[2];
    a[3] = a[1] ^ temp2 ^ a[3];
    a[0] = temp1;
    a[1] = temp2;
}

