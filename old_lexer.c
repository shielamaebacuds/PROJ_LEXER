#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void checkFileExistence(char *filename);                                // check if the file exists
void driverFunction(FILE *f, FILE *symbolTable);                        // this is where we will loop until EOF
char getNextCharacter(FILE *f);                                         // gets the next character from the file
char create_NDI_Lexeme(FILE *f, FILE *symbolTable, char firstChar);     // num,decimal ,invalid
char create_NKRI_Lexeme(FILE *f, FILE *symbolTable, char firstChar);    // noise,keyword,reserve,invalid
char create_COMMENT_Lexeme(FILE *f, FILE *symbolTable, char firstChar); // single, multi
char create_WORD_Lexeme(FILE *f, FILE *symbolTable);                    // string or word
char create_CHARACTER_Lexeme(FILE *f, FILE *symbolTable);               // char or letter
bool isOperator(FILE *f, FILE *symbolTable, char *c);                   // for operators
bool isDelimiterBracket(FILE *f, FILE *symbolTable, char *c);           // for delimiters and brackets

int main()
{
    FILE *sourceCode;
    char filename[30];

    printf("Enter file directory: ");
    scanf("%[^\n]", filename);

    checkFileExistence(filename);
    sourceCode = fopen(filename, "r");

    FILE *symbolTable = fopen("symbol_table.txt", "w");
    if (symbolTable == NULL)
    {
        printf("Error opening symbol table");
        return 1;
    }

    driverFunction(sourceCode, symbolTable);
    printf("Symbol Table was successfully generated!\n");

    fclose(symbolTable);
    fclose(sourceCode);
}

void checkFileExistence(char *filename)
{
    FILE *fp;
    // allows user to input file directory

    // checks if file extension is .kidz
    int length = strlen(filename);
    if (length >= 5)
    {
        if (filename[length - 1] == 'z' && filename[length - 2] == 'd' && filename[length - 3] == 'i' && filename[length - 4] == 'k' && filename[length - 5] == '.')
        {
            fp = fopen(filename, "r+");
            if (fp == NULL)
            {
                printf("File [%s] does not exist\n", filename);
                exit(1);
            }
            else
            {
                fclose(fp);
            }
        }

        else
        {
            printf("Invalid File Type [%s]\n", filename);
            exit(1);
        }
    }
    else
    {
        printf("Invalid input [%s]\n", filename);
        exit(1);
    }
}

void driverFunction(FILE *f, FILE *symbolTable)
{
    char currentCharacter = fgetc(f);
    while (currentCharacter != EOF)
    {
        if (currentCharacter == ' ')
        {
            currentCharacter = getNextCharacter(f);
        }

        else if (currentCharacter == '\n')
        {
            currentCharacter = getNextCharacter(f);
            if (currentCharacter != '\n')
            {
                fprintf(symbolTable, "NEWLINE~NEWLINE\n");
            }
        }

        else if (isdigit(currentCharacter))
        {
            // marereturn dito if hindi na number yung maread
            currentCharacter = create_NDI_Lexeme(f, symbolTable, currentCharacter);
        }
        else if (isalpha(currentCharacter))
        {
            currentCharacter = create_NKRI_Lexeme(f, symbolTable, currentCharacter);
        }
        else if (currentCharacter == '#')
        {
            currentCharacter = create_COMMENT_Lexeme(f, symbolTable, currentCharacter);
        }
        else if (currentCharacter == '"')
        {
            currentCharacter = create_WORD_Lexeme(f, symbolTable);
        }
        else if (currentCharacter == '\'')
        {
            currentCharacter = create_CHARACTER_Lexeme(f, symbolTable);
        }
        else if (isOperator(f, symbolTable, &currentCharacter) == true)
        {
            continue;
        }
        else if (isDelimiterBracket(f, symbolTable, &currentCharacter) == true)
        {
            continue;
        }
        else
        {
            fprintf(symbolTable, "INVALID~%c\n", currentCharacter);
            currentCharacter = getNextCharacter(f);
        }
    }
    
    if (currentCharacter == EOF)
    {
        fprintf(symbolTable, "EOF~EOF");
    }
}

char getNextCharacter(FILE *f)
{
    return (fgetc(f));
}

