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
        bytesPerBlock--;
    }

    mpz_clears(quotient, remain, NULL);
}

void signECDSA(char *messageFile, struct point G, mpz_t d, mpz_t n, mpz_t p, mpz_t a, mpz_t b)
{
    hashingMD5(messageFile);

    char *contentFile = readFile(HASH_FILE_NAME);

    char str[1000];
    mpz_get_str(str, 16, n);
    int bytesPerBlock = lengthStr(str) / 2 - 1;

    // append sufficiently character
    int sizeContentFile = lengthStr(contentFile);
    int appendCharacter = bytesPerBlock - (sizeContentFile % bytesPerBlock);
    char contentFileAppend[sizeContentFile + appendCharacter];
    contentFileAppend[0] = '\0';
    concat(contentFileAppend, contentFile);
    for (; appendCharacter > 0; appendCharacter--)
    {
        contentFileAppend[sizeContentFile] = 127;
        sizeContentFile++;
    }

    int numberBlock = (sizeContentFile + appendCharacter) / bytesPerBlock;
    int sizeSignature = (bytesPerBlock + 1) * (numberBlock + 1);
    char signature[sizeSignature];
    signature[sizeSignature] = 0;
    char buffer[bytesPerBlock + 1];
    int countByte = 0;
    mpz_t rop, k, r;
    mpz_inits(rop, k, r, NULL);

    // random k in [1,n-1]
    gmp_randstate_t state;
    gmp_randinit_default(state);
    int seed = time(NULL);
    gmp_randseed_ui(state, seed);
    mpz_urandomm(k, state, n);
    if (mpz_cmp_d(k, 0) == 0)
    {
        mpz_add_ui(k, k, 1);
    }

    // calculate P = k.G
    struct point P = init_point();
    multipleV2(&P, G, k, p, a, b);

    // calculate r, k inverse
    mpz_mod(r, P.x, n);
    mpz_invert(k, k, n);

    // signing
    int j = 0;
    for (int i = 0; i < lengthStr(contentFileAppend); i++)
    {
        buffer[countByte] = contentFileAppend[i];
        countByte++;
        if (countByte == bytesPerBlock)
        {
            convertBytesToDecimal(rop, buffer, bytesPerBlock);
            mpz_addmul(rop, r, d);
            mpz_mul(rop, k, rop);
            mpz_mod(rop, rop, n);

            convertDecimalToBytes(rop, buffer, bytesPerBlock + 1);

            for (; countByte > -1; countByte--)
            {
                signature[j + countByte] = buffer[countByte];
            }
            j += (bytesPerBlock + 1);
            countByte = 0;
        }

    }

    // store r to signature
    convertDecimalToBytes(r, buffer, bytesPerBlock + 1);
    for (countByte = 0; countByte < bytesPerBlock + 1; countByte++)
    {
        signature[j + countByte] = buffer[countByte];
    }
    signature[(bytesPerBlock + 1) * (numberBlock + 1)] = '\0';

    printf("size signature: %d\n", lengthStr(signature));
    writeFile(SIGN_FILE_NAME, signature);


    free(contentFile);
    mpz_clears(rop, k, r, NULL);
    gmp_randclear(state);
    clear_point(P);
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

    mpz_t p, b, a, d, n;

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

    // read n
    buffer = readFileByLine(fileName, 5, buffer);
    value = getSubStr(buffer, 4, lengthStr(buffer));
    mpz_init_set_str(n, value, 0);
    free(value);

    // read d
    buffer = readFileByLine(keyfileName, 0, buffer);
    value = getSubStr(buffer, 5, lengthStr(buffer));
    mpz_init_set_str(d, value, 0);
    free(value);

    gmp_printf("d   = 0x%Zx\n", d);

    signECDSA(messageFile, G, d, n, p, a, b);

    clear_point(G);

    mpz_clears(p, b, a, d, n, NULL);
    return 0;
}