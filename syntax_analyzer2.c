#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexer.c"


char currentChar[2];    //for current character
FILE *symbolTable;      //for symbolTable.txt
char *lexeme;           //lexeme line
char space[2];          //whitespace
char *token;            //current token
char *errorLine;        //error line
char *tokenLine;        //token line
FILE *syntaxTable;      //for syntaxTable.txt
int leftParenCount;     //number of left paren
int rightParenCount;    //number of right paren

void getNextToken();               // getting the next token in the symbol table
void stmt();                       // check whether the stmt is simple or compound
void expect(char expectedToken[]); // chek if the next token is the same as the expectedToken
void declaration_stmt();           // check if declaration stmt
void error(char message[]);        // display error message
bool resv_word();                  // check if resv_word
bool const_wordCharBool();         // check if the token is a CONSTWORD | CONSTCHARACTER | false | true
int line;                          //current line in the program

void printToTable();             //print details to syntaxTable.txt
void compound_stmt();            //check if compound statement
int simple_stmt();               //check if simple statement
void elseif_stmt();              //check if else if stmt
void else_stmt();                //check if else stmt
int case_stmt();                 //check if case stmt

bool boolean_operator();         //check if token is a boolean operator
bool logical_operator();         //check if token is a logical operator
bool arithmetic_operator();      //check if token is a arithmetic operator
bool const_numDec();             //check if token is a number or decimal

void lowest_logic_expr();        //"or" expr
void lower_logic_expr();         //"and" expr
void low_logic_expr();           //"not" expr
int factor();                    //const_numDec and identifier
void expr_factor();              //const_wordCharBool, (expr)
void expr();                     //boolean expr
void arithmetic_expr();          //+ - expr
void arithmetic_factor();        //factor, (arithmetic_factor)
void higher_term();              //* / @ % expr
void term();                     //^ expr


