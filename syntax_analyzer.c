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
char *tokenLine;

void getNextToken(); // getting the next token in the symbol table
void stmt(); //check whether the stmt is simple or compound
void expect(char expectedToken[]); //chek if the next token is the same as the expectedToken
void declaration_stmt(); //check if declaration stmt
void error(char message[]); //display error message
bool resv_word(); //check if resv_word
bool const_wordCharBool();//check if the toke is a CONSTWORD | CONSTCHARACTER | false | true
int line;

void lowest_logic_expr();
void lower_logic_expr();
bool boolean_operator();
bool logical_operator();
bool arithmetic_operator();
bool const_numDec();
void low_logic_expr();
int factor();
void expr_factor();
void expr();
void arithmetic_expr();
void higher_term();
void term();

int simple_stmt();

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

    tokenLine = NULL;
    tokenLine = (char*)malloc(sizeof(char)+1);
    tokenLine[0] = '\0';

    line = 1;

    while(currentChar[0]!=EOF){
        getNextToken();
        stmt();
        printf("\nLEXEME:%s\n\n", lexeme);
        lexeme[0] = '\0';
    }
    fclose(symbolTable);

    return 0;
}


void getNextToken(){

        token[0] = '\0';

        currentChar[0]=fgetc(symbolTable);
        currentChar[1]='\0';

        while(currentChar[0]!='~' && currentChar[0]!=EOF){

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

        tokenLine = (char *)realloc(tokenLine, (strlen(tokenLine) + strlen(token)) * sizeof(char) + 1);
        strcat(tokenLine,token);

        tokenLine = (char *)realloc(tokenLine, (strlen(tokenLine) + strlen(space)) * sizeof(char) + 1);
        strcat(tokenLine,space);

}


void expect(char expectedToken[]){
    getNextToken();

    if(strcmp(token,expectedToken) == 0){
        printf("\n%s", token);
    }else{
        error("error: unexpected symbol");
    }



}



void error(char message[]){
    printf("\n%s", message);
}


void stmt(){
    printf("\n-><stmt>");
    if(strcmp(token,"if")==0 || strcmp(token,"else")==0 || strcmp(token,"foreach")==0 || strcmp(token,"match") ==0|| strcmp(token,"while")==0){
        ;
        //compound_stmt(lexeme,token);
    }
    else{

        simple_stmt();
    }
}

int simple_stmt(){
    printf("\n(LINE%d)", line);

    printf("\n-><simple_stmt>");
    if(resv_word()){
        printf("\n-><declarative_stmt>");
        printf("\n<resv_word>");
        expect("IDENTIFIER");
        expect("=");
        expr();
    }
    else if(strcmp(token,"IDENTIFIER")==0){
        printf("\n-><assignment_stmt>");
        printf("\n->IDENTIFIER");
        expect("=");
        expr();
    }
    else if(strcmp(token,"display")==0){
        ;
    }
    else if(strcmp(token,"return")==0){
        ;
    }
    else if(strcmp(token,"input")==0){
        printf("\n-><input_stmt>");
        printf("\ninput");
        expect("(");
        getNextToken();
        if(const_wordCharBool()){
            printf("\n%s",token);
        }
        else{
            error("\nerror: unexpected symbol");
        }
        expect(",");
        expect("IDENTIFIER");
        expect(")");
        getNextToken();
    }
    else if(strcmp(token,"list")==0){
        printf("%s ", token);
        expect("IDENTIFIER");
        expect("=");
        expect("[");
        getNextToken();
        const_wordCharBool();

    }
    else{
        error("error: unexpected symbol");
    }



    //for newline and optional semicolon
    if(strcmp(token,"NEWLINE")==0){
        printf("\n%s", token);
    }
    else if(token[0]==';'){
        printf("\n%s", token);
        getNextToken();

        if(strcmp(token,"NEWLINE")==0){
            printf("\n%s", token);
        }
        else{
            error("error: unexpected symbol (missing a NEWLINE)");
            while(strcmp(token,"NEWLINE")!=0){
                getNextToken();
            }
            printf("\nTOKEN LINE:%s", tokenLine);
            tokenLine[0]='\0';
            line = line +1;
            return 0;
        }
    }
    else if(currentChar[0]==EOF){
        ;
    }
    else{
        error("error: unexpected symbol");

        while(strcmp(token,"NEWLINE")!=0){
            getNextToken();
        }
        printf("\nTOKEN LINE:%s", tokenLine);
        tokenLine[0]='\0';
        line = line +1;
        return 0;
    }

    printf("\nTOKEN LINE:%s", tokenLine);
    tokenLine[0]='\0';
    line = line +1;

return 1;

}

/*
void compound_stmt(){

}
*/


bool resv_word(){

    if(strcmp(token,"word")==0 || strcmp(token,"character")==0 || strcmp(token,"num")==0 || strcmp(token,"boolean") ==0|| strcmp(token,"decimal")==0){
        return true;
    }
    else{

        return false;
    }


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
        return true;
    }
    else{
        return false;
    }

}

