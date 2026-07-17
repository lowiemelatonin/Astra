#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

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

bool isAtEnd(lexer *lexer){
    return lexer->src[lexer->position] == '\0';
}

bool match(lexer *lexer, char expected){
    if(isAtEnd(lexer)) return false;
    if(peek(lexer) != expected) return false;

    advance(lexer);
    return true;
}

void skipWhiteSpace(lexer *lexer){
    while(1){
        if(isspace(peek(lexer))){
            advance(lexer);
        } 
        else if(peek(lexer) == '/' && peekNext(lexer) == '/'){
            while(!isAtEnd(lexer) && peek(lexer) != '\n'){
                advance(lexer);
            }
        }
        else if(peek(lexer) == '/' && peekNext(lexer) == '*'){
            advance(lexer);
            advance(lexer);

            while(!isAtEnd(lexer)){
                if(peek(lexer) == '*' && peekNext(lexer) == '/'){
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                advance(lexer);
            }
        } 
        else {
            break;
        }
    }
}

static int hexValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

token createToken(lexer *lexer, token_type type, token_data *data, char *lexeme){
    token token;
    token.type = type;
    token.data = *data;
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
    int base = 10;
    int prefix_offset = 0;

    if(peek(lexer) == '0'){
        char next = peekNext(lexer);
        
        if(next == 'x' || next == 'X'){
            base = 16;
            prefix_offset = 2;
            advance(lexer);
            advance(lexer);
            while(isxdigit(peek(lexer))) advance(lexer);
            
        } else if (next == 'b' || next == 'B'){
            base = 2;
            prefix_offset = 2;
            advance(lexer);
            advance(lexer);
            while(peek(lexer) == '0' || peek(lexer) == '1') advance(lexer);
            
        } else if(next == 'o' || next == 'O'){
            base = 8;
            prefix_offset = 2;
            advance(lexer);
            advance(lexer);
            while(peek(lexer) >= '0' && peek(lexer) <= '7') advance(lexer);
            
        } else if(isdigit(next)){
            base = 8;
            advance(lexer);
            
            while(isdigit(peek(lexer))) {
                if(peek(lexer) == '8' || peek(lexer) == '9') base = 10;
                advance(lexer);
            }
        } else {
            advance(lexer);
        }
    } else {
        while(isdigit(peek(lexer))){
            advance(lexer);
        }
    }

    if(base == 10 && peek(lexer) == '.' && isdigit(peekNext(lexer))){
        is_float = true;
        advance(lexer);
        while(isdigit(peek(lexer))){
            advance(lexer);
        }
    }

    long long len = lexer->position - start;
    char *numStr = malloc(len + 1);
    if(!numStr) return createToken(lexer, null_token, &(token_data){0}, ""); 
    
    memcpy(numStr, &lexer->src[start], len);
    numStr[len] = '\0';

    token_data data = {0};
    token token;

    if(is_float){
        double val = strtod(numStr, NULL);
        data.properties.value.type = type_double;
        data.properties.value.value.d_value = val;
        token = createToken(lexer, float_literal_token, &data, numStr);
    } else {
        char *parseStr = numStr + prefix_offset;
        
        long long val = strtoll(parseStr, NULL, base);
        
        if(val >= INT_MIN && val <= INT_MAX){
            data.properties.value.type = type_int;
            data.properties.value.value.i_value = (int)val;
            token = createToken(lexer, int_literal_token, &data, numStr);
        } else if(val >= LONG_MIN && val <= LONG_MAX){
            data.properties.value.type = type_long;
            data.properties.value.value.l_value = (long)val;
            token = createToken(lexer, long_literal_token, &data, numStr);
        } else {
            data.properties.value.type = type_long_long;
            data.properties.value.value.ll_value = val;
            token = createToken(lexer, long_long_literal_token, &data, numStr);
        }
    }
    
    free(numStr);
    return token;
}

token lexStr(lexer *lexer){
    char delimiter = lexer->src[lexer->position - 1];
    long long start = lexer->position;

    while(!isAtEnd(lexer) && peek(lexer) != delimiter){
        if(peek(lexer) == '\\' && !isAtEnd(lexer)){
            advance(lexer);
        }
        advance(lexer);
    }

    if(isAtEnd(lexer)){
        return createToken(lexer, null_token, &(token_data){0}, "");
    }

    long long end = lexer->position;
    advance(lexer);

    long long max_len = end - start;
    char *str = malloc(max_len + 1);
    if(!str) return createToken(lexer, null_token, &(token_data){0}, "");

    long long src_idx = start;
    long long dest_idx = 0;

    while(src_idx < end){
        if(lexer->src[src_idx] == '\\' && src_idx + 1 < end){
            src_idx++;
            char escape_char = lexer->src[src_idx];

            switch(escape_char) {
                case 'a': str[dest_idx++] = '\a'; break; 
                case 'b': str[dest_idx++] = '\b'; break; 
                case 'f': str[dest_idx++] = '\f'; break; 
                case 'n': str[dest_idx++] = '\n'; break; 
                case 'r': str[dest_idx++] = '\r'; break; 
                case 't': str[dest_idx++] = '\t'; break; 
                case 'v': str[dest_idx++] = '\v'; break; 
                case '\\': str[dest_idx++] = '\\'; break; 
                case '\'': str[dest_idx++] = '\''; break; 
                case '"': str[dest_idx++] = '"'; break;  
                case '?': str[dest_idx++] = '\?'; break; 

                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7': {
                    int octal_val = 0;
                    int count = 0;
                    while(src_idx < end && count < 3 && lexer->src[src_idx] >= '0' && lexer->src[src_idx] <= '7') {
                        octal_val = octal_val * 8 + (lexer->src[src_idx] - '0');
                        src_idx++;
                        count++;
                    }
                    src_idx--;
                    str[dest_idx++] = (char)octal_val;
                    break;
                }

                case 'x': {
                    src_idx++;
                    int hex_val = 0;
                    int count = 0;
                    int val;
                    while(src_idx < end && (val = hexValue(lexer->src[src_idx])) != -1) {
                        hex_val = hex_val * 16 + val;
                        src_idx++;
                        count++;
                    }
                    if (count == 0) {
                        str[dest_idx++] = 'x';
                    } else {
                        str[dest_idx++] = (char)hex_val;
                        src_idx--;
                    }
                    break;
                }

                default:
                    str[dest_idx++] = escape_char;
                    break;
            }
        } else {
            str[dest_idx++] = lexer->src[src_idx];
        }
        src_idx++;
    }
    str[dest_idx] = '\0'; 

    token_data data = {0};
    data.properties.value.type = type_string;
    data.properties.value.value.str_value = strdup(str);

    token token = createToken(lexer, string_literal_token, &data, str);

    free(str);
    return token;
}

