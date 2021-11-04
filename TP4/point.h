#include "gmp.h"

struct point
{
    mpz_t x, y;
};

struct point init_point()
{
    struct point p;
    mpz_inits(p.x, p.y, NULL);
    return p;
}

struct point init_point_set(mpz_t x, mpz_t y)
{
    struct point p;
    mpz_init_set(p.x, x);
    mpz_init_set(p.y, y);
    return p;
}

void clear_point(struct point p)
{
    mpz_clears(p.x, p.y, NULL);
}
