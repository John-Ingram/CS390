/* John Ingram
/* This program is a simple shell that can execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* the max length of a command */
#define CHAR_END '\0' /* the end of a string */

/* The main loop that will run the shell */
int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* the command line arguments */
    char *prompt = "$ ";           /* the promp string */
    int i;                        /* loop counter */
    int status;                   /* status of the child process */
    pid_t pid;                    /* process id of the child process */

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);
        fflush(stdin);

        /* Read in the command from the user */
        char *command = NULL;
        size_t size = 0;
        getline(&command, &size, stdin);

        /* Parse the command into arguments */
        char *token = strtok(command, " ");
        i = 0;
        while (token != NULL)
        {
            args[i] = token;
            strip(args[i]);
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;
        

        /* Check if the user wants to exit */
        if (strcmp(args[0], "exit") == 0)
        {
            /* Exit the shell */
            return 0;
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            echo(args);
        }
        else if (strcmp(args[0], "PS1") == 0)
        {
            /* Change the prompt */
            prompt = args[1];
        } 
        else if (strcmp(args[0], "cat") == 0)
        {
            switch (cat(args[1]))
            {
            case 0:
                /* File was opened and read successfully */
                break;
            case 1:
                /* File could not be opened because it does not exist */
                printf("File %s does not exist\n", args[1]);
            default:
                /* File could not be opened for some other reason */
                printf("File %s could not be opened\n", args[1]);
                break;
            }
        } else if (strcmp(args[0], "cp") ==0)
        {
            switch (cp(args[1], args[2]))
            {
            case 0:
                /* File was copied successfully */
                break;
            case 1:
                /* Source file error */
                printf("Source file %s could not be opened\n", args[1]);
                break;
            case 2:
                /* Destination file error */
                printf("Destination file %s could not be opened\n", args[2]);
                break;
            default:
                /* Some other error, something has gone very wrong */
                printf("File could not be copied\n");
                break;
            }
        } else if (strcmp(args[0], "rm") == 0)
        {
            /* code */
        }
        
        
        /* Debugging code, prints the arguments if no other was executed */
        else
        {
            for (i = 0; args[i] != NULL; i++)
            {
                printf("%s ", args[i]);
            }
            /* print the number of elements in args for debugging */
            printf("%d\n", i);
        }
    }

    return 0;
}

/* Strip the last character from a string if it is a newline */
int strip(char *str)
{
    if (str[strlen(str) - 1] == '\n')
    {
        str[strlen(str) - 1] = '\0';
    }

    return 0;
}

/* Implementation of the cp command */
int cp(char *src, char *dest)
{
    /* Open the source file */
    FILE *srcFile = fopen(src, "r");
    if (srcFile == NULL)
    {
        /* Source file could not be opened */
        return 1;
    }

    /* Open the destination file */
    FILE *destFile = fopen(dest, "w");
    if (destFile == NULL)
    {
        /* Destination file could not be opened TODO: Ask about this. How can I get errno?*/
        return 2;
    }

    /* Copy the contents of the source file to the destination file */
    char c;
    while ((c = fgetc(srcFile)) != EOF)
    {
        fputc(c, destFile);
    }

    /* Close the files */
    fclose(srcFile);
    fclose(destFile);
    printf("%s", "\n\r");

    return 0;
}

/* Implementation of the echo command */
int echo(char *args[])
{
    int i = 1;
    char *end_string = "\n\r";
    /* If the first argument is -n, do not print a newline */
    if (strcmp(args[1], "-n") == 0)
    {
        end_string = "";
        i++;
    }
    
    /* Print the arguments */
    for (; args[i] != NULL && strcmp(args[i], "-n") != 0; i++)
    {
        printf("%s ", args[i]);
    }
    /* go to a new line and return to column 1 */
    printf("%s", end_string);
    return 0;
}

/* Implementation of the cat command */
int cat(char *filename)
{
    /* Open the file */
    FILE *file = fopen(filename, "r");
    /* Check if the file exists */
    if (file == NULL)
    {
        /* File does not exist */
        return 1;
    }
    else
    {
        char c;
        /* Read in the file character by character */
        while ((c = fgetc(file)) != EOF)
        {
            /* Print the character */
            printf("%c", c);
        }
    }
    /* Close the file and go to col 1 on a new line */
    fclose(file);
    printf("%s", "\n\r");
    return 0;
}

/* Implementation of the rm command */
int rm(char *filename)
{
    if (remove(filename) == 0)
    {
        /* File was deleted successfully */
        return 0;
    }
    else
    {
        /* File could not be deleted */
        return 1;
    }
}