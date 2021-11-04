#include "point.h"

int isOnCourbe(struct point P, mpz_t p, mpz_t a, mpz_t b)
{
    int isOn = 0;
    mpz_t result;
    mpz_inits(result, NULL);

    mpz_pow_ui(result, P.x, 3);
    mpz_addmul(result, P.x, a);
    mpz_add(result, result, b);
    mpz_submul(result, P.y, P.y);
    mpz_mod(result, result, p);
    if (mpz_cmp_d(result, 0) == 0)
    {
        isOn = 1;
    }

    mpz_clears(result, NULL);
    return isOn;
}

int addition(struct point *R, struct point P, struct point Q, mpz_t p, mpz_t a, mpz_t b)
{
    if (isOnCourbe(P, p, a, b) && isOnCourbe(Q, p, a, b))
    {
        struct point P_temp = init_point_set(P.x, P.y);
        struct point Q_temp = init_point_set(Q.x, Q.y);

        mpz_t lamda;
        mpz_init(lamda);

        // lamda = (yQ - yP) / (xQ - xP)
        mpz_sub(R->x, Q_temp.x, P_temp.x);
        mpz_mod(R->x, R->x, p);

        mpz_invert(R->x, R->x, p);

        mpz_sub(R->y, Q_temp.y, P_temp.y);
        mpz_mod(R->y, R->y, p);

        mpz_mul(lamda, R->y, R->x);
        mpz_mod(lamda, lamda, p);

        //xR = lamda2 - xP - xQ
        mpz_powm_ui(R->x, lamda, 2, p);
        mpz_sub(R->x, R->x, P_temp.x);
        mpz_mod(R->x, R->x, p);

        mpz_sub(R->x, R->x, Q_temp.x);
        mpz_mod(R->x, R->x, p);

        //yR = lamda * (xP - xR) - yP
        mpz_sub(R->y, P_temp.x, R->x);
        mpz_mod(R->y, R->y, p);

        mpz_mul(R->y, lamda, R->y);
        mpz_mod(R->y, R->y, p);

        mpz_sub(R->y, R->y, P_temp.y);
        mpz_mod(R->y, R->y, p);

        clear_point(P_temp);
        clear_point(Q_temp);
        mpz_clear(lamda);
        return 0;
    }
    printf("addition is NOT valid\n");

    return -1;
}

int doublement(struct point *R, struct point P, mpz_t p, mpz_t a, mpz_t b)
{
    if (isOnCourbe(P, p, a, b))
    {
        struct point P_temp = init_point_set(P.x, P.y);
        mpz_t lamda;
        mpz_init(lamda);

        // lamda = (3xP2 + a) / 2yP
        mpz_powm_ui(R->x, P_temp.x, 2, p);
        mpz_mul_ui(R->x, R->x, 3);
        mpz_add(R->x, R->x, a);
        mpz_mod(R->x, R->x, p);

        mpz_mul_ui(lamda, P_temp.y, 2);
        mpz_invert(lamda, lamda, p);
        mpz_mul(lamda, R->x, lamda);
        mpz_mod(lamda, lamda, p);

        //xR = lamda2 - 2xP
        mpz_powm_ui(R->x, lamda, 2, p);
        mpz_submul_ui(R->x, P_temp.x, 2);
        mpz_mod(R->x, R->x, p);

        //yR = lamda * (xP - xR) - yP
        mpz_sub(R->y, P_temp.x, R->x);
        mpz_mul(R->y, lamda, R->y);
        mpz_sub(R->y, R->y, P_temp.y);
        mpz_mod(R->y, R->y, p);

        clear_point(P_temp);
        mpz_clear(lamda);
        return 0;
    }
    printf("doublement is NOT valid\n");

    return -1;
}

int multiple(struct point *R, struct point P, mpz_t k, mpz_t p, mpz_t a, mpz_t b)
{
    if (isOnCourbe(P, p, a, b))
    {
        mpz_t k_temp, count, quotient, remainder, countRemain;
        struct point R_temp = init_point_set(P.x, P.y);

        mpz_inits(quotient, remainder, NULL);
        mpz_init_set_d(count, 0);
        mpz_init_set_d(countRemain, 0);
        mpz_init_set(k_temp, k);

        int isInitR = 0;
        if (mpz_odd_p(k_temp))
        {
            mpz_sub_ui(k_temp, k_temp, 1);
            mpz_set(R->x, P.x);
            mpz_set(R->y, P.y);
            isInitR = 1;
        }

        mpz_set(quotient, k_temp);
        do
        {
            mpz_fdiv_qr_ui(quotient, remainder, quotient, 2);
            if (mpz_cmp_d(remainder, 1) == 0)
            {
                while (mpz_cmp(countRemain, count) < 0)
                {
                    doublement(&R_temp, R_temp, p, a, b);
                    mpz_add_ui(countRemain, countRemain, 1);
                }

                if (!isInitR)
                {
                    mpz_set(R->x, R_temp.x);
                    mpz_set(R->y, R_temp.y);
                    isInitR = 1;
                }
                else
                {
                    addition(R, *R, R_temp, p, a, b);
                }
            }

            mpz_add_ui(count, count, 1);
        } while (mpz_cmp_d(quotient, 0) != 0);

        clear_point(R_temp);
        mpz_clears(k_temp, count, quotient, remainder, countRemain, NULL);
        return 0;
    }

    printf("multiple is NOT valid\n");
    return -1;
}

int multipleV2(struct point *R, struct point P, mpz_t k, mpz_t p, mpz_t a, mpz_t b)
{
    if (isOnCourbe(P, p, a, b))
    {
        mpz_t k_temp;
        struct point R_temp = init_point_set(P.x, P.y);
        mpz_init_set(k_temp, k);

        if (mpz_cmp_d(k_temp, 1) == 0)
        {
            mpz_set(R->x, P.x);
            mpz_set(R->y, P.y);
        }

        if (mpz_cmp_d(k_temp, 0) > 0)
        {
            if (mpz_odd_p(k_temp))
            {
                if (mpz_cmp_d(k_temp, 1) == 0)
                {
                    mpz_sub_ui(k_temp, k_temp, 1);
                    multipleV2(R, P, k_temp, p, a, b);
                }
                else
                {
                    mpz_sub_ui(k_temp, k_temp, 1);
                    multipleV2(R, P, k_temp, p, a, b);
                    addition(R, *R, P, p, a, b);
                }
            }
            else
            {
                mpz_fdiv_q_ui(k_temp, k_temp, 2);
                multipleV2(R, P, k_temp, p, a, b);
                doublement(R, *R, p, a, b);
            }
        }

        clear_point(R_temp);
        mpz_clear(k_temp);
        return 0;
    }

    printf("multiple is NOT valid\n");
    return -1;
}