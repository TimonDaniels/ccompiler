#include "characters.h"
#include <stdio.h>
#

void parseFile(FILE *file, struct CurChar *curChar)
{
    // declare variables
    char c;
    int count = 0;

    // read the file
    while (fread(&c, sizeof(char), 1, file))
    {
        count++;
        curChar->type = c;
        printf("Character: %c\n", curChar->type);
        // fflush(stdout);
    }
    printf("Total characters: %d\n", count);
}

// open the code-1.txt file
void main()
{
    // declare variables
    FILE *file;
    struct CurChar curChar;

    // open the file
    file = fopen("parser/code-1.txt", "r");
    if (file == NULL)
    {
        printf("Error: file not found\n");
        return;
    }

    // read the file
    printf("Reading the file...\n");
    parseFile(file, &curChar);

    // close the file
    fclose(file);
}