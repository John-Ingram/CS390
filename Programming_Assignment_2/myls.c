/* John Ingram
This program implements a simplified version of the ls command in ansi C
It has one possible argument '-a' which works the same way that the real ls -a works
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *ent;
    int i;
    int a = 0;
    char *path = ".";

    /* Check for the -a argument */
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-a") == 0)
        {
            a = 1;
        }
    }

    /* Check if there is a supplied Path */
    for ( i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-a") != 0)
        {
            path = argv[i];
        }
    }
    

    /* Open the directory */
    if ((dir = opendir(path)) != NULL)
    {
        /* Print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            if (a == 1)
            {
                printf("%s ", ent->d_name);
            }
            else if (ent->d_name[0] != '.')
            {
                printf("%s ", ent->d_name);
            }
        }
        printf("\n");
        closedir(dir);
    }
    else
    {
        /* Could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
}
