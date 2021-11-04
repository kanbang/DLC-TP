#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HASH_FILE_NAME "hash.txt"

int lengthStr(char *ptr)
{
    int offset = 0;
    int count = 0;

    while (ptr[offset] != '\0')
    {
        ++count;
        ++offset;
    }
    return count;
}

void concat(char *str, char *addstr)
{
    int size = lengthStr(str);
    int i;
    for (i = 0; i < lengthStr(addstr); i++)
    {
        str[size] = addstr[i];
        size++;
    }
    str[size] = '\0';
}

void hashingMD5(char *messagefile)
{
    char cmd[100] = "md5sum ";
    concat(cmd, messagefile);
    concat(cmd, " | awk '{print $1}' | tr -d '\n' > ");
    concat(cmd, HASH_FILE_NAME);
    system(cmd);
}

char *getSubStr(char *str, int start, int end)
{
    if (start < end)
    {
        int size = end - start + 1;
        char *subStr = (char *)malloc(size * sizeof(char));
        int i = 0;
        for (;start < end; start++)
        {
            subStr[i] = str[start];
            i++;
        }
        subStr[size-1] = '\0';
        return subStr;
    }

    return NULL;
}

void writeFile(char *fileName, char *value)
{
    FILE *fptr;
    fptr = fopen(fileName, "w");
    // fwrite(&value, 1, lengthStr(value), fptr);
    fprintf(fptr, "%s",value);
    fclose(fptr);
}

char* readFileByLine(char *fileName, int line, char *value)
{
    // printf("%s\n", file);
    int bufferLength = 255;
    char buffer[bufferLength];
    // printf("%s\n", fileName);
    FILE *fptr;
    fptr = fopen(fileName, "r");
    if (fptr == NULL)
    {
        printf("No file, %s and\n", fileName);
        return NULL;
    }
    int j = 0;

    while (fgets(buffer, bufferLength, fptr))
    {
        value = buffer;
        value[bufferLength] = '\0';
        if (j == line)
        {
            break;
        }
        j++;
    }
    fclose(fptr);

    return value;
}

void readDoubleFileByLine(char *fileName, double *data)
{
    // printf("%s\n", file);
    int bufferLength = 255;
    char buffer[bufferLength];
    printf("%s\n", fileName);
    FILE *fptr;
    fptr = fopen(fileName, "r");
    if (fptr == NULL)
    {
        printf("No file, %s and\n", fileName);
        return;
    }
    int j = 0;
    char *eptr;

    while (fgets(buffer, bufferLength, fptr))
    {
        data[j] = strtod(buffer, &eptr);
        j++;
    }
    fclose(fptr);
}

char *readFile(char *fileName)
{
    char *buffer;
    FILE *fp = fopen(fileName, "r");
    if (fp != NULL)
    {
        fseek(fp, 0L, SEEK_END);
        long stell = ftell(fp);
        rewind(fp);
        buffer = (char *)malloc(stell +1);
        if (buffer != NULL)
        {
            fread(buffer, stell, 1, fp);
            fclose(fp);
            fp = NULL;
        }
    }
    return buffer;
}

void readFileByLineKnownLength(char *fileName, char *strArray[], int nrows, int ncolumns)
{
    FILE *fp; // pointer to file
    int i, j;
    //char content[MAX_NUM_LINES][MAX_LINE_LENGTH];

    for (i = 0; i < nrows; i++)
    {
        strArray[i] = (char *)malloc(ncolumns * sizeof(char));
        for (j = 0; j < ncolumns; j++)
        {
            strArray[i][j] = 0;
        }
    }
    // // has file argument
    fp = fopen(fileName, "r");

    if (fp != NULL)
    {
        i = 0;
        while (fgets(strArray[i], ncolumns, fp))
        {
            fgetc(fp);
            i++;
        }
    }
    fclose(fp);
}

int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

int hexbyte2int(char chl, char chr)
{
    int number = hex2int(chl);
    int number2 = hex2int(chr);
    return number2 + number * 16;
}

int hex2intArray(char *hexStr, char *hex)
{
    int i, j = 0;
    int size = strlen(hexStr);
    for (i = 0; i < size; i += 2)
    {
        hex[j] = hexbyte2int(hexStr[i], hexStr[i + 1]);
        j++;
    }
}