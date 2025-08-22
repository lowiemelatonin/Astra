#include "parser.h"

void initParser(parser *parser, lexer *lexer){
    parser->lexer = lexer;
    parser->current = nextToken(lexer);
}

void advanceParser(parser *parser){
    parser->current = nextToken(parser->lexer);
}

parser *parseExpression(parser *parser){
    return parsePrimary(parser); // This is gonna be replaced soon btw  
}

parser *parsePrimary(parser *parser){
    token token = parser->current;

    if(token.type == int_literal_token || token.type == long_literal_token || token.type == float_literal_token || token.type == string_literal_token){
        dataValue val = token.data.properties.value;
        advanceParser(parser);
        return createValueNode(val);
    }

    if(token.type == identifier_token){
        char *name = strdup(token.data.identifier);
        advanceParser(parser);
        return createIdentifierNode(name);
    }

    if(token.type == l_paren_token){
        advanceParser(parser);
        astNode *expr = parseExpression(parser);
        if(parser->current.type != r_paren_token){
            return NULL;
        }
        advanceParser(parser);
        return expr;
    }

    return NULL;
}