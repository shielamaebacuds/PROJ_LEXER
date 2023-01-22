#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char resv_word[5][9] = {"num", "decimal", "boolean", "character", "word"};
char currentChar;


int main(){
    FILE *symbolTable = fopen("symbol_table.txt", "r");

    //open file
    if (symbolTable == NULL){
        printf("Error opening symbol table");
        return 1;
    }

    //read file
    getNextToken();
    fclose(symbolTable);

    return 0;
}


void getNextToken(){
    currentChar = fgetc(symbolTable);
    //dito bale may string/char na token at lexeme
    //tapos yung lexeme and token nagbabago yung size, yung ilalagay dun ay currentChar

}


//outline pa lang yung nasa baba


/*
void expect(){

}

void stmt(){
    if(token == "if" ||token == "else" ||token == "match" ||token == "while" ||token == "foreach"){
        compound_stmt();
    }
    else{
        simple_stmt();
    }
}

void simple_stmt(){
    if(resv_word){
        declaration_stmt();
        expect(; | NEWLINE);
    }
}

void compound_stmt(){

}

void declaration_stmt(){
    if(identifier);
    expect('=');
    expr();
    const_wordCharBool();
}

*/