int main(){
    lexer();
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

    errorLine = NULL;
    errorLine = (char*)malloc(sizeof(char)+1);
    errorLine[0] = '\0';

    line = 1;

    syntaxTable = fopen("syntaxTable.txt", "w");
    if (syntaxTable == NULL)
    {
        printf("Error opening syntax table");
        return 1;
    }


    while(strcmp(token,"EOF")!=0){
        getNextToken();


            stmt();

            printToTable();

            printf("\nLEXEME:%s", lexeme);
            lexeme[0] = '\0';

            printf("\nTOKEN LINE:%s\n\n", tokenLine);
            tokenLine[0]='\0';

            errorLine[0]='\0';
            line = line +1;



    }
    printf("Syntax Table was successfully generated!\n");
    fclose(symbolTable);
    fclose(syntaxTable);

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

        if(strcmp(token,"SLCOMMENT")==0||strcmp(token,"MLCOMMENT")==0|| strcmp(token,"an")==0 ||strcmp(token,"an")==0){ //ignore comments and noise words
            getNextToken();
        }
        else{

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






}


void expect(char expectedToken[]){
    getNextToken();

    char errorMessage [50] = "error:missing ";

    strcat(errorMessage,expectedToken);

    if(strcmp(token,expectedToken) == 0){
        printf("\n%s", token);
    }
    else{
        error(errorMessage);
    }


}


void error(char message[]){
    printf("\n%s", message);

    space[0] = ' ';
    space[1] = '\0';

    errorLine = (char *)realloc(errorLine, (strlen(errorLine) + strlen(space)) * sizeof(char) + 1);
    strcat(errorLine,space);

    errorLine = (char *)realloc(errorLine, (strlen(errorLine) + strlen(message)) * sizeof(char) + 1);
    strcat(errorLine,message);

}

void expr_error(){
    char message[24] = "error:unexpected symbol";

    space[0] = ' ';
    space[1] = '\0';

    errorLine = (char *)realloc(errorLine, (strlen(errorLine) + strlen(space)) * sizeof(char) + 1);
    strcat(errorLine,space);

    errorLine = (char *)realloc(errorLine, (strlen(errorLine) + strlen(message)) * sizeof(char) + 1);
    strcat(errorLine,message);

}


void stmt(){
    printf("\n-><stmt>");


    if(strcmp(token,"if")==0 || strcmp(token,"foreach")==0 || strcmp(token,"match") ==0|| strcmp(token,"while")==0){
        compound_stmt();
    }
    else if(strcmp(token,"else")==0){
        error("error: missing if statement");

        printf("\n-><compound_stmt>");

        fprintf(syntaxTable,"\n(LINE%d) to", line);

        getNextToken();

        if(strcmp(token, "if") == 0){ //elseif_stmt
            elseif_stmt();
            return;
        }
        else_stmt();

    }
    else{
        simple_stmt();
    }
}

void printToTable(){
    fprintf(syntaxTable,"\n(LINE%d)", line);
    fprintf(syntaxTable, "\nLEXEME:%s",lexeme);
    fprintf(syntaxTable, "\nTOKEN LINE:%s",tokenLine);
    fprintf(syntaxTable, "\n");
    fprintf(syntaxTable, errorLine);
    fprintf(syntaxTable, "\n");


}



int simple_stmt(){
    printf("\n(LINE%d)", line);

    printf("\n-><simple_stmt>");

    if(resv_word()){ //declarative_stmt
        printf("\n-><declarative_stmt>");
        printf("\n<resv_word>");
        expect("IDENTIFIER");
        expect("=");
        getNextToken();
        expr();
    }
    else if(strcmp(token,"IDENTIFIER")==0){ //assignment_stmt
        printf("\n-><assignment_stmt>");
        printf("\n->IDENTIFIER");
        expect("=");
        getNextToken();
        expr();
    }
    else if(strcmp(token,"display")==0){ //display_stmt
        printf("\n-><display_stmt>");
        printf("\ndisplay");
        expect("(");

        getNextToken();

        if(strcmp(token,"CONSTWORD")==0 || strcmp(token,"IDENTIFIER")==0)
        {
            while((strcmp(token,"CONSTWORD")==0 || strcmp(token,"IDENTIFIER")==0))
            {
                printf("\n%s",token);
                getNextToken();
                if (strcmp(token,",")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                }
                else if(strcmp(token,")")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                    break;
                }

                else
                {
                    error("unexpected symbol");
                    break;
                }

            }

            if(strcmp(token,"NEWLINE")!=0)
            {
                error("unexpected symbol");
            }
        }
        else
        {
            expr();
        }

    }
    else if(strcmp(token,"return")==0){ //return_stmt
        printf("\n-><return_stmt>");
        printf("\nreturn");
        expect("(");

        getNextToken();
        if(strcmp(token,"CONSTWORD")==0 || strcmp(token,"IDENTIFIER")==0)
        {
            while((strcmp(token,"CONSTWORD")==0 || strcmp(token,"IDENTIFIER")==0))
            {
                printf("\n%s",token);
                getNextToken();
                if (strcmp(token,",")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                }

                else if(strcmp(token,")")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                    break;
                }

                else
                {
                    error("unexpected symbol");
                    break;
                }

            }

            if(strcmp(token,"NEWLINE")!=0)
            {
                error("unexpected symbol");
            }
        }
        else
        {
            expr();
        }


    }
    else if(strcmp(token,"input")==0){ //input_stmt
        printf("\n-><input_stmt>");
        printf("\ninput");
        expect("(");

        getNextToken();
        if(strcmp(token,"CONSTWORD")==0)
        {
            while((strcmp(token,"CONSTWORD")==0 || strcmp(token,"IDENTIFIER")==0))
            {
                printf("\n%s",token);
                getNextToken();
                if (strcmp(token,",")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                }

                else if(strcmp(token,")")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                    break;
                }

                else
                {
                    error("unexpected symbol");
                    break;
                }

            }

            if(strcmp(token,"NEWLINE")!=0)
            {
                error("unexpected symbol");
            }
        }
    }
    else if(strcmp(token,"list")==0){ //list_stmt
      printf("\n-><list_stmt>");
        printf("\nlist");
        expect("IDENTIFIER");
        expect("=");
        expect("[");

        getNextToken();
        if(const_numDec() || const_wordCharBool() || strcmp(token,"IDENTIFIER")==0)
        {
            while(const_numDec() || const_wordCharBool() || strcmp(token,"IDENTIFIER")==0)
            {
                printf("\n%s",token);
                getNextToken();
                if (strcmp(token,",")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                }

                else if(strcmp(token,"]")==0)
                {
                    printf("\n%s",token);
                    getNextToken();
                    break;
                }

                else
                {
                    error("error: unexpected symbol");
                    break;
                }

            }

            if(strcmp(token,"NEWLINE")!=0)
            {
                error("error: unexpected symbol");
            }
        }


    }else if(strcmp(token,"EOF")==0){
        return 0;

    }
    else{
        error("error: unexpected symbol");
    }



    //check for newline and optional semicolon
    if(strcmp(token,"NEWLINE")==0){ //if NEWLINE exists
        printf("\n%s", token);
    }
    else if(token[0]==';'){ //if there's a semicolon
        printf("\n%s", token);
        getNextToken();

        if(strcmp(token,"NEWLINE")==0){ //if NEWLINE exists after semicolon
            printf("\n%s", token);
        }
        else{
            error("error: missing NEWLINE"); //missing NEWLINE

            //get the rest of the line until there's a NEWLINE
            while(strcmp(token,"NEWLINE")!=0){
                getNextToken();
            }
            printf("\nTOKEN LINE:%s", tokenLine);
            tokenLine[0]='\0';
            line = line +1;
            return 0;
        }
    }
    else if(strcmp(token,"EOF")==0){ // if end of the file
        return 1;
    }
    else{
        error("error: missing NEWLINE"); //missing NEWLINE

        //get the rest of the line until there's a NEWLINE
        while(strcmp(token,"NEWLINE")!=0){
            getNextToken();
        }
        printf("\nTOKEN LINE:%s", tokenLine);
        tokenLine[0]='\0';
        line = line +1;
        return 0;
    }

return 1;

}

void compound_stmt()
{
    printf("\n(LINE%d)", line);
    printf("\n-><compound_stmt>");

    fprintf(syntaxTable,"\n(LINE%d) to", line);

    if (strcmp(token, "if") == 0) //if_stmt
    {
        printf("\n-><if_stmt>{");
        printf("\nif");
        getNextToken();
        expr();

        if (strcmp(token, "then") == 0){
            printf("\nthen");
            getNextToken();

            if (token[0]=='{'){
                printf("\n{");
                getNextToken();

                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                }
                else{
                    error("error: missing NEWLINE");
                }

             }
             else{
                error("error: missing {");

                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                }
                else{
                    error("error: missing NEWLINE");
                    line = line - 1;
                }
             }
        }
        else{
            error("error: missing then");

             if (token[0]=='{'){
                printf("\n{");
                getNextToken();

                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                }
                else{
                    error("error: missing NEWLINE");
                }
             }
             else{
                error("error: missing {");

                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                }
                else{
                    error("error: missing NEWLINE");
                    line = line - 1;
                }
             }

        }
        getNextToken();

        while (strcmp(token, "}") != 0)
        {   line = line +1;
            stmt();
            getNextToken();
        }
        if (strcmp(token, "}") == 0){
            printf("\n}");
            getNextToken();


            //ELSE IF & ELSE
            if(strcmp(token, "NEWLINE") == 0){
                printf("\nNEWLINE");
                line = line +1;
            }
            else if(strcmp(token, "EOF") == 0){
                printf("\nEOF");
                line = line + 1;
            }
            else if(strcmp(token, "else") == 0){
                getNextToken();
                line = line +1;
                if(strcmp(token, "if") == 0){ //elseif_stmt
                    elseif_stmt();
                    return;
                }
                else_stmt();
            }
            else{
                error("error: unexpected symbol");
            }
        }
    }
    else if (strcmp(token, "while") == 0){ //while_stmt
        printf("\n-><while_stmt>{");
        printf("\nwhile");
        getNextToken();
        expr();
        if (strcmp(token, "do") == 0)
        {
            printf("\ndo");
            expect("{");
            expect("NEWLINE");
        }
        getNextToken();
        while (strcmp(token, "}") != 0)
        {   line = line +1;
            stmt();
            getNextToken();
        }
        if (strcmp(token, "}") == 0)
        {
            printf("\n}");
            getNextToken();

            if(strcmp(token, "NEWLINE") == 0 || strcmp(token, "EOF") == 0 ){
                printf("\n%s",token);
            }
            else{
                error("error: missing NEWLINE");
            }
        }
    }
    else if (strcmp(token, "foreach") == 0){ //foreach_stmt
        printf("\n-><foreach_stmt>{");
        printf("\nforeach");
        expect("IDENTIFIER");
        expect("in");
        expect("IDENTIFIER");
        getNextToken();

        if (strcmp(token, "{") == 0)
        {
            printf("\n{");
            expect("NEWLINE");
        }
        getNextToken();
        while (strcmp(token, "}") != 0)
        {   line = line +1;
            stmt();
            getNextToken();
        }
        if (strcmp(token, "}") == 0)
        {
            printf("\n}");
            getNextToken();

            if(strcmp(token, "NEWLINE") == 0 || strcmp(token, "EOF") == 0 ){
                printf("\n%s",token);
            }
            else{
                error("error: missing NEWLINE");
            }
        }
    }
    else if (strcmp(token, "match") == 0){
        printf("\n-><match_stmt>{");
        printf("\nmatch");
        expect("IDENTIFIER");
        expect(":");
        expect("{");
        expect("NEWLINE");
        getNextToken();

        while (strcmp(token, "}") != 0){
            line = line + 1;
            case_stmt();
            getNextToken();
        }
        if (strcmp(token, "}") == 0)
        {
            printf("\n}");
            getNextToken();

            if(strcmp(token, "NEWLINE") == 0 || strcmp(token, "EOF") == 0 ){
                printf("\n%s",token);
            }
            else{
                error("error: missing NEWLINE");
            }
        }
    }




}



