#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

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

bool match(lexer *lexer, char expected){
    if(isAtEnd(lexer)) return false;
    if(peek(lexer) != expected) return false;

    advance(lexer);
    return true;
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

token lexNums(lexer *lexer){
    long long start = lexer->position;
    bool is_float = false;

    while(isdigit(peek(lexer))){
        advance(lexer);
    }

    if(peek(lexer) == '.' && isdigit(peekNext(lexer))){
        is_float = true;
        advance(lexer);
        while(isdigit(peek(lexer))){
            advance(lexer);
        }
    }

    long long len = lexer->position - start;
    char *numStr = malloc(len + 1);
    if(!numStr) return createToken(lexer, null_token, (token_data){0}, ""); 
    memcpy(numStr, &lexer->src[start], len);
    numStr[len] = '\0';

    token_data data = {0};
    token token;

    if(is_float){
        double val = strtod(numStr, NULL);
        data.properties.value.type = type_double;
        data.properties.value.value.d_value = val;
        token = createToken(lexer, float_literal_token, data, numStr);
    } else {
        long val = strtol(numStr, NULL, 10);
        if(val >= INT_MIN && val <= INT_MAX){
            data.properties.value.type = type_int;
            data.properties.value.value.i_value = (int)val;
            token = createToken(lexer, int_literal_token, data, numStr);
        } else {
            data.properties.value.type = type_long;
            data.properties.value.value.l_value = (long)val;
            token = createToken(lexer, long_literal_token, data, numStr);
        }
    }
    free(numStr);
    return token;
}

token nextToken(lexer *lexer, char *src);

void initLexer(lexer *lexer, char *src){
    lexer->src = strdup(src);
    lexer->position = 0;
    lexer->column = 1;
    lexer->line = 1;
}

void freeLexer(lexer *lexer){
    free(lexer->src);
    lexer->src = NULL;
}

void freeToken(token *token){
    free(token->lexeme);
    token->lexeme = NULL;
}