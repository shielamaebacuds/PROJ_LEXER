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
            fprintf(symbolTable, "%c~INVALID\n", currentCharacter);
            currentCharacter = getNextCharacter(f);
        }
    }
}

char getNextCharacter(FILE *f)
{
    return (fgetc(f));
}

char create_NDI_Lexeme(FILE *f, FILE *symbolTable, char firstChar) // converted to print directly
{
    char cur_char;
    int counter = 0;
    cur_char = firstChar;

    char temp[2];
    temp[0] = '\0';
    temp[1] = '\0';

    // identify num, decimal, and invalid numbers
    int flag = 0; // 0 if (1) num; (2) decimal; (3) invalid

    fprintf(symbolTable, "%c", cur_char);
    cur_char = getNextCharacter(f);
    while (isdigit(cur_char) && cur_char != EOF)
    {
        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
    }

    if (cur_char == '.')
    {
        flag = 1;
        while ((cur_char == '.' || isalnum(cur_char)) && cur_char != EOF)
        {
            fprintf(symbolTable, "%c", cur_char);
            temp[0] = cur_char;
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
            fprintf(symbolTable, "%c", cur_char);
            cur_char = getNextCharacter(f);
        }
    }

    if (flag == 0)
        fprintf(symbolTable, "~CONSTNUMBER\n");
    else if (flag == 1)
        fprintf(symbolTable, "~CONSTDECIMAL\n");
    else if (flag == 2)
        fprintf(symbolTable, "~INVALID\n");

    return cur_char;
}

