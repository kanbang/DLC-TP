#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "gmp.h"

int FirstPrime[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};

unsigned long int cputime()
{
    struct rusage rus;
    getrusage(0, &rus);
    return rus.ru_utime.tv_sec * 1000 + rus.ru_utime.tv_usec / 1000;
}

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

int simple_check_prime(int b, int k, int t, mpz_t n, gmp_randstate_t state)
{
    mpz_t w[k];
    do
    {
        random_exact_k_bit(n, b, state);
    } while (mpz_divisible_ui_p(n, 2) != 0);

    int j, i;
    for (j = 1; j < k; j++)
    {
        mpz_init(w[j]);
        mpz_mod_ui(w[j], n, FirstPrime[j]);
    }

    int isPrime;
    do
    {
        isPrime = 1;
        for (j = 1; j < k; j++)
        {
            if (mpz_cmp_d(w[j], 0) == 0)
            {
                for (i = 1; i < k; i++)
                {
                    mpz_add_ui(w[i], w[i], 2);
                    mpz_mod_ui(w[i], w[i], FirstPrime[i]);
                }
                mpz_add_ui(n, n, 2);
            }
        }

        isPrime = test_miller_rabin(n, t);
        if (!isPrime)
        {
            for (i = 1; i < k; i++)
            {
                mpz_add_ui(w[i], w[i], 2);
                mpz_mod_ui(w[i], w[i], FirstPrime[i]);
            }
            mpz_add_ui(n, n, 2);
        }
    } while (!isPrime);

    for (j = 1; j < k; j++)
    {
        mpz_clear(w[j]);
    }
    return isPrime;
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Enter param");
        return -1;
    }
    int b = atoi(argv[1]);
    int t = atoi(argv[2]);
    int k = atoi(argv[3]);
    int r = atoi(argv[4]);

    mpz_t n;

    mpz_init(n);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    long int seed = time(NULL);
    gmp_randseed_ui(state, seed);

    int i;
    for (i = 0; i < 30; i++)
    {
        int count = 0;
        int found = 0;
        long int starttime = cputime();
        while (found < r)
        {
            int isPrime = simple_check_prime(b, k, t, n, state);
            if (isPrime)
            {
                // gmp_printf("%Zd, %d\n", n, isPrime);
                found++;
            }
            count++;
        }
        long int endtime = cputime();
        printf("%ld\n", endtime - starttime);
        k++;
        // printf("%d pseudoprime Miller Rabin found\n", found);
    }

    gmp_randclear(state);
    mpz_clears(n, NULL);

    return 1;
}