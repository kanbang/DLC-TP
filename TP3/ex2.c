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

int extract_2exp(mpz_t n1, mpz_t r)
{
    int s = 1;

    while (mpz_divisible_2exp_p(n1, s) != 0)
    {
        s++;
    }
    s--;
    mpz_div_2exp(r, n1, s);
    return s;
}

int test_miller_rabin_base(mpz_t n, mpz_t a)
{
    mpz_t y, n1, r;
    mpz_inits(y, r, NULL);
    mpz_init_set(n1, n);
    mpz_sub_ui(n1, n1, 1);

    int s = extract_2exp(n1, r);
    mpz_powm(y, a, r, n);

    int isPrime = 1;
    if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n1) != 0)
    {
        int j = 1;
        while (j <= s - 1 && mpz_cmp(y, n1) != 0)
        {
            mpz_powm_ui(y, y, 2, n);
            if (mpz_cmp_ui(y, 1) == 0)
            {
                isPrime = 0;
                break;
            }
            j++;
        }

        if (mpz_cmp(y, n1) != 0)
        {
            isPrime = 0;
        }
    }

    mpz_clears(y, n1, r, NULL);
    return isPrime;
}

int test_miller_rabin(mpz_t n, int t)
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

        isPrime = test_miller_rabin_base(n, a);

        if (isPrime != 1)
        {
            break;
        }
    }

    gmp_randclear(state);
    mpz_clears(a, n4, NULL);
    return isPrime;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Enter param");
        return -1;
    }
    int b = atoi(argv[1]);
    int t = atoi(argv[2]);
    int r = atoi(argv[3]);

    mpz_t n;

    mpz_init(n);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    long int seed = time(NULL);
    gmp_randseed_ui(state, seed);

    int count = 0;
    int found = 0;
    while (count < r)
    {
        random_exact_k_bit(n, b, state);
        int isPrime = test_miller_rabin(n, t);

        if (isPrime)
        {
            isPrime = mpz_probab_prime_p(n, 10);
            if (isPrime == 2 || isPrime == 1)
            {
                gmp_printf("%Zd, %d\n", n, isPrime);
                found++;
            }
        }
        count++;
    }
    printf("%d pseudoprime Miller Rabin found\n", found);

    count = 0;
    found = 0;
    while (count < r)
    {
        random_exact_k_bit(n, b, state);
        int isPrime = test_miller_rabin(n, t);

        if (isPrime)
        {
            isPrime = mpz_probab_prime_p(n, 10);
            if (isPrime == 2 || isPrime == 1)
            {
                gmp_printf("%Zd, %d\n", n, isPrime);
                found++;
            }
        }
        count++;
    }
    printf("%d pseudoprime Fermat found\n", found);
    gmp_randclear(state);
    mpz_clears(n, NULL);

    return 1;
}