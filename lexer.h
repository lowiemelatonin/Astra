#ifndef LEXER_H
#define LEXER_H

#include "ast.h"

typedef enum {
    identifier_token,       // lorem_isum
    const_token,            // const
    static_token,           // static
    int_token,              // int
    int_literal_token,      // 0, 1, 2, 3...
    long_token,             // long
    long_literal_token,     // 100000000000000000000...
    float_token,            // float
    float_literal_token,    // 3.14...
    double_token,           // double
    string_token,           // string
    string_literal_token,   // "dolor sit"
    plus_token,             // +
    minus_token,            // -
    star_token,             // *
    slash_token,            // /
    percent_token,          // %
    and_token,              // &&
    or_token,               // ||
    not_token,              // !
    equal_token,            // =
    equal_equal_token,      // ==
    not_equal_token,        // !=
    less_token,             // <
    less_equal_token,       // <=
    greater_token,          // >
    greater_equal_token,    // >=
    if_token,               // if
    else_token,             // else
    for_token,              // for
    define_token,           // define
    import_token,           // import
    function_token,         // fun
    return_token,           // return
    break_token,            // break
    continue_token,         // continue
    l_paren_token,          // (
    r_paren_token,          // )
    l_bracket_token,        // [
    r_bracket_token,        // ]
    l_brace_token,          // {
    r_brace_token,          // }
    quote_token,            // '
    double_quote_token,     // "
    comma_token,            // ,
    semicolon_token,        // ;
    address_token,          // &
    dot_token,              // .
    arrow_token,            // ->
    null_token,
    eof_token
} token_type;

typedef struct {
    char *src;
    long long position;
    long line;
    long column;
} lexer;

typedef union {
    struct {
        dataValue value;
        dataType type;
    } properties;

    char *identifier;
} token_data;

typedef struct {
    token_type type;
    token_data data;
    char *lexeme;
    int line;
    int column;
} token;

token createToken(lexer *lexer, token_type type, token_data data, char *lexeme);
token nextToken(lexer *lexer, char *src);
void initLexer(lexer *lexer, char *src);
void freeLexer(lexer *lexer);
void freeToken(token *token);

#endif