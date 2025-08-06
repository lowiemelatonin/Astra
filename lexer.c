#include "ast.h"
#include "lexer.h"
#include <string.h>

token createToken(lexer *lexer, token_type type, token_data data, char *lexeme){
    token token;
    token.type = type;
    token.data = data;
    token.line = lexer->line;
    token.column = lexer->column;
    token.lexeme = strdup(lexeme);

    if(!token.lexeme){
        token.type = eof_token;
        token.data.identifier = NULL;
        return token;
    }

    return token;
}

token nextToken(lexer *lexer, char *src){

}

void initLexer(lexer *lexer, char *src){

}