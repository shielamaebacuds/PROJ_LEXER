//DRAFT DRAFT DRAFT (just for testing <expr> rules)

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
// char *expr;

void getNextToken(); // getting the next token in the symbol table
void stmt(); //check whether the stmt is simple or compound
void expect(char expectedToken[]); //chek if the next token is the same as the expectedToken
void declaration_stmt(); //check if declaration stmt
void error(char message[]); //display error message
bool resv_word(); //check if resv_word
bool const_wordCharBool(); //check if the toke is a CONSTWORD | CONSTCHARACTER | false | true
bool arithmetic_expr();
bool arithmetic_term();
void simple_stmt();
void expr();
bool lowest_logic_expr();
bool lower_logic_expr();
bool low_logic_expr();
bool expr_factor();
bool maykapartnernaparenthesis = false;


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

    // expr = NULL;
    // expr = (char*)malloc(sizeof(char)+1);
    // expr[0] = '\0';

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
        printf("\nLexeme: %s\n", lexeme);
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
        //const_wordCharBool();
        //arithmetic_expr();

        expr();

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

bool arithmetic_expr(){

    //arithmetic_term
    if(arithmetic_term){
        printf("%s ", token);
        return true;
    }
    // else if(){

    // }
    else{
        return false;
    }

}

bool arithmetic_term(){

    //arithmetic_term
    if(strcmp(token,"IDENTIFIER")==0 || strcmp(token,"CONSTNUM")==0 || strcmp(token,"CONSTDECIMAL")==0){
        return true;
    }
    else{
        return false;
    }

}

bool highest(){

    if(arithmetic_term){

    }

}

void expr(){
    // pwedeng arithmetic, boolean, and logical expr
    bool merongBooleanOperator = true; // if may sunod pang boolean operator yung expr

    printf(" <expr>{");
    merongBooleanOperator=lowest_logic_expr();

    while(merongBooleanOperator==true)
    {  
        getNextToken();
        merongBooleanOperator=lowest_logic_expr();
    }
    
    if (maykapartnernaparenthesis==false)
    {
        if(strcmp(token,";")!=0 && strcmp(token,"NEWLINE")!=0)
        {   
            while(strcmp(token,"NEWLINE")!=0)
            {
                getNextToken(); // error until newline is encountered
            }
            printf("\nWALA SA RULES");
        }
    }

    return;    
    printf("\n  <expr>}");

    
}

bool lowest_logic_expr (){
    
    bool merongORoperator = true;
    merongORoperator = lower_logic_expr();
    while (merongORoperator == true)
    {
        getNextToken();
        merongORoperator = lower_logic_expr();
    }

    if(strcmp(token,"<")==0|| strcmp(token,">")==0 || strcmp(token,"==")==0 || strcmp(token,"<=")==0 || strcmp(token,">=")==0 || strcmp(token,"!=")==0) 
    {   
        return true;
    }

    return false;
}

bool lower_logic_expr()
{
    bool merongANDoperator = true;
    merongANDoperator = low_logic_expr();
    while(merongANDoperator== true)
    {
        getNextToken();
        merongANDoperator = low_logic_expr();
    }

    if(strcmp(token,"or")==0)
    {
        return true;
    }

    return false;
}

bool low_logic_expr()
{
    bool merongNOToperator = true;
    merongNOToperator = expr_factor();
    while(merongNOToperator==true)
    {
        getNextToken();
        merongNOToperator = expr_factor();
    }

    if (strcmp(token,"and")==0)
    {
        return true;
    }

    return false;
}

bool expr_factor()
{


    if (strcmp(token,"true")==0 || strcmp(token,"false")==0 || strcmp(token,"IDENTIFIER")==0 
        || strcmp(token, "CONSTNUMBER")==0 || strcmp(token,"CONSTDECIMAL")==0 )
    {
        getNextToken();
        if (strcmp(token,"not")==0)
        {
            return true;
        }
        
        return false;
    }
    //else if identifier, number, decimal, (<expr>), <arithmetic_expr>
    
    else if (strcmp(token,"(")==0)
    {
            
        maykapartnernaparenthesis=true;
        getNextToken();
        expr();
        
        if(strcmp(token,")")!=0) // expect(RPAREN) dapat kaso di ko mahanap kung saan nag bug kapag true or (false or true) NEWLINE, yung newline na yung naeexpect
        {
            while(strcmp(token,"NEWLINE")!=0)
            {
                getNextToken(); // error until newline is encountered
            }
            printf("\nNo right parenthesis ')' before token: [%s]", token);

            return false;
        }

        getNextToken();
        if (strcmp(token,"not")==0)
        {
            return true;
        }
        
        return false;

    }

    else if (maykapartnernaparenthesis==false)
    {
        while(strcmp(token,"NEWLINE")!=0)
        {
            getNextToken(); // error until newline is encountered
        }
        printf("\nWala sa ruless");

        return false;
        
    }

    else return false;

}


