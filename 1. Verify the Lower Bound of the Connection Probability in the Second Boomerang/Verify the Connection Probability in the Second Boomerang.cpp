#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<immintrin.h>
#include<stdint.h>
#include<time.h>

__m128i k3 = _mm_set_epi32(0xE23A7BE9, 0xC345F27F, 0x835C326B, 0x215BE15E);
__m128i k4 = _mm_set_epi32(0x8142E55B, 0x42071724, 0xC15B254F, 0x63789EB2);
__m128i imck3 = _mm_set_epi32(0x8ADC879B, 0x37E910C5, 0x60FFC6DF, 0x450C1498);
unsigned f1[4] = { 0x00ffffff,0xff00ffff,0xffff00ff,0xffffff00 };//diagonal mask
unsigned f2[4] = { 0x00ffffff,0xffffff00,0xffff00ff,0xff00ffff };//inverse diagonal mask

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

int Judge(__m128i x, __m128i y)
{
	int i, j, con = 0, t;
	unsigned* temp;
	__m128i dif, mask, dif0;
	x = _mm_xor_si128(x, k4);
	x = _mm_aesimc_si128(x);
	x = _mm_aesdec_si128(x, imck3);
	y = _mm_xor_si128(y, k4);
	y = _mm_aesimc_si128(y);
	y = _mm_aesdec_si128(y, imck3);
	dif = _mm_xor_si128(x, y);
	for (i = 0; i < 4; i++)
	{
		t = 1;
		mask = _mm_set_epi32(f2[i], f2[(i + 1) & 3], f2[(i + 2) & 3], f2[(i + 3) & 3]);
		dif0 = _mm_and_si128(dif, mask);//calculate differences in the other three inverse diagonals
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
			con += 1;
		}
	}
	return con;
}

int Exchange(__m128i x, __m128i y)
{
	int i, con = 0, j, flag;
	__m128i px, py, ex, ey, cx, cy, mask1, mask2, maskf, diff;
	unsigned* temp;
	maskf = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

	for (i = 0; i < 4; i++)//exchanged any diagonal
	{
		mask1 = _mm_set_epi32(f1[i], f1[(i + 1) & 3], f1[(i + 2) & 3], f1[(i + 3) & 3]);
		mask2 = _mm_xor_si128(mask1, maskf);

		px = _mm_and_si128(x, mask2);
		py = _mm_and_si128(y, mask2);
		diff = _mm_xor_si128(px, py);
		temp = (unsigned*)&diff;

		flag = 0;
		for (j = 0; j < 4; j++)
		{
			if (*(temp + j) != 0)
			{
				flag = 1;
				break;
			}
		}

		if (flag == 0)
			continue;

		ex = _mm_and_si128(x, mask1);
		ey = _mm_and_si128(y, mask1);
		cx = _mm_xor_si128(ex, py);
		cy = _mm_xor_si128(ey, px);
		con += Judge(cx, cy);
	}
	return con;
}


int main()
{
	unsigned* j;
	unsigned  con = 0, k;
	unsigned long long i;
	time_t starttime, endtime;
	unsigned* random;
	__m128i x, y, dif, start;
	random = (unsigned*)malloc(sizeof(unsigned) * 4);
	j = (unsigned*)malloc(sizeof(unsigned));
	starttime = time(NULL);

	for (i = 0; i < 4398046511104; i++)
	{
		for (k = 0; k < 4; k++)
		{
			_rdrand32_step(random + k);
		}
		start = _mm_set_epi32(*random, *(random + 1), *(random + 2), *(random + 3));
		_rdrand32_step(j);

		if (!(*j & 0xff000000))
		{
			*j = *j ^ ((rand() & 0xff) << 24);
		}
		if (!(*j & 0x00ff0000))
		{
			*j = *j ^ ((rand() & 0xff) << 16);
		}
		if (!(*j & 0x0000ff00))
		{
			*j = *j ^ ((rand() & 0xff) << 8);
		}
		if (!(*j & 0x000000ff))
		{
			*j = *j ^ (rand() & 0xff);
		}

		dif = _mm_set_epi32( 0, 0, 0, *j);//choose the 0-th inverse diagonal
		y = _mm_xor_si128(start, dif);
		x = _mm_aesenc_si128(_mm_aesenc_si128(start, k3), k4);
		y = _mm_aesenc_si128(_mm_aesenc_si128(y, k3), k4);
		k = Exchange(x, y);
		if (k > 0)
		{
			con += k;
			printf("X1=");
			pp(start);
			printf("diff=");
			pp(dif);
			printf("\n");
		}

	}
	endtime = time(NULL);
	printf("time=%f\n", difftime(endtime, starttime));
	printf("total_count=%d\n", con);

}