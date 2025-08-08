#include "ast.h"
#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char peek(lexer *lexer){
    return lexer->src[lexer->position];
}

char peekNext(lexer *lexer){
    if(lexer->src[lexer->position + 1] == '\0') return '\0';
    return lexer->src[lexer->position + 1];
}

char advance(lexer *lexer){
    char current = lexer->src[lexer->position];
    if(current == '\0') return '\0';

    lexer->position++;
    if(current == '\n'){
        lexer->line++;
        lexer->column = 1;
    } else{
        lexer->column++;
    }
    return current;
}

void skipWhiteSpace(lexer *lexer){
    while(isspace(lexer->src[lexer->position])){
        advance(lexer);
    }
}

bool isAtEnd(lexer *lexer){
    return lexer->src[lexer->position] == '\0';
}

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

token nextToken(lexer *lexer, char *src);

void initLexer(lexer *lexer, char *src){
    lexer->src = strdup(src);
    lexer->position = 0;
    lexer->column = 1;
    lexer->line = 1;
}