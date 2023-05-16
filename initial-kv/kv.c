#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define READ_DATABASE "database.txt"
#define WRITE_DATABASE "dbwrite.txt"

typedef struct Node Node;

struct Node
{
    char *key;
    char *val;
    Node *next;
};

Node *database = NULL;

// Insert a value into the database
void database_insert(Node **db, char *key, char *val)
{
    if (*db == NULL)
    {
        Node *new = malloc(sizeof(Node));
        new->key = key;
        new->val = val;
        new->next = NULL;

        *db = new;
        return;
    }

    Node *cur = *db;
    while (1)
    {
        if (strcmp(cur->key, key) == 0)
        {
            cur->val = val;
            return;
        }

        if (cur->next == NULL)
        {
            Node *new = malloc(sizeof(Node));
            new->key = key;
            new->val = val;
            new->next = NULL;
            cur->next = new;
            return;
        }

        cur = cur->next;
    }
}

// Print all values in the database
void database_print_all(Node *db)
{
    while (db != NULL)
    {
        printf("%s,%s\n", db->key, db->val);
        db = db->next;
    }
}

// Get a the value of the given key in the database
char *database_get(Node *db, char *key)
{
    while (db != NULL)
    {
        if (strcmp(db->key, key) == 0)
            return db->val;

        db = db->next;
    }

    return NULL;
}

// Delete the item with the given key in the database
void database_delete(Node **db, char *key)
{
    // No entries
    if (*db == NULL)
        return;

    // Deleting first entry
    if (strcmp((*db)->key, key) == 0)
    {
        // Point to the next and free old head
        Node *to_free = *db;
        *db = (*db)->next;
        free(to_free);
        return;
    }

    // Deleting another entry
    Node *cur = *db;
    while (cur->next != NULL)
    {
        if (strcmp(cur->next->key, key) == 0)
        {
            Node *to_free = cur->next;
            cur->next = cur->next->next;
            free(to_free);
            return;
        }

        cur = cur->next;
    }
}

// Delete all elements in the database
void database_clear(Node **db)
{
    while (*db != NULL)
    {
        Node *to_free = *db;
        *db = (*db)->next;
        free(to_free);
    }
}

// Read the database file into memory
void read_database(Node **db)
{
    database_clear(db);
    FILE *fp = fopen(READ_DATABASE, "r");
    if (fp == NULL)
    {
        return;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) > 0)
    {
        // Make a copy of the line
        char *copy = malloc(linelen * sizeof(char));
        strncpy(copy, line, linelen);

        // Create node for the new item, with current db as next element
        Node *new = malloc(sizeof(Node));
        new->key = strsep(&copy, ",");
        new->val = strsep(&copy, "\n");
        new->next = *db;

        // Update the db to point to the new element
        *db = new;
    }

    fclose(fp);
}

// Save the current database to the database file.
void save_database(Node *db)
{
    FILE *fp = fopen(WRITE_DATABASE, "w");

    while (db != NULL)
    {
        fprintf(fp, "%s,%s\n", db->key, db->val);
        db = db->next;
    }

    // Close file and replace read database with most recent write database.
    fclose(fp);
    remove(READ_DATABASE);
    rename(WRITE_DATABASE, READ_DATABASE);
}

void handle_put(char *request)
{
    char *key = strsep(&request, ",");
    char *val = strsep(&request, "\0");
    database_insert(&database, key, val);
}

void handle_get(char *request)
{
    char *key = strsep(&request, "\0");
    char *val = database_get(database, key);
    if (val == NULL)
        printf("%s not found\n", key);
    else
        printf("%s,%s\n", key, val);
}

void handle_delete(char *request)
{
    char *key = strsep(&request, "\0");
    database_delete(&database, key);
}

void handle_all()
{
    database_print_all(database);
}

void handle_clear()
{
    database_clear(&database);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;
    }

    read_database(&database);

    char *request, *command;
    for (int i = 1; i < argc; ++i)
    {
        request = argv[i];
        command = strsep(&request, ",");
        switch (command[0])
        {
        case 'p':
            handle_put(request);
            break;
        case 'g':
            handle_get(request);
            break;
        case 'd':
            handle_delete(request);
            break;
        case 'c':
            handle_clear();
            break;
        case 'a':
            handle_all();
            break;
        default:
            printf("bad command\n");
            break;
        }
    }

    save_database(database);
    database_clear(&database);
}