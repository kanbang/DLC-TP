#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gmp.h"

int random_exact_k_bit(mpz_t n, int k, gmp_randstate_t state)
{
    mpz_t rop;
    mpz_init_set_ui(rop, 2);
    mpz_urandomb(n, state, k - 1);
    mpz_pow_ui(rop, rop, k - 1);

    mpz_add(n, n, rop);

    mpz_clear(rop);
    return 1;
}

int test_fermat_base(mpz_t n, mpz_t a)
{
    mpz_t y, n1;
    mpz_init(y);
    mpz_init_set(n1, n);
    mpz_sub_ui(n1, n1, 1);

    mpz_powm(y, a, n1, n);

    int isPrime = 1;
    if (mpz_cmp_ui(y, 1) != 0)
        isPrime = 0;

    mpz_clears(y, n1, NULL);
    return isPrime;
}

int test_fermat(mpz_t n, int t)
{
    int i;
    mpz_t a, n4;
    mpz_init(a);
    mpz_init_set(n4, n);
    mpz_sub_ui(n4, n4, 4);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    int seed = time(NULL);
    gmp_randseed_ui(state, seed);

    int isPrime = 1;
    for (i = 0; i < t; i++)
    {
        mpz_urandomm(a, state, n4);
        mpz_add_ui(a, a, 2);

        isPrime = test_fermat_base(n, a);

        if (isPrime != 1)
        {
            break;
        }
    }

    gmp_randclear(state);
    mpz_clears(a, n4, NULL);
    return isPrime;
}

int main(int argc, char* argv[])
{
    if(argc < 3){
        printf("Enter param");
        return -1;
    }
    int b = argv[1];
    int t = argv[2];


    mpz_t n;

    mpz_init(n);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    long int seed = time(NULL);
    gmp_randseed_ui(state, seed);

    random_exact_k_bit(n, 4, state);
    int isPrime = test_fermat(n, 10);
    gmp_printf("%Zd, %d\n", n, isPrime);


    gmp_randclear(state);
    mpz_clears(n, NULL);

    return 1;
}