token lexIdent(lexer *lexer){
    long long start = lexer->position;
    while(isalnum(peek(lexer)) || peek(lexer) == '_'){
        advance(lexer);
    }

    long long len = lexer->position - start;
    char *text = malloc(len + 1);
    if(!text) return createToken(lexer, null_token, &(token_data){0}, "");

    memcpy(text, &lexer->src[start], len);
    text[len] = '\0';

    token_type type = identifier_token;
    #define keyword(str, tk) if(strcmp(text, str) == 0) type = tk;

    keyword("if", if_token);
    keyword("else", else_token);
    keyword("switch", switch_token);
    keyword("case", case_token);
    keyword("default", default_token);
    keyword("for", for_token);
    keyword("while", while_token);
    keyword("do", do_token);

    keyword("import", import_token);
    keyword("impl", impl_token);
    keyword("trait", trait_token);
    keyword("self", self_token);
    keyword("fun", function_token);
    keyword("return", return_token);
    keyword("break", break_token);

    keyword("continue", continue_token);
    keyword("const", const_token);
    keyword("static", static_token);
    keyword("extern", extern_token);
    keyword("volatile", volatile_token);

    keyword("bool", bool_token);
    keyword("true", true_token);
    keyword("false", false_token);
    keyword("null", null_literal_token);

    keyword("short", short_token);
    keyword("ushort", ushort_token);
    keyword("int", int_token);
    keyword("uint", uint_token);
    keyword("long", long_token);
    keyword("ulong", ullong_token);
    keyword("ullong", ullong_token);
    keyword("float", float_token);
    keyword("double", double_token);
    keyword("string", string_token);
    keyword("typedef", typedef_token);
    keyword("struct", struct_token);
    keyword("enum", enum_token);
    keyword("union", union_token);
    
    keyword("sizeof", sizeof_token);
    keyword("typeof", typeof_token);

    #undef keyword

    token_data data = {0};
    if(type == identifier_token){
        data.identifier = strdup(text);
    } else if(type == true_token){
        data.properties.value.type = type_bool;
        data.properties.value.value.b_value = 1;
    } else if(type == false_token){
        data.properties.value.type = type_bool;
        data.properties.value.value.b_value = 0;
    }

    token token = createToken(lexer, type, &data, text);
    free(text);
    return token;
}

