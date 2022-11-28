/* John Ingram
This program is a simple shell that can execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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
            if (args[1] == NULL || (strcmp(args[1], "-n") == 0 && args[2] == NULL))
            {
                printf("echo: missing operand\n");
            }
            else
            {
                echo(args, raw);
            }
        }
        else if (strcmp(args[0], "PS1") == 0)
        {
            /* If there is no argument then throw an error */
            if (args[1] == NULL)
            {
                printf("PS1: missing operand\n");
            }
            else
            {
                /* Change the prompt */
                prompt = malloc((strlen(raw) - 3) * sizeof(char));
                strcpy(prompt, raw + 4);
                strip(prompt);
            }
        }
        else if (strcmp(args[0], "cat") == 0)
        {
            /*If there is no file name, print an error message*/
            if (args[1] == NULL)
            {
                printf("cat: missing operand\n");
            }
            else
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
        }
        else if (strcmp(args[0], "cp") == 0)
        {
            if (args[1] == NULL)
            {
                printf("cp: missing operand\n");
            }
            else
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
        }
        else if (strcmp(args[0], "rm") == 0)
        {
            /*If there is no file name, print an error message*/
            if (args[1] == NULL)
            {
                printf("rm: missing operand\n");
            }
            else
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
        }
        else if (strcmp(args[0], "mkdir") == 0)
        {
            /*If there is no file name, print an error message*/
            if (args[1] == NULL)
            {
                printf("mkdir: missing operand\n");
            }
            else
            {
                /* Run the mkdir command for each file name*/
                for (i = 1; i <= argc; i++)
                {
                    switch (mkdir(args[i]))
                    {
                    case 0:
                        /* Directory was created successfully */
                        break;
                    case 1:
                        /* Directory could not be created because it already exists */
                        printf("Directory %s already exists\n", args[i]);
                        break;
                    default:
                        /* Directory could not be created for some other reason */
                        printf("Directory %s could not be created\n", args[i]);
                        break;
                    }
                }
            }
        }
        else if (strcmp(args[0], "rmdir") == 0)
        {
            /*If there is no file name, print an error message*/
            if (args[1] == NULL)
            {
                printf("rmdir: missing operand\n");
            }
            else
            {
                /* Run the rmdir command for each file name*/
                for (i = 1; i <= argc; i++)
                {
                    switch (rmdir(args[i]))
                    {
                    case 0:
                        /* Directory was removed successfully */
                        break;
                    default:
                        /* Directory could not be removed for some other reason */
                        printf("Directory %s could not be removed\n", args[i]);
                        break;
                    }
                }
            }
        }
        else /*Run command externally. At this point raw is no longer needed, so we can use it as a place to store our path to the command/file */
        {
            /* Check if anything was entered at all*/
            if (argc >= 0)
            {

                /* If the input is a command, raw will hold the path to that command. If not, make raw contain args[0]*/
                /* If the input is a file path, or is not a command, put args[0] in raw*/
                if (isFile(args[0]) == 1 || isCommand(args[0], raw) == 0)
                {
                    free(raw);
                    raw = malloc((strlen(args[0]) + 1) * sizeof(char));
                    strcpy(raw, args[0]);
                }

                /*Run the entered path with execv*/
                switch (fork())
                {
                case -1:
                    /* Error */
                    printf("Error forking\n");
                    break;
                case 0:
                    /* Child process */
                    if (execv(raw, args) == -1)
                    {
                        perror("");
                    }
                    break;
                default:
                    /* Parent process */
                    wait(NULL);
                    break;
                }
            }
        }

        /* Free the memory in preperation for the next run */
        free(command);
        free(raw);
    }

    return 0;
}

/* Checks to see if the string is a path to a file */
int isFile(char *path)
{
    /* If the path is a directory, return 0 */
    if (path[strlen(path) - 1] == '/')
    {
        return 0;
    }
    /* If the path is a file, return 1 */
    else if (strchr(path, '/') != NULL && access(path, F_OK) != -1)
    {
        return 1;
    }
    /* If the path is neither a file or directory, return 0 */
    else
    {
        return 0;
    }
}

/* Checks if the string is a command on the PATH, if it is put the path to that program in newPath */
int isCommand(char *command, char *newPath)
{
    char *realPath = getenv("PATH");
    char *token;
    char *pathCommand;
    int i;

    /* Copy path as to not break the real one*/
    char *path = malloc((strlen(realPath) + 1) * sizeof(char));
    strcpy(path, realPath);

    /* Get the first token */
    token = strtok(path, ":");

    /* Walk through other tokens */
    while (token != NULL)
    {
        /* Create the path to the command */
        pathCommand = malloc((strlen(token) + strlen(command) + 2) * sizeof(char));
        strcpy(pathCommand, token);
        strcat(pathCommand, "/");
        strcat(pathCommand, command);

        /* Check if the file exists */
        if (access(pathCommand, F_OK) != -1)
        {
            /* Free newpath and replace it with the path */
            free(newPath);
            newPath = malloc((strlen(pathCommand) + 1) * sizeof(char));
            strcpy(newPath, pathCommand);

            /* File exists */
            free(pathCommand);
            free(path);
            return 1;
        }

        /* Get the next token */
        token = strtok(NULL, ":");
    }

    /* File does not exist */
    free(pathCommand);
    free(path);
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
    /* If there is no first argument, print an error */
    if (args[1] == NULL)
    {
        printf("echo: missing operand\n");
        return 1;
    }

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