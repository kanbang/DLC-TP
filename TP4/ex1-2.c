#include "gmp.h"
#include <stdio.h>
#include "ECDSA.h"
#include "file.h"

int main(int argc, char *argv[])
{
    char *fileName = "coubre.txt";
    char* value;
    char*  buffer;
    char *kStr = argv[1];

    mpz_t p, b, a, k;
    
    buffer[255];

    // read p
    buffer = readFileByLine(fileName, 0, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(p, value, 0);
    free(value);

    // read b
    buffer = readFileByLine(fileName, 1, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(b, value, 0);
    free(value);

    // read a
    buffer = readFileByLine(fileName, 2, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(a, value, 0);
    free(value);

    // read G.x
    struct point G = init_point();
    buffer = readFileByLine(fileName, 3, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    // printf("%s %s %d %d\n", buffer, value, lengthStr(buffer), lengthStr(value));
    mpz_init_set_str(G.x, value, 0);
    free(value);

    // read G.y
    buffer = readFileByLine(fileName, 4, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(G.y, value, 0);
    free(value);

    // read k
    mpz_init_set_str(k, kStr, 10);

    // gmp_printf("p = %Zd\n", p);
    // gmp_printf("b = %Zd\n", b);
    // gmp_printf("a = %Zd\n", a);
    // gmp_printf("Gx= %Zd\n", G.x);
    // gmp_printf("Gy= %Zd\n", G.y);

    struct point R = init_point();

    doublement(&R, G, p, a, b);
    gmp_printf("2Gx = %Zd\n", R.x);
    gmp_printf("2Gy = %Zd\n\n", R.y);

    addition(&R, G, R, p, a, b);
    gmp_printf("3Gx = %Zd\n", R.x);
    gmp_printf("3Gy = %Zd\n\n", R.y);

    multiple(&R, G, k, p, a, b);
    gmp_printf("%ZdGx = %Zd\n", k, R.x);
    gmp_printf("%ZdGy = %Zd\n\n", k, R.y);

    multipleV2(&R, G, k, p, a, b);
    gmp_printf("%ZdGx = %Zd\n", k, R.x);
    gmp_printf("%ZdGy = %Zd\n\n", k, R.y);

    // mpz_clears(a.x, a.y, NULL);

    clear_point(G);
    clear_point(R);
    mpz_clears(p, b, a, k, NULL);
}