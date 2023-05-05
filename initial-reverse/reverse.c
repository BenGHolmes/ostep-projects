#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct Lines
{
    char **lines;
    size_t linecount;
} Lines;

void validate_args(int argc, char *argv[])
{
    // Check usage
    if (argc > 3)
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc == 3)
    {
        // Check input equals output, including hardlinks
        struct stat stat1, stat2;
        stat(argv[1], &stat1);
        stat(argv[2], &stat2);

        // If files point to same Inode, they are the same
        // file even if the paths differ.
        if (stat1.st_ino == stat2.st_ino)
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }
}

FILE *valid_fopen(char *filename, char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp == NULL)
    {
        fprintf(stderr, "reverse: cannot open file '%s'\n", filename);
        exit(1);
    }

    return fp;
}

Lines get_lines(FILE *in)
{
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    char **lines = NULL;
    size_t linecount = 0;

    while ((linelen = getline(&line, &linecap, in)) > 0)
    {
        lines = realloc(lines, sizeof(char *) * (linecount + 1));
        char *linep = malloc(sizeof(char) * (linelen + 1));

        if (lines == NULL || linep == NULL)
        {
            fprintf(stderr, "reverse: malloc failed");
            exit(1);
        }

        strlcpy(linep, line, linelen + 1);
        lines[linecount] = linep;
        linecount++;
    }

    return (Lines){lines, linecount};
}

void reverse(FILE *in, FILE *out)
{
    Lines lines = get_lines(in);
    for (int i = (lines.linecount - 1); i >= 0; i--)
    {
        fprintf(out, "%s", lines.lines[i]);
    }
}

int main(int argc, char *argv[])
{
    validate_args(argc, argv);

    FILE *in;
    FILE *out;

    switch (argc)
    {
    case 1:
        in = stdin;
        out = stdout;
        break;
    case 2:
        in = valid_fopen(argv[1], "r");
        out = stdout;
        break;
    case 3:
        in = valid_fopen(argv[1], "r");
        out = valid_fopen(argv[2], "w");
        break;
    }

    reverse(in, out);

    return 0;
}