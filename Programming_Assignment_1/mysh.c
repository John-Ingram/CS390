/* John Ingram
/* This program is a simple shell that can execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80   /* the max length of a command */
#define CHAR_END '\0' /* the end of a string */

/* The main loop that will run the shell */
int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* the command line arguments */
    char *raw = NULL;             /* the raw input from the user */
    char *prompt = "$ ";          /* the promp string */
    int i;                        /* loop counter */
    int argc;                     /* number of arguments */
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

        /* Copy the test into the raw string */
        raw = malloc((strlen(command) + 1) * sizeof(char));
        strcpy(raw, command);

        /* Parse the command into arguments */
        char *token = strtok(command, " ");
        i = 0;
        /* If the command is PS1 then set the prompt */

        while (token != NULL)
        {
            args[i] = token;
            strip(args[i]);
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;
        argc = i - 1; /* subtract 1 for the command */

        /* Check if the user wants to exit */
        if (strcmp(args[0], "exit") == 0)
        {
            /* Exit the shell */
            return 0;
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            echo(args, raw);
        }
        else if (strcmp(args[0], "PS1") == 0)
        {
            /* Change the prompt */
            prompt = malloc((strlen(raw) - 3) * sizeof(char));
            strcpy(prompt, raw + 4);
            strip(prompt);
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
        }
        else if (strcmp(args[0], "cp") == 0)
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
                printf("File could not be copied from %s to %s\n", args[1], args[2]);
                break;
            }
        }
        else if (strcmp(args[0], "rm") == 0)
        {
            /* Run the rm command for each file name*/
            for (i = 1; i <= argc; i++)
            {
                switch (rm(args[i]))
                {
                case 0:
                    /* File was removed successfully */
                    break;
                case 1:
                    /* File could not be removed because it does not exist */
                    printf("File %s does not exist\n", args[i]);
                    break;
                default:
                    /* File could not be removed for some other reason */
                    printf("File %s could not be removed\n", args[i]);
                    break;
                }
            }
        }
        else if (strcmp(args[0], "mkdir") == 0)
        {
            switch (makedir(args[1]))
            {
            case 0:
                /* Directory was created successfully */
                break;
            default:
                /* Directory could not be created*/
                printf("Directory %s could not be created\n", args[1]);
                break;
            }
        }
        else if (strcmp(args[0], "rmdir") == 0)
        {
            switch (removedir(args[1]))
            {
            case 0:
                /* Directory was removed successfully */
                break;

            default:
                /* Directory could not be removed */
                printf("Directory %s could not be removed\n", args[1]);
                break;
            }
        }
        else /*if no command was executed, display a little help text */
        {

            printf("command \"%s\" not found, try echo, PS1, cat , cp, rm, mkdir, or rmdir\n Type exit to exit\n", args[0]);
        }

        /* Free the memory in preperation for the next run */
        free(command);
        free(raw);
    }

    return 0;
}

/* Strip the last character from a string if it is a newline */
int strip(char *str)
{
    if (str[strlen(str) - 1] == '\n')
    {
        str[strlen(str) - 1] = CHAR_END;
    }

    return 0;
}

/* Implementation of the cp command */
int cp(char *src, char *dest)
{
    /* Open the source file */
    FILE *srcFile = fopen(src, "rb");
    if (srcFile == NULL)
    {
        /* Source file could not be opened */
        return 1;
    }

    /* Open the destination file */
    FILE *destFile = fopen(dest, "wb");
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
int echo(char *args[], char *raw)
{
    int commandLength = 5; /* length of the command "echo " */
    /* If the first argument is -n, do not print a newline */
    if (strcmp(args[1], "-n") == 0)
    {
        strip(raw);
        commandLength += 3; /* length of the argument "-n " */
    }

    /* Print the raw input minus the command */
    printf("%s", raw + commandLength);

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

/* Implementation of the mkdir command */
int makedir(char *dirname)
{
    if (mkdir(dirname, 0777) == 0)
    {
        /* Directory was created successfully */
        return 0;
    }
    else
    {
        /* Directory could not be created */
        return 1;
    }
}

/* Implementation of the rmdir command */
int removedir(char *dirname)
{
    if (rmdir(dirname) == 0)
    {
        /* Directory was removed successfully */
        return 0;
    }
    else
    {
        /* Directory could not be removed */
        return 1;
    }
}