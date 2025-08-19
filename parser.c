#include "parser.h"

void initParser(parser *parser, lexer *lexer){
    parser->lexer = lexer;
    parser->current = nextToken(lexer);
}

void advanceParser(parser *parser){
    parser->current = nextToken(parser->lexer);
}