bool const_numDec(){

    if(strcmp(token,"CONSTNUMBER")==0 || strcmp(token,"CONSTDECIMAL")==0){
        return true;
    }
    else{
        return false;
    }

}



//EXPRESSION!!



void expr(){
    getNextToken();
    printf("\n<expr>{");

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

            printf("\n\t<lowest_logic_expr>{");
            lowest_logic_expr(); //get the leftmost term



            printf("\n\t\t\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t}");
            printf("\n\t\t\t\t}");
            printf("\n\t\t\t}");
            printf("\n\t\t}");
            printf("\n\t}");

            if(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){
                getNextToken();
            }else{
                break;
            }

    }

    printf("\n}");
    return 0;
}


void lowest_logic_expr(){

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(strcmp(token, "or") == 0 ){
            printf("\n\t\t\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t\t}");
            printf("\n\t\t\t\t\t}");
            printf("\n\t\t\t\t}");
            printf("\n\t\t\t}");
            printf("\n\t\t}");
            printf("\n\t}");

            printf("\n\n\t%s\n", token);

            getNextToken();

            if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='('){ //get next lowest_logic_expr of expr()

                printf("\n\t<lowest_logic_expr>{");
                lowest_logic_expr();
                break;

            }
            else{//unexpected symbol eg 12+3>
                error("\n\tunexpected symbol7");
            }

        }
        else{
            printf("\n\t\t<lower_logic_expr>{");
            lower_logic_expr(); //get leftmost lower_logic_expr of lowest_logic_expr()
            break;
        }

        getNextToken();

    }


}


void lower_logic_expr(){

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){
        if(strcmp(token, "and") == 0){
                printf("\n\t\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t}");
                printf("\n\t\t\t\t}");
                printf("\n\t\t\t}");
                printf("\n\t\t}");
                printf("\n\n\t\t%s\n", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]=='-' ){//get next lower_logic_expr of lowest_logic_expr()

                    printf("\n\t\t<lower_logic_expr>{");
                    lower_logic_expr();
                    break;

                }
                else{
                    error("\t\tunexpected symbol6");
                }
                break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else{
            printf("\n\t\t\t<low_logic_expr>{");
            low_logic_expr();
            break;
        }


        getNextToken();


    }


}



void low_logic_expr(){

   while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(strcmp(token, "not") == 0){
                printf("\n\t\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t}");
                printf("\n\t\t\t\t}");
                printf("\n\t\t\t}");
                printf("\n\n\t\t\t%s\n", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]=='-' ){//get next lower_logic_expr of lowest_logic_expr()

                    printf("\n\t\t\t<low_logic_expr>{");
                    low_logic_expr();
                    break;
                }
                else{
                    error("\t\tunexpected symbol5");
                }
                break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else{
            printf("\n\t\t\t\t<expr_factor>{");
            expr_factor();

            break;
        }
        getNextToken();
    }

}

void expr_factor(){


    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(boolean_operator()){
                printf("\n\t\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t}");
                printf("\n\t\t\t\t}");
                printf("\n\n\t\t\t\t%s\n", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='-' ){//get next lower_logic_expr of lowest_logic_expr()

                    printf("\n\t\t\t\t<expr_factor>{");
                    expr_factor();
                    break;
                }
                else{
                    error("\t\tunexpected symbol43");
                }
               break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else if(strcmp(token, "not") == 0){
            low_logic_expr();
            break;
        }
        else if(const_wordCharBool()){
            printf("\n\t\t\t\t\t<const_wordCharBool>{");
            printf("\n\t\t\t\t\t\t%s", token);

        }
        else if(const_numDec() || strcmp(token, "IDENTIFIER") == 0 || arithmetic_operator()){
            printf("\n\t\t\t\t\t<arithmetic_expr>{");
            printf("\n\t\t\t\t\t\t<higher_term>{");
            higher_term();
            break;
        }
        else{
            printf("\n\t\t\t\t\t\t%s", token);
            break;
        }
        getNextToken();
    }

}

