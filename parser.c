#include "parser.h"

void initParser(parser *parser, lexer *lexer){
    parser->lexer = lexer;
    parser->current = nextToken(lexer);
}

void advanceParser(parser *parser){
    parser->current = nextToken(parser->lexer);
}

astNode *parseExpression(parser *parser){
    return parseLogicalOr(parser); // This is gonna be replaced soon btw  
}

astNode *parsePrimary(parser *parser){
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

astNode *parseUnary(parser *parser){
    token_type current_type = parser->current.type;

    if(current_type == not_token || current_type == minus_token || current_type == plus_token || current_type == star_token || current_type == address_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseUnary(parser);
        opType operation;

        switch(op.type){
            case not_token: 
                operation = not_op;
                break;
            case minus_token:
                operation = minus_op;
                break;
            case plus_token:
                operation = plus_op;
                break;
            case star_token:
                operation = dereference_op;
                break;
            case address_token:
                operation = address_op;
                break;
            default:
                return NULL;
        }

        return createDataOperationNode(NULL, right, operation);
    }
    return parsePrimary(parser);
}

astNode *parseMultiplicative(parser *parser){
    astNode *left = parseUnary(parser);

    while(parser->current.type == star_token || parser->current.type == slash_token || parser->current.type == percent_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseUnary(parser);

        opType operation;
        switch(op.type){
            case star_token:
                operation = star_op;
                break;
            case slash_token:
                operation = slash_op;
                break;
            case percent_token:
                operation = percent_op;
                break;
            default:
                return left;
        }
        left = createDataOperationNode(left, right, operation);
    }
    return left;
}

astNode *parseAdditive(parser *parser){
    astNode *left = parseMultiplicative(parser);

    while(parser->current.type == plus_token || parser->current.type == minus_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseMultiplicative(parser);

        opType operation;
        switch(op.type){
            case plus_token:
                operation = plus_op;
                break;
            case minus_token:
                operation = minus_op;
                break;
            default:
                return left;
        }
        left = createDataOperationNode(left, right, operation);
    }
    return left;
}

astNode *parseRelational(parser *parser){
    astNode *left = parseAdditive(parser);

    while(parser->current.type == less_token || parser->current.type == less_equal_token || parser->current.type == greater_token || parser->current.type == greater_equal_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseAdditive(parser);

        opType operation;
        switch(op.type){
            case less_token:
                operation = less_op;
                break;
            case less_equal_token:
                operation = less_or_equal_op;
                break;
            case greater_token:
                operation = greater_op;
                break;
            case greater_equal_token:
                operation = greater_or_equal_op;
                break;
            default:
                return left;
        }
        left = createDataOperationNode(left, right, operation);
    }
    return left;
}

astNode *parseEquality(parser *parser){
    astNode *left = parseRelational(parser);

    while(parser->current.type == equal_equal_token || parser->current.type == not_equal_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseRelational(parser);

        opType operation;
        switch(op.type){
            case equal_equal_token:
                operation = equal_op;
                break;
            case not_equal_token:
                operation = not_equal_op;
                break;
            default:
                return left;
        }
        left = createDataOperationNode(left, right, operation);
    }
    return left;
}

astNode *parseLogicalAnd(parser *parser){
    astNode *left = parseEquality(parser);

    while(parser->current.type == and_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseEquality(parser);
        left = createDataOperationNode(left, right, and_op);
    }
    return left;
}

astNode *parseLogicalOr(parser *parser){
    astNode *left = parseLogicalAnd(parser);

    while(parser->current.type == or_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseLogicalAnd(parser);
        left = createDataOperationNode(left, right, or_op);
    }
    return left;
}