token nextToken(lexer *lexer){
    skipWhiteSpace(lexer);

    if(isAtEnd(lexer)){
        return createToken(lexer, eof_token, &(token_data){0}, "");
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
            if(match(lexer, '+')) return createToken(lexer, increment_token, &(token_data){0}, "++");
            if(match(lexer, '=')) return createToken(lexer, plus_equal_token, &(token_data){0}, "+=");
            return createToken(lexer, plus_token, &(token_data){0}, "+");
        case '-':
            if(match(lexer, '-')) return createToken(lexer, decrement_token, &(token_data){0}, "--");
            if(match(lexer, '>')) return createToken(lexer, arrow_token, &(token_data){0}, "->");
            if(match(lexer, '=')) return createToken(lexer, minus_equal_token, &(token_data){0}, "-=");
            return createToken(lexer, minus_token, &(token_data){0}, "-");
        case '*':
            if(match(lexer, '=')) return createToken(lexer, star_equal_token, &(token_data){0}, "*=");
            return createToken(lexer, star_token, &(token_data){0}, "*");
        case '/':
            if(match(lexer, '=')) return createToken(lexer, slash_equal_token, &(token_data){0}, "/=");
            return createToken(lexer, slash_token, &(token_data){0}, "/");
        case '%':
            if(match(lexer, '=')) return createToken(lexer, percent_equal_token, &(token_data){0}, "%=");
            return createToken(lexer, percent_token, &(token_data){0}, "%");
        case '=':
            if(match(lexer, '=')) return createToken(lexer, equal_equal_token, &(token_data){0}, "==");
            return createToken(lexer, equal_token, &(token_data){0}, "=");
        case '!':
            if(match(lexer, '=')) return createToken(lexer, not_equal_token, &(token_data){0}, "!=");
            return createToken(lexer, not_token, &(token_data){0}, "!");
        case '>':
            if(match(lexer, '>')){
                if(match(lexer, '=')) return createToken(lexer, shift_right_equal_token, &(token_data){0}, ">>=");
                return createToken(lexer, shift_right_token, &(token_data){0}, ">>");
            }
            if(match(lexer, '=')) return createToken(lexer, greater_equal_token, &(token_data){0}, ">=");
            return createToken(lexer, greater_token, &(token_data){0}, ">");
        case '<':
            if(match(lexer, '<')){
                if(match(lexer, '=')) return createToken(lexer, shift_left_equal_token, &(token_data){0}, "<<=");
                return createToken(lexer, shift_left_token, &(token_data){0}, "<<");
            }
            if(match(lexer, '=')) return createToken(lexer, less_equal_token, &(token_data){0}, "<=");
            return createToken(lexer, less_token, &(token_data){0}, "<");
        case '&':
            if(match(lexer, '&')) return createToken(lexer, and_token, &(token_data){0}, "&&");
            if(match(lexer, '=')) return createToken(lexer, and_equal_token, &(token_data){0}, "&=");
            return createToken(lexer, address_token, &(token_data){0}, "&");
        case '|':
            if(match(lexer, '|')) return createToken(lexer, or_token, &(token_data){0}, "||");
            if(match(lexer, '=')) return createToken(lexer, or_equal_token, &(token_data){0}, "|=");
            return createToken(lexer, bitwise_or_token, &(token_data){0}, "|");
        case '^':
            if(match(lexer, '=')) return createToken(lexer, xor_equal_token, &(token_data){0}, "^=");
            return createToken(lexer, bitwise_xor_token, &(token_data){0}, "^");
        case '~':
            return createToken(lexer, bitwise_not_token, &(token_data){0}, "~");
        case '(':
            return createToken(lexer, l_paren_token, &(token_data){0}, "(");
        case ')':
            return createToken(lexer, r_paren_token, &(token_data){0}, ")");
        case '[':
            return createToken(lexer, l_bracket_token, &(token_data){0}, "[");
        case ']':
            return createToken(lexer, r_bracket_token, &(token_data){0}, "]");
        case '{':
            return createToken(lexer, l_brace_token, &(token_data){0}, "{");
        case '}':
            return createToken(lexer, r_brace_token, &(token_data){0}, "}");
        case ',':
            return createToken(lexer, comma_token, &(token_data){0}, ",");
        case '.':
            if(peek(lexer) == '.' && peekNext(lexer) == '.'){
                advance(lexer);
                advance(lexer);
                return createToken(lexer, ellipsis_token, &(token_data){0}, "...");
            }
            return createToken(lexer, dot_token, &(token_data){0}, ".");
        case ';':
            return createToken(lexer, semicolon_token, &(token_data){0}, ";");
        case ':':
            return createToken(lexer, colon_token, &(token_data){0}, ":");
        default:
            return createToken(lexer, null_token, &(token_data){0}, "");
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

void freeToken(token *t) {
    if (t->lexeme != NULL) {
        free(t->lexeme);
        t->lexeme = NULL;
    }

    if (t->type == identifier_token && t->data.identifier != NULL) {
        free(t->data.identifier);
        t->data.identifier = NULL;
    }

    if (t->type == string_literal_token) {
        if (t->data.properties.value.value.str_value != NULL) {
            free(t->data.properties.value.value.str_value);
            t->data.properties.value.value.str_value = NULL;
        }
    }
}