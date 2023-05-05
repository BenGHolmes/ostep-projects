#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }

    FILE *fp;
    u_int32_t count;
    char c;
    for (int i = 1; i < argc; ++i)
    {
        fp = fopen(argv[i], "r");
        while (fread(&count, 4, 1, fp) > 0)
        {
            c = getc(fp);
            for (int j = 0; j < count; j++)
            {
                putc(c, stdout);
            }
        }
        fclose(fp);
    }

    return 0;
}