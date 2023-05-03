#include <stdio.h>

typedef struct State
{
    u_int32_t count;
    char c;
} State;

void reset_state(State *state, char c)
{
    state->c = c;
    state->count = 1;
}

void print_state(State *state)
{
    fwrite(&state->count, 4, 1, stdout);
    putc(state->c, stdout);
}

void parse_file(FILE *fp, State *state)
{
    if (state->c == EOF)
    {
        reset_state(state, getc(fp));
    }

    char c;
    while ((c = getc(fp)) != EOF)
    {
        if (c == state->c)
        {
            state->count++;
        }
        else
        {
            print_state(state);
            reset_state(state, c);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    State state = {0, EOF};
    FILE *fp;

    for (int i = 1; i < argc; ++i)
    {
        fp = fopen(argv[i], "r");
        parse_file(fp, &state);
        fclose(fp);
    }

    print_state(&state);
}