char create_NKRI_Lexeme(FILE *f, FILE *symbolTable, char firstChar) // converted to print directly
{
    char cur_char;
    int counter = 0;
    cur_char = firstChar;

    if (cur_char == 'n') // num, not
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);

        if (cur_char == 'u') // num
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'm')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "~num\n");
                    return cur_char;
                }
            }
        }
        else if (cur_char == 'o') // not
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 't') // not
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "~num\n");
                    return cur_char;
                }
            }
        }
    }
    // boolean
    else if (cur_char == 'b')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'o')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'o')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'l')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'n')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "~boolean\n");
                                    counter++;
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // a ,an ,and
    else if (cur_char == 'a')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (!isalnum(cur_char) && cur_char != '_')
        {
            fprintf(symbolTable, " = a\n");
            return cur_char;
        }

        else if (cur_char == 'n')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "~an\n");
                return cur_char;
            }

            else if (cur_char == 'd')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "~and\n");
                    return cur_char;
                }
            }
        }
    }
    // or
    else if (cur_char == 'o')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'r')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "~or\n");
                return cur_char;
            }
        }
    }
    // match
    else if (cur_char == 'm')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'a')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 't')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'c')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'h')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "~match\n");
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    // case, continue, character
    else if (cur_char == 'c')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'a') // case
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~case\n");
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'o') // continue
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'n')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 't')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'i')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'n')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'u')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (cur_char == 'e')
                                {
                                    fprintf(symbolTable, "%c", cur_char);
                                    counter++;
                                    cur_char = getNextCharacter(f);

                                    if (!isalnum(cur_char) && cur_char != '_')
                                    {
                                        fprintf(symbolTable, "~continue\n");
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
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'a')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'r')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'a')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'c')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 't')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (cur_char == 'e')
                                {
                                    fprintf(symbolTable, "%c", cur_char);
                                    counter++;
                                    cur_char = getNextCharacter(f);

                                    if (cur_char == 'r')
                                    {
                                        fprintf(symbolTable, "%c", cur_char);
                                        counter++;
                                        cur_char = getNextCharacter(f);

                                        if (!isalnum(cur_char) && cur_char != '_')
                                        {
                                            fprintf(symbolTable, "~character\n");
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
    // return
    else if (cur_char == 'r')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'e')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 't')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'u')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'r')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'n')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (!isalnum(cur_char) && cur_char != '_')
                            {
                                fprintf(symbolTable, "~return\n");
                                return cur_char;
                            }
                        }
                    }
                }
            }
        }
    }
    // list
    else if (cur_char == 'l')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'i') // list
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 't')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~list\n");
                        return cur_char;
                    }
                }
            }
        }
    }
    // word,while
    else if (cur_char == 'w')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // word
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'r')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'd')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~word\n");
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'h') // while
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'i')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'l')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "~while\n");
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    // do, decimal, display
    else if (cur_char == 'd')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // do
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "~do\n");
                return cur_char;
            }
        }
        else if (cur_char == 'e') // decimal
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'c')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'i')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'm')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'l')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "~decimal\n");
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
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'p')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'l')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'a')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'y')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "~display\n");
                                    return cur_char;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // foreach, false
    else if (cur_char == 'f')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'o') // foreach
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'r')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'a')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (cur_char == 'c')
                        {
                            fprintf(symbolTable, "%c", cur_char);
                            counter++;
                            cur_char = getNextCharacter(f);

                            if (cur_char == 'h')
                            {
                                fprintf(symbolTable, "%c", cur_char);
                                counter++;
                                cur_char = getNextCharacter(f);

                                if (!isalnum(cur_char) && cur_char != '_')
                                {
                                    fprintf(symbolTable, "~foreach\n");
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
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'l')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 's')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 'e')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "~false\n");
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    // else, end
    else if (cur_char == 'e')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'l') // else
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 's')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~else\n");
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'n') // end
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'd')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "~end\n");
                    return cur_char;
                }
            }
        }
    }
    // in, input, if
    else if (cur_char == 'i')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'f')
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "~if\n");
                return cur_char;
            }
        }
        else if (cur_char == 'n') // in, input
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (!isalnum(cur_char) && cur_char != '_')
            {
                fprintf(symbolTable, "~in\n");
                return cur_char;
            }

            else if (cur_char == 'p')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'u')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (cur_char == 't')
                    {
                        fprintf(symbolTable, "%c", cur_char);
                        counter++;
                        cur_char = getNextCharacter(f);

                        if (!isalnum(cur_char) && cur_char != '_')
                        {
                            fprintf(symbolTable, "~input\n");
                            return cur_char;
                        }
                    }
                }
            }
        }
    }
    // true, the, then
    else if (cur_char == 't')
    {
        fprintf(symbolTable, "%c", cur_char);
        counter++;
        cur_char = getNextCharacter(f);
        if (cur_char == 'r') // true
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'u')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (cur_char == 'e')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~true\n");
                        return cur_char;
                    }
                }
            }
        }
        else if (cur_char == 'h') // the, then
        {
            fprintf(symbolTable, "%c", cur_char);
            counter++;
            cur_char = getNextCharacter(f);

            if (cur_char == 'e')
            {
                fprintf(symbolTable, "%c", cur_char);
                counter++;
                cur_char = getNextCharacter(f);

                if (!isalnum(cur_char) && cur_char != '_')
                {
                    fprintf(symbolTable, "~the\n");
                    return cur_char;
                }

                else if (cur_char == 'n')
                {
                    fprintf(symbolTable, "%c", cur_char);
                    counter++;
                    cur_char = getNextCharacter(f);

                    if (!isalnum(cur_char) && cur_char != '_')
                    {
                        fprintf(symbolTable, "~then\n");
                        return cur_char;
                    }
                }
            }
        }
    }


    while (isalnum(cur_char) || cur_char == '_')
    {
        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
    }

    if (counter > 31)
        fprintf(symbolTable, "~INVALID\n");
    else
        fprintf(symbolTable, "~IDENTIFIER\n");
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

        fprintf(symbolTable, "%s~SLCOMMENT\n", lexeme);
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
            fprintf(symbolTable, "%s~MLCOMMENT\n", lexeme);
        else
            fprintf(symbolTable, "%s~INVALID\n", lexeme);
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
    int counter = 0;
    char *lexeme = NULL;

    lexeme = (char *)malloc(sizeof(char) + 1);
    lexeme[0] = '\0';

    char temp[2];
    temp[0] = '"';
    temp[1] = '\0';

    // fprintf(symbolTable, "\"");
    cur_char = getNextCharacter(f);
    while (cur_char != '"' && cur_char != '\n' && cur_char != EOF)
    {
        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(temp)) * sizeof(char) + 1);
        temp[0] = cur_char;
        strcat(lexeme, temp);
        // fprintf(symbolTable, "%c", cur_char);
        counter++;

        cur_char = getNextCharacter(f);
    }

    if (counter == 0)
    {
        fprintf(symbolTable, "%c~INVALID\n", temp[0]);
        // fprintf(symbolTable, " = INVALID\n");
        return cur_char;
    }
    else if (cur_char != '"')
    {
        fprintf(symbolTable, "\"%s~INVALID\n", lexeme);
        // fprintf(symbolTable, " = INVALID\n");
        return cur_char;
    }
    else
    {
        fprintf(symbolTable, "%s~CONSTWORD\n", lexeme);
        // fprintf(symbolTable, " = CONSTWORD\n");
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
        fprintf(symbolTable, "%c~INVALID\n", temp[0]);
        return cur_char;
    }

    else if (cur_char != '\'')
    {
        fprintf(symbolTable, "'%s~INVALID\n", lexeme);
        return cur_char;
    }
    else if (strlen(lexeme) > 1)
    {
        fprintf(symbolTable, "%s~INVALID\n", lexeme);
        cur_char = getNextCharacter(f);
        return cur_char;
    }
    else
    {
        fprintf(symbolTable, "%s~CONSTCHARACTER\n", lexeme);
        cur_char = getNextCharacter(f);
        return cur_char;
    }
}

