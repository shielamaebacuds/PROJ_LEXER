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
void expect(char expectedToken[]); //chek if the next token is the same as the expectedToken
void declaration_stmt(); //check if declaration stmt
void error(char message[]); //display error message
bool resv_word(); //check if resv_word
bool const_wordCharBool(); //check if the toke is a CONSTWORD | CONSTCHARACTER | false | true




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

    token = NULL;
    token = (char*)malloc(sizeof(char)+1);


    //read file
    stmt();
    fclose(symbolTable);

    return 0;
}


void getNextToken(){


    token[0] = '\0';

    currentChar[0]=fgetc(symbolTable);
    currentChar[1]='\0';

    while(currentChar[0]!='~'){

        token = (char *)realloc(token, (strlen(token) + strlen(currentChar)) * sizeof(char) + 1);
        strcat(token,currentChar);

        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';
    }


    currentChar[0]=fgetc(symbolTable);
    currentChar[1]='\0';



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



}


void expect(char expectedToken[]){
    getNextToken();

    if(strcmp(expectedToken, ";") == 0){
        if(strcmp(token, expectedToken) == 0){
            printf("%s ", token);
            getNextToken();
        }

    }
    else {
        if(strcmp(token,expectedToken) == 0){
            printf("%s ", token);
        }else{
        error("expect: unexpected symbol");
        }
    }



}





void error(char message[]){
    printf("\n%s", message);
}


void stmt(){
    getNextToken();

    if(strcmp(token,"if")==0 || strcmp(token,"else")==0 || strcmp(token,"foreach")==0 || strcmp(token,"match") ==0|| strcmp(token,"while")==0){
        ;
        //compound_stmt(lexeme,token);
    }
    else{
        simple_stmt();
    }
}

void simple_stmt(){

    if(resv_word){
        printf("%s ", token);
        declaration_stmt();
        //expect(";");
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

    if(strcmp(token,"IDENTIFIER")==0){
        printf("%s ", token);
        expect("=");
        getNextToken();
        const_wordCharBool();
        expr(); // ITO YUNG CURRENTLY NA GINAGAGAWA KO. AYUN YUNG DRAFT_EXPR
    }else{
        printf("\nerror");
    }

}

bool resv_word(){

    if(strcmp(token,"word")==0 || strcmp(token,"character")==0 || strcmp(token,"num")==0 || strcmp(token,"boolean") ==0|| strcmp(token,"decimal")==0){
        return true;
    }
    else{

        return false;
    }


}


bool const_wordCharBool(){

    if(strcmp(token,"CONSTWORD")==0 || strcmp(token,"CONSTCHARACTER")==0){
        printf("%s ", token);
        return true;
    }
    else if(strcmp(token,"true")==0 || strcmp(token,"false")==0){
        printf("%s ", token);
        return true;
    }
    else{
        return false;
    }

}


