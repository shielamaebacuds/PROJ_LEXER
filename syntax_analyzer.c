#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


char currentChar[2];
FILE *symbolTable;
int position;
char *lexeme;
char space[2];
char *token;


void getNextToken(); // getting the next token in the symbol table
void stmt(); //check whether the stmt is simple or compound
int expect(char expectedToken[]); //chek if the next token is the same as the expectedToken
void declaration_stmt(); //check if declaration stmt
void error(char message[]); //display error message
bool resv_word(); //check if resv_word


int main(){
    symbolTable = fopen("symbol_table.txt", "r");

    //open file
    if (symbolTable == NULL){
        printf("Error opening symbol table");
        return 1;
    }

    //iniatlization of arrays

    lexeme = NULL;
    lexeme = (char*)malloc(sizeof(char)+1);
    lexeme[0] = '\0';

    //token[0] = '\0';

    //read file
    stmt();
    fclose(symbolTable);

    return 0;
}


void getNextToken(){

    token = NULL;
    token = (char*)malloc(sizeof(char)+1);
    token[0] = '\0';

    currentChar[0]=fgetc(symbolTable);
    currentChar[1]='\0';
    
    
    //get token
    while(currentChar[0]!='~'){

        token = (char *)realloc(token, (strlen(token) + strlen(currentChar)) * sizeof(char) + 1);
        strcat(token,currentChar);

        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';
    }


    currentChar[0]=fgetc(symbolTable);
    currentChar[1]='\0';


    //get lexeme
    while(currentChar[0]!='\n'){

        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(currentChar)) * sizeof(char) + 1);
        strcat(lexeme,currentChar);

        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';
    }

    space[0] = ' ';
    space[1] = '\0';

    lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(space)) * sizeof(char) + 1);
    strcat(lexeme,space);
    //printf("%s\t", token);


}


int expect(char expectedToken[]){

    printf(expectedToken);
    printf(token);
    getNextToken();
    if(token==expectedToken){
        printf(expectedToken);
        return 1;
    }
    error("1expect: unexpected symbol");
    return 0;

}


void error(char message[]){
    printf("\n%s", message);
}



void stmt(){
    getNextToken();
    printf("<stmt>\n");
    if(token == "if" ||token == "else" ||token == "match" ||token == "while" ||token == "foreach"){
        ;
        //compound_stmt(lexeme,token);
    }
    else{
        simple_stmt();
    }
}

void simple_stmt(){
    printf("<simple_stmt>\n");

    if(resv_word){
        printf("<declaration_stmt>\n");
        printf("<resv_word> ");
        declaration_stmt();
        //optional(;);
        //expect(NEWLINE);
        printf("\n%s", lexeme);
    }
    else{
        printf("<wrong!> ");
    }
}

/*
void compound_stmt(){

}
*/

void declaration_stmt(){

    getNextToken();
    //printf(token);

    //MAY BUG DITO. AYAW MAG EQUAL NUNG TOKEN AT IDENTIFIER
    if(token == "IDENTIFIER"){
        printf(token);
    }else{
        printf("error");
    }

}

bool resv_word(){


    if(token == "word" || token == "boolean" || token == "character" || token == "num" || token == "decimal"){
        return true;
    }
    else{

        return false;
    }


}
