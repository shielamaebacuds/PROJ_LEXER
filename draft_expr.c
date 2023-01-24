
//di pa maayos ito. draft or outline pa lang ng expression part



//currently ginagawa ko itong part na ito na kasama na bool at logic
void expr(){

    printf("\n<expr>{");

    while(token==ID|CONST|ARITH_OP|LOGIC_OP|BOOL_OP|'('|')'){
        

    }

}



void lowest_logic_expr(){




}



///ito yung una kong ginawa na arithmetic lang

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


