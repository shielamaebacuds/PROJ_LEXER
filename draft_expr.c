#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>



//di pa maayos ito. draft or outline pa lang ng expression

char currentChar[2];
FILE *symbolTable;
int position;
char *lexeme;
char space[2];
char *token;


void getNextToken(); // getting the next token in the symbol table
void lowest_logic_expr();
void lower_logic_expr();
bool boolean_operator();
bool logical_operator();
bool arithmetic_operator();
bool const_wordCharBool();
bool const_numDec();


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

    getNextToken();
    expr();

    fclose(symbolTable);

    return 0;

}

void getNextToken(){


        token[0] = '\0';

        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';

    if(currentChar[0]!=EOF){
        while(currentChar[0]!='~'){

            token = (char *)realloc(token, (strlen(token) + strlen(currentChar)) * sizeof(char) + 1);
            strcat(token,currentChar);

            currentChar[0]=fgetc(symbolTable);
            currentChar[1]='\0';
        }


        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';



        while(currentChar[0]!='\n' && currentChar[0]!=EOF){

            lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(currentChar)) * sizeof(char) + 1);
            strcat(lexeme,currentChar);

            currentChar[0]=fgetc(symbolTable);
            currentChar[1]='\0';
        }

        space[0] = ' ';
        space[1] = '\0';

        lexeme = (char *)realloc(lexeme, (strlen(lexeme) + strlen(space)) * sizeof(char) + 1);
        strcat(lexeme,space);

        position = ftell(symbolTable);
    }

}

void error(char message[]){
    printf("\n%s", message);
}


void expr(){

    printf(" <expr>{");

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        lowest_logic_expr(); //get the leftmost lowest_logic_expr of expr()

        if(boolean_operator()){ //get bool operator if exists
            printf("\n\t%s", token);

            getNextToken();

            if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){ //get next lowest_logic_expr of expr()

                lowest_logic_expr();
            }
            else{//unexpected symbol eg 12+3>
                error("unexpected symbol");
            }

        }
        else if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() ){//unexpected symbol eg 12 + 3 12
            error("unexpected symbol1");
        }
        else if(token[0]==')'){//missing '(' symbol eg 12 +3 )
            error("missing ( symbol2");
        }

        getNextToken();
    }

    printf("\n}");
}


void lowest_logic_expr(){

    printf("\n\t<lowest_logic_expr>{");

    while(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || arithmetic_operator() || logical_operator() || token[0]=='(' || token[0]==')' ){

        if(boolean_operator()){
            getNextToken();
            break;
        }
        else{
            lower_logic_expr(); //get leftmost lower_logic_expr of lowest_logic_expr()

            if(strcmp(token, "or") == 0){
                printf("\n\t%s", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]=='-' ){//get next lower_logic_expr of lowest_logic_expr()

                    lower_logic_expr();

                }
                else{//unexpected symbol eg 12+3>
                    error("\t\tunexpected symbol3");
                }

            }
            getNextToken();
        }

    }

    printf("\n\t}");

}


void lower_logic_expr(){

    printf("\n\t\t<lower_logic_expr>{");
    printf("\n\t\t%s", token);
    //getNextToken();
    printf("\n\t\t}");


}

bool boolean_operator(){


    if(strcmp(token, "==") == 0 || token[0]== '>' || token[0]=='<'|| strcmp(token, "!=") == 0 ||strcmp(token, ">=") == 0 || strcmp(token, "<=") == 0){
        return true;
    }
    else{
        return false;
    }

}

bool logical_operator(){


    if(strcmp(token, "and") == 0 || strcmp(token, "not") == 0 ||strcmp(token, "or") == 0){
        return true;
    }
    else{
        return false;
    }

}

bool arithmetic_operator(){


    if(token[0]=='+' || token[0]=='-'  || token[0]=='*' || token[0]=='/'  || token[0]=='@' || token[0]=='^' ){
        return true;
    }
    else{
        return false;
    }

}

bool const_wordCharBool(){

    if(strcmp(token,"CONSTWORD")==0 || strcmp(token,"CONSTCHARACTER")==0 || strcmp(token,"true")==0 || strcmp(token,"false")==0){
        //printf("%s ", token);
        return true;
    }
    else{
        return false;
    }

}

bool const_numDec(){

    if(strcmp(token,"CONSTNUMBER")==0 || strcmp(token,"CONSTDECIMAL")==0){
        //printf("%s ", token);
        return true;
    }
    else{
        return false;
    }

}
/*
void arithmetic_expr(){
    printf("\n<arithmetic_expr>{");

    if(token==ID|CONST|'-'){

        while(token==ID|CONST|OP|(|)){

            higher_term(); //first left term

            if(token==+|-){ //first operator
                O = +|-;
                higher_term(); //get right
            }
            else if(token==ID|CONST|(){
                    error();
            }

        }


    }
    else{
        error();
    }

     printf("\n}");

}



void higher_term(){

    //get the left side of the expression
    while(token!= +- && token==ID|CONST|(|O){

              if(token == '('){ //if there is a grouping


                left =+ token;
                getNextToken();

                while(token!=')' && token!='\n'){
                    left =+ token;
                    getNextToken();

                    if(token=='\n'){
                        error(); //missing ')'
                    }
                }


              }
              else{//if there is no grouping
                left =+ token;
                getNextToken();

              }
        }
    printf("\n<higher_term>{");

}
*/