char create_NDI_Lexeme(FILE *f, FILE *symbolTable, char firstChar)
{
    char cur_char;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = firstChar;
    temp[1] = '\0';

    strcat(lexeme, temp);

    // identify num, decimal, and invalid numbers
    int flag = 0; // 0 if (1) num; (2) decimal; (3) invalid

    cur_char = getNextCharacter(f);
    while (isdigit(cur_char) && cur_char != EOF)
    {
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);
        cur_char = getNextCharacter(f);
    }

    if (cur_char == '.')
    {
        flag = 1;
        while ((cur_char == '.' || isalnum(cur_char)) && cur_char != EOF)
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (temp[0] == '.' && (cur_char == EOF || cur_char == '\n')) // ex: 12. <--wala ng number after decimal point
            {
                flag = 2;
            }

            if (cur_char == '.' || isalpha(cur_char))
            {
                flag = 2;
            }
        }
    }

    else if (isalpha(cur_char))
    {
        flag = 2;
        while ((cur_char == '.' || isalnum(cur_char)) && cur_char != EOF)
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);
        }
    }

    if (flag == 0)
        fprintf(symbolTable, "CONSTNUMBER~%s\n", lexeme);
    else if (flag == 1)
        fprintf(symbolTable, "CONSTDECIMAL~%s\n", lexeme);
    else if (flag == 2)
        fprintf(symbolTable, "INVALID~%s\n", lexeme);

    return cur_char;
}

char create_NKRI_Lexeme(FILE *f, FILE *symbolTable, char firstChar)
{
    char cur_char;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = firstChar;
    temp[1] = '\0';

    strcat(lexeme, temp);

    if (lexeme[0] == 'n') // num, not
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'u') // num
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'm')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "num~%s\n", lexeme);
                    return cur_char;
                }
            }
        }
        else if (cur_char == 'o') // not
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 't') // not
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "not~%s\n", lexeme);
                    return cur_char;
                }
            }
        }
    }
    else if (lexeme[0] == 'b') // boolean
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'o')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'o')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'l')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'n')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "boolean~%s\n", lexeme);
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'a') // a ,an ,and
    {
        cur_char = getNextCharacter(f);
        if (!isalnum(cur_char) && cur_char != '_')
        {
            fprintf(symbolTable, "a~%s\n", lexeme);
            return cur_char;
        }

        else if (cur_char == 'n')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "an~%s\n", lexeme);
                return cur_char;
            }

            else if (cur_char == 'd')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "and~%s\n", lexeme);
                    return cur_char;
                }
            }
        }
    }
    else if (lexeme[0] == 'o') // or
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'r')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "or~%s\n", lexeme);
                return cur_char;
            }
        }
    }
    else if (lexeme[0] == 'm') // match
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'a')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 't')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'c')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'h')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "match~%s\n", lexeme);
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'c') // case, continue, character
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'a') // case
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "case~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'o') // continue
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'n')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 't')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'i')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'n')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'u')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (cur_char == 'e')
                                {
                                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                    temp[0] = cur_char;
                                    strcat(lexeme, temp);
                                    cur_char = getNextCharacter(f);

                                    if (!isalnum(cur_char) && cur_char != '_')
                                    {
                                        fprintf(symbolTable, "continue~%s\n", lexeme);
                                        return cur_char;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (cur_char == 'h') // character
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'a')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'r')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'a')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'c')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 't')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (cur_char == 'e')
                                {
                                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                    temp[0] = cur_char;
                                    strcat(lexeme, temp);
                                    cur_char = getNextCharacter(f);

                                    if (cur_char == 'r')
                                    {
                                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                        temp[0] = cur_char;
                                        strcat(lexeme, temp);
                                        cur_char = getNextCharacter(f);

                                        if (!isalnum(cur_char) && cur_char != '_')
                                        {
                                            fprintf(symbolTable, "character~%s\n", lexeme);
                                            return cur_char;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'r') // return
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'e')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 't')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'u')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'r')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'n')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (!isalnum(cur_char) && cur_char != '_')
                            {
                                fprintf(symbolTable, "return~%s\n", lexeme);
                                return cur_char;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'l') // list
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'i') // list
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 't')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "list~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'w') // word,while
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // word
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'r')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'd')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "word~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'h') // while
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'i')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'l')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "while~%s\n", lexeme);
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'd') // do, decimal, display
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // do
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "do~%s\n", lexeme);
                return cur_char;
            }
        }
        else if (cur_char == 'e') // decimal
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'c')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'i')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'm')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'l')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "decimal~%s\n", lexeme);
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (cur_char == 'i') // display
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'p')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'l')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'y')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "display~%s\n", lexeme);
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'f') // foreach, false
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // foreach
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'r')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'a')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'c')
                        {
                            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                            temp[0] = cur_char;
                            strcat(lexeme, temp);
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'h')
                            {
                                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                                temp[0] = cur_char;
                                strcat(lexeme, temp);
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "foreach~%s\n", lexeme);
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (cur_char == 'a') // false
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'l')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 's')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "false~%s\n", lexeme);
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 'e') // else, end
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'l') // else
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "else~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'n') // end
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'd')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "end~%s\n", lexeme);
                    return cur_char;
                }
            }
        }
    }
    else if (lexeme[0] == 'i') // in, input, if
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'f')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "if~%s\n", lexeme);
                return cur_char;
            }
        }
        else if (cur_char == 'n') // in, input
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "in~%s\n", lexeme);
                return cur_char;
            }

            else if (cur_char == 'p')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'u')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (cur_char == 't')
                    {
                        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                        temp[0] = cur_char;
                        strcat(lexeme, temp);
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "input~%s\n", lexeme);
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    else if (lexeme[0] == 't') // true, the, then
    {
        cur_char = getNextCharacter(f);
        if (cur_char == 'r') // true
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'u')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "true~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'h') // the, then
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);

            if (cur_char == 'e')
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                temp[0] = cur_char;
                strcat(lexeme, temp);
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "the~%s\n", lexeme);
                    return cur_char;
                }

                else if (cur_char == 'n')
                {
                    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                    temp[0] = cur_char;
                    strcat(lexeme, temp);
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "then~%s\n", lexeme);
                        return cur_char;
                    }
                }
            }
        }
    }
    else
    {
        cur_char = getNextCharacter(f);
    }

    while (isalnum(cur_char) || cur_char == '_')
    {
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);
        cur_char = getNextCharacter(f);
    }

    if (strlen(lexeme) > 31)
        fprintf(symbolTable, "INVALID~%s\n", lexeme);
    else
        fprintf(symbolTable, "IDENTIFIER~%s\n", lexeme);
    return cur_char;
}

