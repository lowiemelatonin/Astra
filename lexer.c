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

token lexStr(lexer *lexer){
    long long start = lexer->position;

    while(!isAtEnd(lexer) && peek(lexer) != '"'){
        if(peek(lexer) == '\\' && peekNext(lexer) == '"') advance(lexer);
        advance(lexer);
    }

    if(isAtEnd(lexer)){
        return createToken(lexer, null_token, (token_data){0}, "");
    }

    advance(lexer);
    long long len = lexer->position - start - 1;
    char *str = malloc(len + 1);
    if(!str) return createToken(lexer, null_token, (token_data){0}, "");

    memcpy(str, &lexer->src[start], len);
    str[len] = '\0';

    token_data data = {0};
    data.properties.value.type = type_string;
    data.properties.value.value.str_value = strdup(str);

    token token = createToken(lexer, string_literal_token, data, str);
    return token;
}

token lexIdent(lexer *lexer){
    long long start = lexer->position;
    while(isalnum(peek(lexer)) || peek(lexer) == '_'){
        advance(lexer);
    }

    long long len = lexer->position - start;
    char *text = malloc(len + 1);
    if(!text) return createToken(lexer, null_token, (token_data){0}, "");

    memcpy(text, &lexer->src[start], len);
    text[len] = '\0';

    token_type type = identifier_token;
    #define keyword(str, tk) if(strcmp(text, str) == 0) type = tk;

    keyword("if", if_token);
    keyword("else", else_token);
    keyword("for", for_token);
    keyword("define", define_token);

    keyword("import", import_token);
    keyword("fun", function_token);
    keyword("return", return_token);
    keyword("break", break_token);

    keyword("continue", continue_token);
    keyword("const", const_token);
    keyword("static", static_token);
    keyword("int", int_token);

    keyword("long", long_token);
    keyword("float", float_token);
    keyword("double", double_token);
    keyword("string", string_token);

    #undef keyword;

    token_data data = {0};
    if(type == identifier_token){
        data.identifier = strdup(text);
    }

    token token = createToken(lexer, type, data, text);
    free(text);
    return token;
}

token nextToken(lexer *lexer){
    skipWhiteSpace(lexer);

    if(isAtEnd(lexer)){
        return createToken(lexer, eof_token, (token_data){0}, "");
    }

    char current = advance(lexer);

    if(isalpha(current) || current == '_'){
        lexer->position--;
        lexer->column--;
        return lexIdent(lexer);
    }

    if(isdigit(current)){
        lexer->position--;
        lexer->column--;
        return lexNums(lexer);
    }

    if(current == '"' || current == '\''){
        return lexStr(lexer);
    }

    switch(current){
        case '+':
            return createToken(lexer, plus_token, (token_data){0}, "+");
        case '-':
            if(match(lexer, ">")) return createToken(lexer, arrow_token, (token_data){0}, "->");
            return createToken(lexer, minus_token, (token_data){0}, "-");
        case '*':
            return createToken(lexer, star_token, (token_data){0}, '*');
        case '/':
            return createToken(lexer, slash_token, (token_data){0}, '/');
        case '%':
            return createToken(lexer, percent_token, (token_data){0}, '%');
        case '=':
            if(match(lexer, '=')) return createToken(lexer, equal_equal_token, (token_data){0}, "==");
            return createToken(lexer, equal_token, (token_data){0}, '=');
        case '!':
            if(match(lexer, '=')) return createToken(lexer, not_equal_token, (token_data){0}, "!=");
            return createToken(lexer, not_token, (token_data){0}, '!');
        case '>':
            if(match(lexer, '=')) return createToken(lexer, greater_equal_token, (token_data){0}, ">=");
            return createToken(lexer, greater_token, (token_data){0}, '>');
        case '<':
            if(match(lexer, '=')) return createToken(lexer, less_equal_token, (token_data){0}, "<=");
            return createToken(lexer, less_token, (token_data){0}, '<');
        case '&':
            if(match(lexer, '&')) return createToken(lexer, and_token, (token_data){0}, "&&");
            return createToken(lexer, address_token, (token_data){0}, '&');
        case '|':
            return createToken(lexer, or_token, (token_data){0}, "||");
        case '(':
            return createToken(lexer, l_paren_token, (token_data){0}, '(');
        case ')':
            return createToken(lexer, r_paren_token, (token_data){0}, ')');
        case '[':
            return createToken(lexer, l_bracket_token, (token_data){0}, '[');
        case ']':
            return createToken(lexer, r_bracket_token, (token_data){0}, ']');
        case '{':
            return createToken(lexer, l_brace_token, (token_data){0}, '{');
        case '}':
            return createToken(lexer, r_brace_token, (token_data){0}, '}');
        case ',':
            return createToken(lexer, comma_token, (token_data){0}, ',');
        case '.':
            return createToken(lexer, dot_token, (token_data){0}, '.');
        case ';':
            return createToken(lexer, semicolon_token, (token_data){0}, ';');
        default:
            return createToken(lexer, null_token, (token_data){0}, "");
    }
}

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