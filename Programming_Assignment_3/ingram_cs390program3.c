#include <stdio.h>
#include <malloc.h>
#include <string.h> 
#include <ctype.h>

unsigned char *buffer;
int buffersize = 0, datasize = 0;

#define ONE_MB 1000000
int ReadBinaryFile(char *filename);
int IsItText(unsigned char *buffer, int datasize);
int ToUnix(unsigned char *buffer, int datasize, char *filename);
int HasU( int argc, char *argv[]);
int GetPathIndex(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int status;
    char *filename = argv[GetPathIndex(argc, argv)];
    buffersize = 5 * ONE_MB;
    buffer = (unsigned char *)malloc(buffersize);
    status = ReadBinaryFile(filename);
    if (status > 0)
    {
        if (status == 1)
            fprintf(stderr, "COULD NOT OPEN FILE!!\n");
        else if (status == 2)
            fprintf(stderr, "FILE IS ZERO LENGTH!!\n");
        else if (status == 3)
            fprintf(stderr, "FILE TOO BIG FOR BUFFER!!\n");
        else if (status == 4)
            fprintf(stderr, "ERROR READING FILE!!\n");
    }
    else
    {
        if (!IsItText(buffer, datasize))
        {
            fprintf(stderr, "FILE %s IS *NOT* TEXT\n", filename);
        }
        else
        {
            if (strstr((char *)buffer, "\r"))
            {
                fprintf(stdout, "FILE IS WINDOWS\n");

                if (HasU(argc, argv))
                {
                    fprintf(stdout, "CONVERTING TO UNIX\n");
                    ToUnix(buffer, datasize, filename);
                }
            }
            else
            {
                fprintf(stdout, "FILE IS UNIX\n");
            }
        }
    }
    if (buffer)
        free(buffer);
    return 0;
}

/* Check if there is a supplied Path, and return it's index in argv */
int GetPathIndex(int argc, char *argv[])
{
    int i;
    for ( i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-u") != 0)
        {
            return i;
        }
    }
}

/* Has -u operator */
int HasU(int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-u") == 0)
        {
            return 1;
        }
    }
    return 0;
}

int ReadBinaryFile(char *filename)
{
    int status = 0;
    int bytesInFile = 0;
    FILE *ifs = 0;
    ifs = fopen(filename, "rb");
    if (!ifs)
    {
        status = 1;
    }
    else
    { /* make sure file will fit into buffer */
        fseek(ifs, 0, SEEK_END);
        bytesInFile = (int)ftell(ifs);
        fseek(ifs, 0, SEEK_SET);

        if (bytesInFile == 0)
        {
            status = 2;
        }
        else
        {
            if (bytesInFile >= (buffersize - 1))
            {
                status = 3;
            }
            else
            {
                datasize = (int)fread(buffer, 1, bytesInFile, ifs);
                if (datasize != bytesInFile)
                {
                    status = 4;
                }
                else
                {   /* if we read a text file then this */
                    /* turns buffer into a "C" string   */
                    buffer[datasize] = '\0';
                }
            }
        }
    }
    if (ifs)
        fclose(ifs);
    return status;
}

int WriteBinaryFile(const char *filename, int datasize)
{
    int status = 0;
    FILE *ofs;
    ofs = fopen(filename, "wb");
    if (ofs)
    {
        fwrite(buffer, 1, datasize, ofs);
        fclose(ofs);
        status = 1;
    }
    return status;
}

int charIsText(char c)
{
    int status = 0;
    if (isprint(c) != 0)
    {
        status = 1;
    }
    else
    {
        switch (c)
        {
        case '\f':
        case '\t':
        case '\r':
        case '\n':
        case '\a':
        case '\b':
        case '\v':
            status = 1;
            break;
        }
    }
    return status;
}

int IsItText(unsigned char *buffer, int datasize)
{
    int i, count;
    count = datasize < 100 ? datasize : 100;
    for (i = 0; i < count; i++)
    {
        if (!charIsText(buffer[i]))
        {
            return 0;
        }
    }
    return 1;
}

/* Converts the Windows file to a Unix file*/
int ToUnix(unsigned char *buffer, int datasize, char *filename)
{
    int i, j;
    for (i = 0; i < datasize; i++)
    {
        if (buffer[i] == '\r')
        {
            for (j = i; j < datasize; j++)
            {
                buffer[j] = buffer[j + 1];
            }
            datasize--;
        }
    }

    WriteBinaryFile(filename, datasize);

    return 1;
}