char create_COMMENT_Lexeme(FILE *f, FILE *symbolTable, char firstChar)
{
    char cur_char;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = firstChar;
    temp[1] = '\0';

    strcat(lexeme, temp);

    cur_char = getNextCharacter(f);
    // this is for single-line comments
    if (cur_char != '#')
    {
        while (cur_char != '\n' && cur_char != EOF)
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);
            cur_char = getNextCharacter(f);
        }

        fprintf(symbolTable, "SLCOMMENT~%s\n", lexeme);
        return cur_char;
    }

    else
    {
        int found = 0; // 0 if ## is not yet found
        char nextchar[2];
        nextchar[1] = '\0';

        // adds the second # to the lexeme
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);

        char presentChar = getNextCharacter(f);
        while (nextchar[0] != EOF)
        {
            cur_char = presentChar;
            temp[0] = cur_char;
            nextchar[0] = getNextCharacter(f);

            if (cur_char == '#' && nextchar[0] == '#')
            {
                found = 1;
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp) + strlen(nextchar)) * sizeof(char) + 1);
                strcat(lexeme, temp);
                strcat(lexeme, nextchar);

                cur_char = nextchar[0];
                break;
            }
            else
            {
                lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
                strcat(lexeme, temp);
                presentChar = nextchar[0];
            }
        }

        if (found == 1)
            fprintf(symbolTable, "MLCOMMENT~%s\n", lexeme);
        else
            fprintf(symbolTable, "INVALID~%s\n", lexeme);
    }

    if (cur_char != EOF)
    {
        cur_char = getNextCharacter(f);
        return cur_char;
    }
}

char create_WORD_Lexeme(FILE *f, FILE *symbolTable)
{
    char cur_char;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = '"';
    temp[1] = '\0';

    cur_char = getNextCharacter(f);
    while (cur_char != '"' && cur_char != '\n' && cur_char != EOF)
    {
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
    }

    if (strlen(lexeme) == 0)
    {
        fprintf(symbolTable, "INVALID~%c\n", temp[0]);
        return cur_char;
    }
    else if (cur_char != '"')
    {
        fprintf(symbolTable, "INVALID~\"%s\n", lexeme);
        return cur_char;
    }
    else
    {
        fprintf(symbolTable, "CONSTWORD~%s\n", lexeme);
        cur_char = getNextCharacter(f);
        return cur_char;
    }
}

