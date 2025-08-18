#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    lexer *lexer;
    token current;
} parser;

void initParser(parser *parser, lexer *lexer);
void advance(parser *parser);
#endif