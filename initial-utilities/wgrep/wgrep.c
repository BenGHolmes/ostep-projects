#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xlocale.h>

FILE *fp;
char *line = NULL;
size_t linecap = 0;
ssize_t linelen;

void open(char *filename, FILE **fpp)
{
    *fpp = fopen(filename, "r");
    if (*fpp == NULL)
    {
        printf("wgrep: cannot open file\n");
        exit(1);
    }
}

void grep(char *search, FILE *fp)
{
    while ((linelen = getline(&line, &linecap, fp)) > 0)
    {
        if (strstr(line, search) != NULL)
        {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    case 2:
        grep(argv[1], stdin);
        return 0;
    default:
        for (int i = 2; i < argc; ++i)
        {
            open(argv[i], &fp);
            grep(argv[1], fp);
            fclose(fp);
        }
    }

    return 0;
}