void elseif_stmt(){

            printf("\n(LINE %d)", line);
            printf("\n-><elseif_stmt>");
            printf("\nelse");
            printf("\nif");
            getNextToken();
            expr();

            if (strcmp(token, "then") == 0){
                printf("\nthen");
            }
            else{
                error("error: missing then");
            }
            expect("{");
            expect("NEWLINE");

            getNextToken();

            while (token[0]!='}'){
                line = line +1;
                stmt();
                getNextToken();

            }

            if (token[0]=='}'){
                printf("}");
            }
            else{
                error("error: missing }");
            }

            getNextToken();

            if(strcmp(token, "else") == 0){
                getNextToken();
                line = line +1;
                if(strcmp(token, "if") == 0){
                    elseif_stmt();
                }
                else if(strcmp(token, "then") == 0){ //else_stmt
                    else_stmt();
                }
            }
            else if(strcmp(token, "NEWLINE") == 0){
                printf("\nNEWLINE");
                line = line +1;
            }
            else if(strcmp(token, "EOF") == 0){
                    printf("\nEOF");
                    line = line + 1;
            }
            else{
                error("error: unexpected symbol");
            }

}


void else_stmt(){

        printf("\n(LINE %d)", line);
        printf("\n-><elseif_stmt>");
        printf("\nelse");

            if (strcmp(token, "then") == 0){
                printf("\nthen");
            }
            else{
                error("error: missing then");
            }
            expect("{");
            expect("NEWLINE");
            getNextToken();

            while (token[0]!='}'){
                line = line +1;
                stmt();
                getNextToken();
            }

            if (token[0]=='}'){
                printf("}");

                getNextToken();
                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                    line = line +1;
                }
                else if(strcmp(token, "EOF") == 0){
                    printf("\nEOF");
                    line = line + 1;
                }
                else{
                    error("error: missing NEWLINE");
                }
            }
            else{
                error("error: missing }");

                if (strcmp(token, "NEWLINE") == 0){
                    printf("\nNEWLINE");
                    line = line +1;
                }
                else if(strcmp(token, "EOF") == 0){
                    printf("\nEOF");
                    line = line + 1;
                }
                else{
                    error("error: missing NEWLINE");
                }

            }


}

