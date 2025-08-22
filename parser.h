#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    lexer *lexer;
    token current;
} parser;

void initParser(parser *parser, lexer *lexer);
void advanceParser(parser *parser);
parser *parseExpression(parser *parser);

#endif