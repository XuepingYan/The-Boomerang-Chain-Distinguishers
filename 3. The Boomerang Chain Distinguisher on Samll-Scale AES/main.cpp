// Boomerang chain attack on 6-round small-scale AES




#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>
#include "PreComputation.c"

#define test_number 20


void main() {

    uint64_t t, w;
    uint64_t i, j, k1, k2, k3;
    uint64_t count[test_number] = { 0 };
    uint64_t counter = 0;
    unsigned long long  zero_diff = 0;
    state plain, newplain1, newplain2, cipher, newcipher1, newcipher2, x, y;
    state plain1, plain2, cipher1, cipher2;
    uint64_t* random;
    random = (uint64_t*)malloc(sizeof(uint64_t));
    state* L;
    L = (state*)malloc(64 * 2 * sizeof(state));
    uint16_t index1, index2, index3, index4;
    uint16_t L_index;
    uint64_t* ran;
    ran = (uint64_t*)malloc(4 * sizeof(uint64_t));
    state masterkey;
    uint16_t flag1[65536], flag2[65536],flag3[65536], flag4[65536];  
    state* friendpair;  
    state  key[R + 1] = { 0 }, keyinv[R + 1];
    

    state* list1, * list2, * list3, * list4;
    list1 = (state*)malloc(65536 * 16 * sizeof(state));
    list2 = (state*)malloc(65536 * 16 * sizeof(state));
    list3 = (state*)malloc(65536 * 16 * sizeof(state));
    list4 = (state*)malloc(65536 * 16 * sizeof(state));


    PreTable();
    time_t starttime, endtime;
    starttime = time(NULL);
    for (t = 0; t < test_number; t++)    //t:the number of experiments
    {
        count[t] = 0;
        //random round keys are chosen in each experiment
        _rdrand64_step(random);
        memcpy(masterkey, random, sizeof(state));
        generationRoundKey(masterkey, key);
        for (i = 0; i < R + 1; i++)
            memcpy(keyinv[i], key[i], sizeof(state));
        for (i = 1; i < R; i++)
            MCInv(keyinv[i]);

        for (j = 0; j < 4; j++)
        {
            _rdrand64_step(ran + j);
        }
        m_seq(ran);


        for (w = 0; w < 2; w++)          //w:the process is repeated twice
        {
            L_index = 0;
            memset(L, 0, 64 * 2 * sizeof(state));
            for (i = 0; i < 29125; i++)         //in the first boomerang, 29125 plaintext atructures are chosen
            {   
                memset(list1, 0, 65536 * 16 * sizeof(state));
                memset(list2, 0, 65536 * 16 * sizeof(state));
                memset(list3, 0, 65536 * 16 * sizeof(state));
                memset(list4, 0, 65536 * 16 * sizeof(state));

                for (j = 0; j < 65536; j++)
                {
                    flag1[j] = 0;
                    flag2[j] = 0;
                    flag3[j] = 0;
                    flag4[j] = 0;
                }

                m_seq(ran);
                memcpy(plain, &ran[2], sizeof(state));
                
                for (j = 0; j < 65536; j++)   //in each structure the 0-th diagonal is active
                {
                    x[0] = plain[0] ^ (j & M0);
                    x[1] = plain[1] ^ (j & M1);
                    x[2] = plain[2] ^ (j & M2);
                    x[3] = plain[3] ^ (j & M3);
                    Encrypt(x, key, cipher);

                    index1 = (cipher[0] & M0) ^ (cipher[3] & M1) ^ (cipher[2] & M2) ^ (cipher[1] & M3);
                    index2 = (cipher[1] & M0) ^ (cipher[0] & M1) ^ (cipher[3] & M2) ^ (cipher[2] & M3);
                    index3 = (cipher[2] & M0) ^ (cipher[1] & M1) ^ (cipher[0] & M2) ^ (cipher[3] & M3);
                    index4 = (cipher[3] & M0) ^ (cipher[2] & M1) ^ (cipher[1] & M2) ^ (cipher[0] & M3);

                    memcpy(list1[index1 * 16 + flag1[index1]], cipher, sizeof(state));
                    flag1[index1]++;

                    memcpy(list2[index2 * 16 + flag2[index2]], cipher, sizeof(state));
                    flag2[index2]++;

                    memcpy(list3[index3 * 16 + flag3[index3]], cipher, sizeof(state));
                    flag3[index3]++;

                    memcpy(list4[index4 * 16 + flag4[index4]], cipher, sizeof(state));
                    flag4[index4]++;
                }

                for (j = 0; j < 65536; j++)
                {
                    if (flag1[j] > 1)
                    {
                        for (k1 = 0; k1 < flag1[j]; k1++)
                        {
                            memcpy(x, list1[j * 16 + k1], sizeof(state));
                            for (k2 = k1 + 1; k2 < flag1[j]; k2++)
                            {                               
                                memcpy(y, list1[j * 16 + k2], sizeof(state));
                                if (weight_inver(x, y) == 3)
                                {
                                    zero_diff++;
                                    for (k3 = 1; k3 < 4; k3++)
                                    {
                                        exchange(x, y, k3, newcipher1, newcipher2);
                                        if (equal(newcipher1, x) || equal(newcipher2, x))
                                            continue;
                                        else
                                        {
                                            Decrypt(newcipher1, keyinv, newplain1);
                                            Decrypt(newcipher2, keyinv, newplain2);
                                            if (weight(newplain1, newplain2) ==2) {
                                                Decrypt(x, keyinv, L[L_index * 2]);
                                                Decrypt(y, keyinv, L[L_index * 2 + 1]);
                                                L_index++;
                                            }
                                        }
                                    }
                                }

                            }
                        }
                    }

                    if (flag2[j] > 1)
                    {
                        for (k1 = 0; k1 < flag2[j]; k1++)
                        {
                            memcpy(x, list2[j * 16 + k1], sizeof(state));
                            for (k2 = k1 + 1; k2 < flag2[j]; k2++)
                            {                                
                                memcpy(y, list2[j * 16 + k2], sizeof(state));
                                if (weight_inver(x, y) == 3)
                                {
                                    zero_diff++;
                                    for (k3 = 0; k3 < 4; k3++)
                                    {
                                        if (k3 != 1)
                                        {
                                            exchange(x, y, k3, newcipher1, newcipher2);
                                            if (equal(newcipher1, x) || equal(newcipher2, x))
                                                continue;
                                            else
                                            {
                                                Decrypt(newcipher1, keyinv, newplain1);
                                                Decrypt(newcipher2, keyinv, newplain2);
                                                if (weight(newplain1, newplain2) == 2) {
                                                    Decrypt(x, keyinv, L[L_index * 2]);
                                                    Decrypt(y, keyinv, L[L_index * 2 + 1]);
                                                    L_index++;
                                                }
                                            }
                                        }
                                    }
                                }

                            }
                        }
                    }

                    if (flag3[j] > 1)
                    {
                        for (k1 = 0; k1 < flag3[j]; k1++)
                        {
                            memcpy(x, list3[j * 16 + k1], sizeof(state));
                            for (k2 = k1 + 1; k2 < flag3[j]; k2++)
                            {                               
                                memcpy(y, list3[j * 16 + k2], sizeof(state));
                                if (weight_inver(x, y) == 3)
                                {
                                    zero_diff++;
                                    for (k3 = 0; k3 < 4; k3++)
                                    {
                                        if (k3 != 2)
                                        {
                                            exchange(x, y, k3, newcipher1, newcipher2);
                                            if (equal(newcipher1, x) || equal(newcipher2, x))
                                                continue;
                                            else
                                            {
                                                Decrypt(newcipher1, keyinv, newplain1);
                                                Decrypt(newcipher2, keyinv, newplain2);
                                                if (weight(newplain1, newplain2) == 2) {
                                                    Decrypt(x, keyinv, L[L_index * 2]);
                                                    Decrypt(y, keyinv, L[L_index * 2 + 1]);
                                                    L_index++;
                                                }
                                            }
                                        }
                                    }
                                }

                            }
                        }
                    }
                    if (flag4[j] > 1)
                    {
                        for (k1 = 0; k1 < flag4[j]; k1++)
                        {
                            memcpy(x, list4[j * 16 + k1], sizeof(state));
                            for (k2 = k1 + 1; k2 < flag4[j]; k2++)
                            {                                
                                memcpy(y, list4[j * 16 + k2], sizeof(state));
                                if (weight_inver(x, y) == 3)
                                {
                                    zero_diff++;
                                    for (k3 = 0; k3 < 3; k3++)
                                    {
                                        exchange(x, y, k3, newcipher1, newcipher2);
                                        if (equal(newcipher1, x) || equal(newcipher2, x))
                                            continue;
                                        else
                                        {
                                            Decrypt(newcipher1, keyinv, newplain1);
                                            Decrypt(newcipher2, keyinv, newplain2);
                                            if (weight(newplain1, newplain2) == 2) {
                                                Decrypt(x, keyinv, L[L_index * 2]);
                                                Decrypt(y, keyinv, L[L_index * 2 + 1]);
                                                L_index++;
                                            }
                                        }
                                    }
                                }

                            }
                        }

                    }

                }

            }

            uint8_t* flag_L, * flag_L1, * flag_L2;
            flag_L = (uint8_t*)malloc(L_index * sizeof(uint8_t));
            flag_L1 = (uint8_t*)malloc(L_index * sizeof(uint8_t));
            flag_L2 = (uint8_t*)malloc(L_index * sizeof(uint8_t));
            friendpair = (state*)malloc(L_index * 128 * sizeof(state));


            for (i = 0; i < L_index; i++)
            {
                flag_L[i] = 1;
                for (j = i+1; j < L_index; j++)
                {
                    if (equal(L[2 * i], L[2 * j]) && equal(L[2 * i + 1], L[2 * j + 1]))
                    {
                        flag_L[i] = 0;
                        break;
                    }
                    if (equal(L[2 * i], L[2 * j+1]) && equal(L[2 * i + 1], L[2 * j]))
                    {
                        flag_L[i] = 0;
                        break;
                    }
                }
            }

            //printf("L_index=%d\n", L_index);
            //printf("zero-diff=%u,\n", zero_diff);
            //for (i = 0; i < L_index; i++)
            //{
              //  printf("flag[%d]=%u,\n", i, flag_L[i]);
            //}


           
            
            memset(friendpair, 0, L_index * 128 * sizeof(state));


            for (i = 0; i < L_index; i++)    //the first middle boomerang£¬construct 89786798 friend pairs for each plaintext pair
            {
                flag_L1[i] = 0;
                if (flag_L[i] == 1)
                {
                    memcpy(x, L[2 * i], sizeof(state));
                    memcpy(y, L[2 * i + 1], sizeof(state));

                    for (j = 0; j < 89786798; j++)
                    {
                        m_seq(ran);
                        memcpy(plain1, &ran[2], sizeof(state));
                        memcpy(plain2, plain1, sizeof(state));
                        plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                        plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                        plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                        plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                        plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                        plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                        plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                        plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);

                        while (equal(plain1, x) && (equal(plain2, y)))
                        {
                            m_seq(ran);
                            memcpy(plain1, &ran[2], sizeof(state));
                            memcpy(plain2, plain1, sizeof(state));
                            plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                            plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                            plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                            plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                            plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                            plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                            plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                            plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);
                        }

                        Encrypt(plain1, key, cipher1);
                        Encrypt(plain2, key, cipher2);
                        if (weight_inver(cipher1, cipher2) == 4)
                        {
                            for (k1 = 0; k1 < 4; k1++)
                            {
                                exchange(cipher1, cipher2, k1, newcipher1, newcipher2);
                                if (equal(newcipher1, cipher1) || equal(newcipher1, cipher2))
                                    continue;
                                else
                                {
                                    Decrypt(newcipher1, keyinv, newplain1);
                                    Decrypt(newcipher2, keyinv, newplain2);
                                    if (weight(newplain1, newplain2) == 2)
                                    {
                                        memcpy(friendpair[i * 128 + flag_L1[i] * 2], newplain1, sizeof(state));
                                        memcpy(friendpair[i * 128 + flag_L1[i] * 2 + 1], newplain2, sizeof(state));
                                        flag_L1[i]++;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (i = 0; i < L_index; i++)
            {
                if (flag_L1[i] > 1)
                {
                    for (j = 0; j < flag_L1[i]; j++)
                        for (k1 = j + 1; k1 < flag_L1[i]; k1++)
                        {
                            if (equal(friendpair[i * 128 + 2 * j], friendpair[i * 128 + 2 * k1]) && equal(friendpair[i * 128 + 2 * j + 1], friendpair[i * 128 + 2 * k1 + 1]))
                            {
                                flag_L1[i]--;
                                break;
                            }
                            if (equal(friendpair[i * 128 + 2 * j + 1], friendpair[i * 128 + 2 * k1]) && equal(friendpair[i * 128 + 2 * j], friendpair[i * 128 + 2 * k1 + 1]))
                            {
                                flag_L1[i] --;
                                break;
                            }
                        }

                }
            }


            //printf("\n\nAfter the first Em\n");
            //for (i = 0; i < L_index; i++)
            //{
             //   printf("i=%d,flag1=%u\n", i,flag_L1[i]);
            //}

          
            memset(friendpair, 0, L_index * 128 * sizeof(state));


            for (i = 0; i < L_index; i++)    //the second middle boomerang, construct 583436336 friend pairs for each plaintext pair
            {
                flag_L2[i] = 0;
                if (flag_L1[i] > 1)
                {
                    memcpy(x, L[2 * i], sizeof(state));
                    memcpy(y, L[2 * i + 1], sizeof(state));

                    for (j = 0; j < 583436336; j++)
                    {
                        m_seq(ran);
                        memcpy(plain1, &ran[2], sizeof(state));
                        memcpy(plain2, plain1, sizeof(state));
                        plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                        plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                        plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                        plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                        plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                        plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                        plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                        plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);

                        while (equal(plain1, x) && (equal(plain2, y)))
                        {
                            m_seq(ran);
                            memcpy(plain1, &ran[2], sizeof(state));
                            memcpy(plain2, plain1, sizeof(state));
                            plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                            plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                            plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                            plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                            plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                            plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                            plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                            plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);
                        }
                        Encrypt(plain1, key, cipher1);
                        Encrypt(plain2, key, cipher2);
                        if (weight_inver(cipher1, cipher2) == 4)
                        {
                            for (k1 = 0; k1 < 4; k1++)
                            {
                                exchange(cipher1, cipher2, k1, newcipher1, newcipher2);
                                if (equal(newcipher1, cipher1) || equal(newcipher1, cipher2))
                                    continue;
                                else
                                {
                                    Decrypt(newcipher1, keyinv, newplain1);
                                    Decrypt(newcipher2, keyinv, newplain2);
                                    if (weight(newplain1, newplain2) ==2)
                                    {
                                        memcpy(friendpair[i * 128 + flag_L2[i] * 2], newplain1, sizeof(state));
                                        memcpy(friendpair[i * 128 + flag_L2[i] * 2 + 1], newplain2, sizeof(state));
                                        flag_L2[i]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (i = 0; i < L_index; i++)
            {
                if (flag_L2[i] > 12)
                {
                    for (j = 0; j < flag_L2[i]; j++)
                        for (k1 = j + 1; k1 < flag_L2[i]; k1++)
                        {
                            if (equal(friendpair[i * 128 + 2 * j], friendpair[i * 128 + 2 * k1]) && equal(friendpair[i * 128 + 2 * j + 1], friendpair[i * 128 + 2 * k1 + 1]))
                            {
                                flag_L2[i]--;
                                break;
                            }
                            if (equal(friendpair[i * 128 + 2 * j + 1], friendpair[i * 128 + 2 * k1]) && equal(friendpair[i * 128 + 2 * j], friendpair[i * 128 + 2 * k1 + 1]))
                            {
                                flag_L2[i] --;
                                break;
                            }
                        }

                }
            }

           // printf("\n\nAfter the second Em\n");
           // for (i = 0; i < L_index; i++)
           // {
            //    printf("i=%d,flag2=%u\n",i, flag_L2[i]);  
           // }


            for (i = 0; i < L_index; i++)    //the second boomerang B2, construct 5361538882 friend pairs for each plaintext pair
            {
                if (flag_L2[i] > 12)
                {
                    memcpy(x, L[2 * i], sizeof(state));
                    memcpy(y, L[2 * i + 1], sizeof(state));


                    for (j = 0; j < 5361538882; j++)
                    {
                        m_seq(ran);
                        memcpy(plain1, &ran[2], sizeof(state));
                        memcpy(plain2, plain1, sizeof(state));
                        plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                        plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                        plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                        plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                        plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                        plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                        plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                        plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);

                        while (equal(plain1, x) && (equal(plain2, y)))
                        {
                            m_seq(ran);
                            memcpy(plain1, &ran[2], sizeof(state));
                            memcpy(plain2, plain1, sizeof(state));
                            plain1[0] = (x[0] & M0) ^ (plain1[0] & N0);
                            plain1[1] = (x[1] & M1) ^ (plain1[1] & N1);
                            plain1[2] = (x[2] & M2) ^ (plain1[2] & N2);
                            plain1[3] = (x[3] & M3) ^ (plain1[3] & N3);

                            plain2[0] = (y[0] & M0) ^ (plain2[0] & N0);
                            plain2[1] = (y[1] & M1) ^ (plain2[1] & N1);
                            plain2[2] = (y[2] & M2) ^ (plain2[2] & N2);
                            plain2[3] = (y[3] & M3) ^ (plain2[3] & N3);
                        }
                        Encrypt(plain1, key, cipher1);
                        Encrypt(plain2, key, cipher2);
                        if (weight_inver(cipher1, cipher2) == 4)
                        {
                            for (k1 = 0; k1 < 4; k1++)
                            {
                                exchange(cipher1, cipher2, k1, newcipher1, newcipher2);
                                if (equal(newcipher1, cipher1) || equal(newcipher1, cipher2))
                                    continue;
                                else
                                {
                                    Decrypt(newcipher1, keyinv, newplain1);
                                    Decrypt(newcipher2, keyinv, newplain2);
                                    if (weight(newplain1, newplain2) ==1)
                                    {
                                        count[t]++;
                                        break;
                                    }
                                }
                            }
                        }

                        if (count[t] > 0)
                            break;

                        
                    }

                    if (count[t] > 0)
                        break;
                }
            }

            if (count[t] > 0)
            {
                break;
            }

        }

    }

    counter = 0;
    for (t = 0; t < test_number; t++)    //t:the number of experiments
    {
        if (count[t] > 0)
        {
            counter++;
        }
    }
    printf("there are %d experiments in total, where %d results is 6-round AES\n", test_number, counter);
    endtime = time(NULL);
    printf("time=%f\n", difftime(endtime, starttime));

}


