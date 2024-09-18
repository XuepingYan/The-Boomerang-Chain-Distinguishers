#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<immintrin.h>
#include<stdint.h>
#include<time.h>
#include<emmintrin.h>

__m128i k3 = _mm_set_epi32( 0xE23A7BE9, 0xC345F27F, 0x835C326B, 0x215BE15E);
__m128i k4 = _mm_set_epi32( 0x8142E55B, 0x42071724, 0xC15B254F, 0x63789EB2);
__m128i imck3 = _mm_set_epi32( 0x8ADC879B, 0x37E910C5, 0x60FFC6DF, 0x450C1498);
unsigned f1[4] = { 0x00ffffff,0xff00ffff,0xffff00ff,0xffffff00 };//对角线掩码
unsigned f2[4] = { 0x00ffffff,0xffffff00,0xffff00ff,0xff00ffff };//反对角线掩码
__m128i start = _mm_set_epi32( 0x650070c9, 0xb6bd59ba, 0xb1cd80f7, 0x46182afd);
__m128i dif = _mm_set_epi32( 0x00000000, 0x00000000, 0x00000000, 0xbefcb725);

void pp2(__m128i* x)
{
	char a[16];
	memcpy(a, x, 16);
	unsigned i, j;
	for (i = 0; i < 16; i++)
	{
		if (i == 0 || i == 4 || i == 8 || i == 12)
			printf("0x");
		
			printf("%02x", a[i] & 0xff);
			if (i == 3 || i == 7 || i == 11)
				printf(", ");

	}
	printf("\n");
}

void pp(__m128i* x)
{
	char a[16];
	memcpy(a, x, 16);
	unsigned t;
	for (t = 0; t < 16; t++)
	{
		printf("%02x", a[t] & 0xff);
		if (t == 3 || t == 7 || t == 11)
			printf(", ");
	}
	printf("\n");
}

void Judge(__m128i x, __m128i y)
{
	int i, j, con = 0, t;
	unsigned* temp;
	__m128i RoundKey = _mm_set_epi32(0, 0, 0, 0);
	__m128i dif, mask, dif0;
	x = _mm_xor_si128(x, k4);
	x = _mm_aesimc_si128(x);
	x = _mm_aesdec_si128(x, imck3);
	y = _mm_xor_si128(y, k4);
	y = _mm_aesimc_si128(y);
	y = _mm_aesdec_si128(y, imck3);
	//x = _mm_aesdeclast_si128(x, RoundKey);
	//y = _mm_aesdeclast_si128(y, RoundKey);
	printf("x1'=");
	pp(&x);
	printf("x2'=");
	pp(&y);
	dif = _mm_xor_si128(x, y);
	printf("\n");
	//pp(&x);
	//pp(&y);
	pp(&dif);
	printf("\n");
	/*for (i = 0; i < 4; i++)
	{
		t = 1;
		mask = _mm_set_epi32(f2[i], f2[(i + 1) & 3], f2[(i + 2) & 3], f2[(i + 3) & 3]);
		dif0 = _mm_and_si128(dif, mask);//计算除某条反对角线外的其他差分值
		temp = (unsigned*)&dif0;
		for (j = 0; j < 4; j++)
		{
			if (*(temp + j) != 0)
			{
				t = 0;
				break;
			}
		}
		if (t)
		{
			printf("%d\n\n", i);
		}
	}*/
}

void Exchange(__m128i x, __m128i y)
{
	int i, con = 0;
	__m128i px, py, ex, ey, cx, cy, mask1, mask2, maskf;
	maskf = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	for (i = 0; i < 4; i++)//交换任意一个对角线
	{
		printf("now exchange the diagonal %d!\n", i);
		mask1 = _mm_set_epi32(f1[i], f1[(i + 1) & 3], f1[(i + 2) & 3], f1[(i + 3) & 3]);
		mask2 = _mm_xor_si128(mask1, maskf);
		//pp(&mask1);
		//pp(&mask2);
		ex = _mm_and_si128(x, mask1);
		px = _mm_and_si128(x, mask2);
		ey = _mm_and_si128(y, mask1);
		py = _mm_and_si128(y, mask2);
		cx = _mm_xor_si128(ex, py);
		cy = _mm_xor_si128(ey, px);
		printf("y3=");
		pp(&cx);
		printf("y4=");
		pp(&cy);
		Judge(cx, cy);
		
	}
}

int main()
{
	__m128i x, y, diff;
	int i, j;


	pp(&k3);
	pp(&k4);
	printf("x_1=");
	pp(&start);
	printf("x_2=");
	y = _mm_xor_si128(start, dif);
	pp(&y);
	printf("x_1+x_2=");
	pp(&dif);
	x = _mm_aesenc_si128(_mm_aesenc_si128(start, k3), k4);
	y = _mm_aesenc_si128(_mm_aesenc_si128(y, k3), k4);
	diff = _mm_xor_si128(y, x);
	printf("y_1=");
	pp(&x);
	printf("y_2=");
	pp(&y);
	printf("y_1+y_2=");
	pp(&diff);
	printf("\n");
	Exchange(x, y);
	printf("\n\n\n\n\n");


	__m128i k0 = _mm_set_epi32(0, 0, 0, 0);
	y = _mm_xor_si128(start, dif);
	x= _mm_aesenclast_si128(start, k0);
	y = _mm_aesenclast_si128(y, k0);
	diff=_mm_xor_si128(x,y);
	printf("x_1=");
	pp(&x);
	printf("x_2=");
	pp(&y);
	printf("x_1+x_2=");
	pp(&diff);
}