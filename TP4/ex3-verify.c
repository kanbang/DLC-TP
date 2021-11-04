#include "gmp.h"
#include <stdio.h>
#include "ECDSA.h"
#include "file.h"
#include <time.h>

#define SIGN_FILE_NAME "signature.txt"

void convertBytesToDecimal(mpz_t rop, unsigned char *buffer, int bytesPerBlock)
{
    int i;
    mpz_t pow_rs;
    mpz_init(pow_rs);
    mpz_set_d(rop, 0);
    for (i = 0; i < bytesPerBlock; i++)
    {
        mpz_ui_pow_ui(pow_rs, 256, bytesPerBlock - i - 1);
        mpz_addmul_ui(rop, pow_rs, buffer[i]);
    }

    mpz_clear(pow_rs);
}

void convertDecimalToBytes(mpz_t rop, unsigned char *buffer, int bytesPerBlock)
{
    int i;
    mpz_t quotient, remain;
    mpz_init_set(quotient, rop);
    mpz_init(remain);

    bytesPerBlock--;
    while (mpz_cmp_d(quotient, 0) > 0)
    {
        mpz_fdiv_qr_ui(quotient, remain, quotient, 256);
        buffer[bytesPerBlock] = mpz_get_ui(remain);
        // gmp_printf("%d %d %Zd\n", bytesPerBlock, (unsigned char) buffer[bytesPerBlock], remain);
        bytesPerBlock--;
    }

    mpz_clears(quotient, remain, NULL);
}

int verifyECDSA(char *messageFile, char *signatureFile, struct point G, struct point Q, mpz_t n, mpz_t p, mpz_t a, mpz_t b)
{
    int isVerifySuccess = 0;
    hashingMD5(messageFile);

    char *contentHashFile = readFile(HASH_FILE_NAME);
    char *contentFile = readFile(signatureFile);

    char str[1000];
    mpz_get_str(str, 16, n);
    int bytesPerBlock = lengthStr(str) / 2;

    // check sufficiently character
    int sizeContentFile = lengthStr(contentFile);
    if (sizeContentFile % bytesPerBlock != 0)
    {
        isVerifySuccess = -1;
    }
    else
    {
        int numberBlock = (sizeContentFile / bytesPerBlock) - 1;

        // append sufficiently character
        int sizeContentHashFile = lengthStr(contentHashFile);
        int appendCharacter = numberBlock*(bytesPerBlock - 1) - sizeContentHashFile;
        char contentHashAppend[numberBlock*(bytesPerBlock - 1)];
        contentHashAppend[0] = '\0';
        concat(contentHashAppend, contentHashFile);
        for (; appendCharacter > 0; appendCharacter--)
        {
            contentHashAppend[sizeContentHashFile] = 127;
            sizeContentHashFile++;

        }
        
        char verifyValue[(bytesPerBlock - 1) * numberBlock];
        char buffer[bytesPerBlock];
        char buffer_Z[bytesPerBlock - 1];
        int countByte;
        mpz_t rop, r, u1, u2, z;
        mpz_inits(rop, r, u1, u2, z, NULL);

        // P
        struct point P = init_point();
        struct point R = init_point();

        // recover r
        for (countByte = 0; countByte < bytesPerBlock; countByte++)
        {
            buffer[countByte] = contentFile[sizeContentFile - bytesPerBlock + countByte];
        }
        convertBytesToDecimal(r, buffer, bytesPerBlock);
        // gmp_printf("\nr = %Zd\n", r);

        // verifying
        int j = 0;
        countByte = 0;
        for (int i = 0; i < (sizeContentFile - bytesPerBlock); i++)
        {

            buffer[countByte] = contentFile[i];

            if (countByte != (bytesPerBlock -1))
            {
                buffer_Z[countByte] = contentHashAppend[j];
                j++;
            }
            
            countByte++;
            if (countByte == bytesPerBlock)
            {
                convertBytesToDecimal(rop, buffer, bytesPerBlock);
                convertBytesToDecimal(z, buffer_Z, bytesPerBlock - 1);

                mpz_invert(rop, rop, n);
                mpz_mul(u1, rop, z);
                mpz_mod(u1, u1, n);

                mpz_mul(u2, rop, r);
                mpz_mod(u2, u2, n);

                multipleV2(&P, G, u1, p, a, b);
                multipleV2(&R, Q, u2, p, a, b);
                addition(&P, P, R, p, a, b);

                if (mpz_cmp(P.x, r) != 0)
                {
                    printf("Fail at %d\n", i);
                    isVerifySuccess = -1;
                    break;
                }
                countByte = 0;
            }
        }

        mpz_clears(rop, r, u1, u2, z, NULL);
        clear_point(P);
        clear_point(R);
    }

    free(contentFile);
    free(contentHashFile);
    return isVerifySuccess;
}

int main(int argc, char *argv[])
{
    char *fileName = "coubre.txt";
    char *keyfileName = "keyECDSA.txt";

    char *value;
    char *buffer;
    if (argc < 2)
    {
        printf("Insufficient parameters");
        return -1;
    }
    char *messageFile = argv[1];

    mpz_t p, b, a, n;

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
    mpz_init_set_str(G.x, value, 0);
    free(value);

    // read G.y
    buffer = readFileByLine(fileName, 4, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(G.y, value, 0);
    free(value);

    // read n
    buffer = readFileByLine(fileName, 5, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(n, value, 0);
    free(value);

    // read Q.x
    struct point Q = init_point();
    buffer = readFileByLine(keyfileName, 1, buffer);
    value = getSubStr(buffer, 5, lengthStr(buffer));
    mpz_init_set_str(Q.x, value, 0);
    free(value);

    // read Q.y
    buffer = readFileByLine(keyfileName, 2, buffer);
    value = getSubStr(buffer, 5, lengthStr(buffer));
    mpz_init_set_str(Q.y, value, 0);
    free(value);

    gmp_printf("Q_x = 0x%Zx\n", Q.x);
    gmp_printf("Q_y = 0x%Zx\n", Q.y);

    int verify = verifyECDSA(messageFile, SIGN_FILE_NAME, G, Q, n, p, a, b);
    if (verify == 0)
    {
        printf("Verify Success\n");
    }
    else
    {
        printf("Verify Fail\n");
    }

    clear_point(G);
    clear_point(Q);

    mpz_clears(p, b, a, n, NULL);
    return 0;
}