int case_stmt(){
    if (strcmp(token, "case") == 0){
        printf("\ncase");
        getNextToken();
    }

    if (strcmp(token, "!") == 0){
        printf("\n!");
        expect(":");
        expect("{");
        expect("NEWLINE");
        line = line + 1;
        getNextToken();
        stmt();
        expect("end");
        expect("}");
        expect("NEWLINE");
        line = line + 1;
    }
    else{
        expr();

        if (strcmp(token, ":") == 0){
            printf("\n:");
        }
        expect("{");
        expect("NEWLINE");
        line = line + 1;
        getNextToken();
        stmt();
        expect("end");
        expect("}");
        expect("NEWLINE");
        line = line + 1;
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

bool boolean_operator(){


    if(strcmp(token, "==") == 0 || strcmp(token, ">") == 0 || strcmp(token, "<") == 0|| strcmp(token, "!=") == 0 ||strcmp(token, ">=") == 0 || strcmp(token, "<=") == 0){
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

    leftParenCount = 0;
    rightParenCount = 0;

    printf("\n<expr>{");

    if(token[0]=='-'){
        printf("\n\t%s",token);
        getNextToken();
    }

    while(const_numDec()|| token[0]=='(' || token[0]==')' || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){


                if(token[0]=='('){
                    printf("\n%s\n",token);
                    getNextToken();
                    leftParenCount = leftParenCount +1;
                }
                else if(token[0]==')'){
                    printf("\n%s\n",token);
                    getNextToken();
                    rightParenCount = rightParenCount +1;
                }

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

                if(const_numDec() || strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || boolean_operator() || logical_operator() || arithmetic_operator()){
                    getNextToken();
                }
                else{
                    break;
                }


    }


    printf("\n}");


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

            if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                printf("\n\t<lowest_logic_expr>{");
                lowest_logic_expr();
                break;

            }
            else if(token[0]=='-'){
                printf("\n\n\t%s\n", token);
                getNextToken();
                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t<lowest_logic_expr>{");
                    lowest_logic_expr();
                    break;

                }
                else{//unexpected symbol eg 12+3>
                printf("\n\terror:unexpected symbol");
                expr_error();
                }
                 break;
            }
            else{//unexpected symbol eg 12+3>
                printf("\n\terror:unexpected symbol");
                expr_error();
            }
             break;

        }
        else if(token[0]=='('){
            printf("\n\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            lowest_logic_expr();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            lowest_logic_expr();
            break;
        }
        else{
            printf("\n\t\t<lower_logic_expr>{");
            lower_logic_expr();
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

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t\t<lower_logic_expr>{");
                    lower_logic_expr();
                    break;

                }
                else if(token[0]=='-'){
                    printf("\n\n\t\t%s\n", token);
                    getNextToken();

                    if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                        printf("\n\t\t<lower_logic_expr>{");
                        lower_logic_expr();
                        break;

                    }
                    else{
                        printf("\t\terror:unexpected symbol");
                        expr_error();
                    }
                    break;
                }
                else{
                    printf("\t\terror:unexpected symbol");
                    expr_error();
                }
                break;
        }
        else if(strcmp(token, "or") == 0){
            lowest_logic_expr();
            break;
        }
        else if(token[0]=='('){
            printf("\n\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            lower_logic_expr();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            lower_logic_expr();
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

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t\t\t<low_logic_expr>{");
                    low_logic_expr();
                    break;
                }
                else if(token[0]=='-' ){
                    printf("\n\n\t\t\t%s\n", token);
                    getNextToken();

                     if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                        printf("\n\t\t\t<low_logic_expr>{");
                        low_logic_expr();
                        break;
                    }
                    else{
                        printf("\t\t\terror:unexpected symbol");
                        expr_error();
                    }
                    break;

                }
                else{
                    printf("\t\t\terror:unexpected symbol");
                    expr_error();
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
        else if(token[0]=='('){
            printf("\n\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            low_logic_expr();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            low_logic_expr();
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

                if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t\t\t\t<expr_factor>{");
                    expr_factor();
                    break;
                }
                else if(token[0]=='-'){
                    printf("\n\n\t\t\t\t%s\n", token);
                    getNextToken();

                    if(strcmp(token, "IDENTIFIER") == 0 || const_wordCharBool() || const_numDec() || token[0]=='(' || token[0]==')'){

                        printf("\n\t\t\t\t<expr_factor>{");
                        expr_factor();
                        break;
                    }
                    else{
                    printf("\t\t\t\terror:unexpected symbol");
                    expr_error();
                    }
                    break;

                }
                else{
                    printf("\t\t\t\terror:unexpected symbol");
                    expr_error();
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
        else if(token[0]=='('){
            printf("\n\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            expr_factor();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            expr_factor();
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

                if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t\t\t\t\t\t<higher_term>{");
                    higher_term();
                    break;
                }
                else if(token[0] == '-'){

                    printf("\n\n\t\t\t\t\t\t%s\n", token);
                    printf("\n\t\t\t\t\t\t<higher_term>{");
                    getNextToken();
                    higher_term();
                    break;

                }
                else{
                    printf("\t\t\t\t\t\terror:unexpected symbol");
                    expr_error();
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
        else if(token[0]=='('){
            printf("\n\t\t\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            higher_term();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            higher_term();
        }
        else if(boolean_operator()){
            higher_term();
            break;
        }
        else if(const_wordCharBool()){
            higher_term();
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

                if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' || token[0]==')'){

                    printf("\n\t\t\t\t\t\t\t<term>{");
                    term();
                    break;
                }
                else if(token[0]=='-'){
                    printf("\n\n\t\t\t\t\t\t\t%s\n", token);
                    getNextToken();

                    if(strcmp(token, "IDENTIFIER") == 0 || const_numDec() || token[0]=='(' || token[0]==')'){

                        printf("\n\t\t\t\t\t\t\t<term>{");
                        term();
                        break;
                    }
                    else{
                        printf("\t\t\t\t\t\t\terror:unexpected symbol");
                        expr_error();
                        break;
                    }


                }
                else{
                    printf("\t\t\t\t\t\t\terror:unexpected symbol");
                    expr_error();
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
        else if(token[0]=='('){
            printf("\n\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            term();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            term();
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
        else if(token[0]=='('){
            printf("\n\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            arithmetic_factor();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            arithmetic_factor();
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

            if(strcmp(token, "IDENTIFIER") == 0 || const_numDec()){
                printf("\n\t\t\t\t\t\t\t\t\t<factor>{");
                factor();
                break;

            }
            else{
                printf("\t\t\t\t\t\t\t\t\terror:unexpected symbol");
                expr_error();
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
        else if(token[0]=='('){
            printf("\n\t\t\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            break;
        }
        else if(strcmp(token, "IDENTIFIER") == 0){
            printf("\n\t\t\t\t\t\t\t\t\t\t%s", token);

        }
        else if(token[0]=='('){
            printf("\n\t\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            leftParenCount = leftParenCount +1;
            factor();
            break;
        }
        else if(token[0]==')'){
            printf("\n\t\t\t\t\t\t\t\t%s\n",token);
            getNextToken();
            rightParenCount = rightParenCount +1;
            factor();
            break;
        }
        else if(const_numDec()){
            printf("\n\t\t\t\t\t\t\t\t\t\t<const_numDec>");
            printf("\n\t\t\t\t\t\t\t\t\t\t\t%s", token);
            printf("\n\t\t\t\t\t\t\t\t\t\t}");

        }



        getNextToken();

    }

}