bool isOperator(FILE *f, FILE *symbolTable, char *c) // converted to print directly
{
    char cur_char = *c;
    switch (cur_char)
    {
    case '=':
        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            fprintf(symbolTable, "%c", cur_char);
            fprintf(symbolTable, "~==\n");

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "~=\n");
            *c = cur_char;
        }

        return true;

    case '>':

        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            fprintf(symbolTable, "~>=\n");

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "~>\n");
            *c = cur_char;
        }
        return true;

    case '<':

        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            fprintf(symbolTable, "~<=\n");

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "~<\n");
            *c = cur_char;
        }

        return true;

    case '!':

        fprintf(symbolTable, "%c", cur_char);
        cur_char = getNextCharacter(f);
        if (cur_char == '=')
        {
            fprintf(symbolTable, "~!=\n");

            *c = getNextCharacter(f); // para magchange yung value ng currentCharacter sa driver function
        }
        else
        {
            fprintf(symbolTable, "~INVALID\n");

            *c = cur_char;
        }

        return true;

    case '+':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~+\n");

        *c = getNextCharacter(f);
        return true;

    case '-':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~-\n");

        *c = getNextCharacter(f);
        return true;

    case '/':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~/\n");

        *c = getNextCharacter(f);
        return true;

    case '*':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~*\n");

        *c = getNextCharacter(f);
        return true;

    case '%':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~%%\n");

        *c = getNextCharacter(f);
        return true;

    case '^':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~^\n");

        *c = getNextCharacter(f);
        return true;

    case '@':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~@\n");

        *c = getNextCharacter(f);
        return true;
    }
    return false;
}

bool isDelimiterBracket(FILE *f, FILE *symbolTable, char *c) // converted to print directly
{
    char cur_char = *c;
    switch (cur_char)
    {
    case '(':
        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~(\n");

        *c = getNextCharacter(f);
        return true;

    case ')':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~)\n");

        *c = getNextCharacter(f);
        return true;

    case '[':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~[\n");

        *c = getNextCharacter(f);
        return true;

    case ']':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~]\n");

        *c = getNextCharacter(f);
        return true;

    case '{':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~{\n");

        *c = getNextCharacter(f);
        return true;

    case '}':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~}\n");

        *c = getNextCharacter(f);
        return true;

    case ';':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~;\n");

        *c = getNextCharacter(f);
        return true;

    case ':':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~:\n");

        *c = getNextCharacter(f);
        return true;

    case ',':

        fprintf(symbolTable, "%c", cur_char);
        fprintf(symbolTable, "~,\n");

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