char create_CHARACTER_Lexeme(FILE *f, FILE *symbolTable)
{
    char cur_char;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = '\'';
    temp[1] = '\0';

    cur_char = getNextCharacter(f);

    while (cur_char != '\'' && cur_char != '\n' && cur_char != EOF)
    {
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
    }

    if (strlen(lexeme) == 0)
    {
        fprintf(symbolTable, "INVALID~%c\n", temp[0]);
        return cur_char;
    }

    else if (cur_char != '\'')
    {
        fprintf(symbolTable, "INVALID~'%s\n", lexeme);
        return cur_char;
    }
    else if (strlen(lexeme) > 1)
    {
        fprintf(symbolTable, "INVALID~%s\n", lexeme);
        cur_char = getNextCharacter(f);
        return cur_char;
    }
    else
    {
        fprintf(symbolTable, "CONSTCHARACTER~%s\n", lexeme);
        cur_char = getNextCharacter(f);
        return cur_char;
    }
}

bool isOperator(FILE *f, FILE *symbolTable, char *c)
{
    char cur_char = *c;
    switch (cur_char)
    {
    case '=':;
        char *lexeme = NULL;

        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '\0';

        char temp[2];
        temp[0] = cur_char;
        temp[1] = '\0';

        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {

            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);

            fprintf(symbolTable, "==~%s\n", lexeme);

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "=~%s\n", lexeme);
            *c = cur_char;
        }

        return true;

    case '>':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '\0';

        temp[0] = cur_char;
        temp[1] = '\0';

        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);

            fprintf(symbolTable, ">=~%s\n", lexeme);

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, ">~%s\n", lexeme);
            *c = cur_char;
        }
        return true;

    case '<':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '\0';

        temp[0] = cur_char;
        temp[1] = '\0';

        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);

            fprintf(symbolTable, "<=~%s\n", lexeme);

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "<~%s\n", lexeme);
            *c = cur_char;
        }

        return true;

    case '!':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '\0';

        temp[0] = cur_char;
        temp[1] = '\0';

        strcat(lexeme, temp);

        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
            temp[0] = cur_char;
            strcat(lexeme, temp);

            fprintf(symbolTable, "!=~%s\n", lexeme);

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "!~%s\n", lexeme);
            *c = cur_char;
        }

        return true;

    case '+':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '+';
        lexeme[1] = '\0';

        fprintf(symbolTable, "+~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '-':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '-';
        lexeme[1] = '\0';

        fprintf(symbolTable, "-~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '/':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '/';
        lexeme[1] = '\0';

        fprintf(symbolTable, "/~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '*':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '*';
        lexeme[1] = '\0';

        fprintf(symbolTable, "*~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '%':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '%';
        lexeme[1] = '\0';

        fprintf(symbolTable, "%%~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '^':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '^';
        lexeme[1] = '\0';

        fprintf(symbolTable, "^~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '@':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '@';
        lexeme[1] = '\0';

        fprintf(symbolTable, "@~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;
    }
    return false;
}

bool isDelimiterBracket(FILE *f, FILE *symbolTable, char *c)
{

    switch (*c)
    {
    case '(':;
        char *lexeme = NULL;
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '(';
        lexeme[1] = '\0';

        fprintf(symbolTable, "(~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case ')':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = ')';
        lexeme[1] = '\0';

        fprintf(symbolTable, ")~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '[':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '[';
        lexeme[1] = '\0';

        fprintf(symbolTable, "[~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case ']':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = ']';
        lexeme[1] = '\0';

        fprintf(symbolTable, "]~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '{':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '{';
        lexeme[1] = '\0';

        fprintf(symbolTable, "{~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case '}':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = '}';
        lexeme[1] = '\0';
        fprintf(symbolTable, "}~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case ';':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = ';';
        lexeme[1] = '\0';

        fprintf(symbolTable, ";~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case ':':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = ':';
        lexeme[1] = '\0';

        fprintf(symbolTable, ":~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;

    case ',':
        lexeme = (char *)malloc(sizeof(char) + 1);
        lexeme[0] = ',';
        lexeme[1] = '\0';

        fprintf(symbolTable, ",~%s\n", lexeme);

        *c = getNextCharacter(f);
        return true;
    }

    return false;
}

// algo:
// check if file exists. open the file
// read the first character of the file; determine if character, digit, etc.
// if letter, punta sa function for keywords ,noise, reserve
// determine the next character by calling the function getNextCharacter()
// check if lettere pa rin yung nireturn na character, if yes, getNextCharacter() pa rin , if not, print lexeme then return mo yung character sa driver para makagawa ng new token