void higher_term(){

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(token[0] == '+' || token[0] == '-'){
                printf("\n\t\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t}");
                printf("\n\n\t\t\t\t\t\t%s\n", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' ){//get next lower_logic_expr of lowest_logic_expr()

                    printf("\n\t\t\t\t\t\t<higher_term>{");
                    higher_term();
                    break;
                }
                else{
                    error("\t\t\t\t\tunexpected symbol3");
                }
                break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else if(strcmp(token, "not") == 0){
            low_logic_expr();
            break;
        }
        else if(boolean_operator()){
            expr_factor();
            break;
        }
        else if(const_wordCharBool()){
            expr_factor();
            break;
        }
        else{
            printf("\n\t\t\t\t\t\t\t<term>{");
            term();
            break;
        }
        getNextToken();
    }


}

void term(){

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(token[0] == '*' || token[0] == '/' || token[0] == '@' || token[0] == '%'){
                printf("\n\t\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t\t}");
                printf("\n\t\t\t\t\t\t\t}");
                printf("\n\n\t\t\t\t\t\t\t%s\n", token);
                getNextToken();

                if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' ){//get next lower_logic_expr of lowest_logic_expr()

                    printf("\n\t\t\t\t\t\t\t<term>{");
                    term();
                    break;
                }
                else{
                    error("\t\t\t\t\tunexpected symbol2");
                }
                break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else if(strcmp(token, "not") == 0){
            low_logic_expr();
            break;
        }
        else if(boolean_operator()){
            expr_factor();
            break;
        }
        else if(token[0] == '+' || token[0] == '-'){
            higher_term();
            break;

        }
        else if(const_wordCharBool()){
            expr_factor();
            break;
        }
        else{
          printf("\n\t\t\t\t\t\t\t\t<arithmetic_factor>{");
           arithmetic_factor();
           break;

        }
        getNextToken();
    }



}


void arithmetic_factor(){

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){


        if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else if(strcmp(token, "not") == 0){
            low_logic_expr();
            break;
        }
        else if(boolean_operator()){
            expr_factor();
            break;
        }
        else if(token[0] == '+' || token[0] == '-'){
            higher_term();
            break;

        }
        else if(token[0] == '*' || token[0] == '/' || token[0] == '@' || token[0] == '%' ){
            term();
            break;
        }
        else if(const_wordCharBool()){
            expr_factor();
            break;
        }
        else if(strcmp(token, "IDENTIFIER") == 0 || const_numDec()){
          printf("\n\t\t\t\t\t\t\t\t\t<factor>{");
           factor();
           break;

        }

        getNextToken();
    }

}


int factor(){


    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){

        if(token[0]=='^'){
            printf("\n\t\t\t\t\t\t\t\t\t}");
            printf("\n\n\t\t\t\t\t\t\t\t\t%s\n", token);
            getNextToken();

            if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' ){//get next lower_logic_expr of lowest_logic_expr()

                printf("\n\t\t\t\t\t\t\t\t\t<factor>{");
                factor();
                break;

            }
            else{
                error("\t\t\t\t\tunexpected symbol1");
            }
            break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(strcmp(token, "and") == 0){
            lower_logic_expr();
            break;
        }
        else if(strcmp(token, "not") == 0){
            low_logic_expr();
            break;
        }
        else if(boolean_operator()){
            expr_factor();
            break;
        }
        else if(token[0] == '+' || token[0] == '-'){
            higher_term();
            break;
        }
        else if(token[0] == '*' || token[0] == '/' || token[0] == '@' || token[0] == '%' ){
            term();
            break;
        }
        else if(const_wordCharBool()){
            expr_factor();
            break;
        }
        else if(strcmp(token, "IDENTIFIER") == 0){
            printf("\n\t\t\t\t\t\t\t\t\t\t%s", token);


        }
        else if(const_numDec()){
            printf("\n\t\t\t\t\t\t\t\t\t\t<const_numDec>");
            printf("\n\t\t\t\t\t\t\t\t\t\t\t%s", token);
            printf("\n\t\t\t\t\t\t\t\t\t\t}");

        }


        getNextToken();

    }

}




