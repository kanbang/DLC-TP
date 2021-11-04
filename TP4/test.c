#include "gmp.h"
#include <stdio.h>
#include "ECDSA.h"
#include "file.h"
#include <time.h>
#define SIGN_FILE_NAME "message.txt"


int main(int argc, char *argv[])
{
    char *buffer;
    buffer = readFile(SIGN_FILE_NAME);
    printf("%s\n%d\n", buffer, lengthStr(buffer));
    writeFile("signature.txt", buffer);
    free(buffer);
    buffer = readFile("signature.txt");
    printf("%s\n%d\n", buffer, lengthStr(buffer));
    free(buffer);
}