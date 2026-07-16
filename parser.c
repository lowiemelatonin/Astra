#include "parser.h"

// Forward declarations
astNode *parseAssignment(parser *parser);
astNode *parsePrimary(parser *parser);
astNode *parsePostfix(parser *parser);
astNode *parseUnary(parser *parser);
astNode *parseMultiplicative(parser *parser);
astNode *parseAdditive(parser *parser);
astNode *parseRelational(parser *parser);
astNode *parseEquality(parser *parser);
astNode *parseLogicalAnd(parser *parser);
astNode *parseLogicalOr(parser *parser);
astNode *parseShift(parser *parser);
astNode *parseBitwiseAnd(parser *parser);
astNode *parseBitwiseXor(parser *parser);
astNode *parseBitwiseOr(parser *parser);
astNode *parseReturnStatement(parser *parser);
astNode *parseContinueStatement(parser *parser);
astNode *parseBreakStatement(parser *parser);
astNode *parseBody(parser *parser);
astNode *parseIfStatement(parser *parser);
astNode *parseSwitchStatement(parser *parser);
astNode *parseCaseStatement(parser *parser);
astNode *parseDefaultStatement(parser *parser);
astNode *parseForStatement(parser *parser);
astNode *parseWhileStatement(parser *parser);
astNode *parseDoWhileStatement(parser *parser);
astNode *parseImportStatement(parser *parser);
astNode *parseFunction(parser *parser);
astNode *parseStruct(parser *parser);
astNode *parseImplStatement(parser *parser);
astNode *parseTraitStatement(parser *parser);
astNode *parseUnion(parser *parser);
astNode *parseEnum(parser *parser);
astNode *parseTypedef(parser *parser);
astNode *parseParamList(parser *parser, int *is_variadic);
astNode *parseType(parser *parser);
dataFlags parseFlags(parser *parser);
static int isTypeToken(token_type type);
static token_type peekNextTokenType(parser *parser);

void initParser(parser *parser, lexer *lexer){
    parser->lexer = lexer;
    parser->current = nextToken(lexer);
}

void advanceParser(parser *parser){
    freeToken(&parser->current);
    parser->current = nextToken(parser->lexer);
}

astNode *parseAssignment(parser *parser){
    astNode *left = parseLogicalOr(parser);
    token_type type = parser->current.type;

    if(type == equal_token || type == plus_equal_token || type == minus_equal_token || type == star_equal_token || type == slash_equal_token || type == percent_equal_token || type == and_equal_token || type == or_equal_token || type == xor_equal_token || type == shift_left_equal_token || type == shift_right_equal_token){
        opType op;
        switch(type){
            case equal_token:
                op = assignment_op;
                break;
            case plus_equal_token:
                op = plus_assignment_op;
                break;
            case minus_equal_token:
                op = minus_assignment_op;
                break;
            case star_equal_token:
                op = star_assignment_op;
                break;
            case slash_equal_token:
                op = slash_assignment_op;
                break;
            case percent_equal_token:
                op = percent_assignment_op;
                break;
            case and_equal_token:
                op = bitwise_and_assignment_op;
                break;
            case or_equal_token:
                op = bitwise_or_assignment_op;
                break;
            case xor_equal_token:
                op = bitwise_xor_assignment_op;
                break;
            case shift_left_equal_token:
                op = shift_left_assignment_op;
                break;
            case shift_right_equal_token:
                op = shift_right_assignment_op;
                break;
            default:
                op = assignment_op;
                break;
        }
        advanceParser(parser);

        astNode *right = parseAssignment(parser);
        left = createAssignmentNode(left, right, op);
    }
    return left;
}

astNode *parseExpression(parser *parser){
    return parseAssignment(parser);
}

astNode *parsePrimary(parser *parser) {
    token token = parser->current;

    if(token.type == self_token){
        astNode *node = createIdentifierNode("self");
        advanceParser(parser);
        return node;
    }

    if(token.type == identifier_token || token.type == const_token || token.type == static_token || token.type == extern_token){
        dataFlags flags = parseFlags(parser);

        if(parser->current.type != identifier_token){
            return NULL;
        }

        char *name = strdup(parser->current.data.identifier);
        advanceParser(parser);

        if(parser->current.type == colon_token){
            advanceParser(parser);

            astNode *type = parseType(parser);
            if(!type){
                free(name);
                return NULL;
            }

            astNode *initializer = NULL;
            if(parser->current.type == equal_token){
                advanceParser(parser);
                initializer = parseExpression(parser);
                if (!initializer) {
                    freeAst(type);
                    free(name);
                    return NULL;
                }
            }

            astNode *node = createDefineNode(type, name, initializer, flags);
            free(name); 
            return node;
        }

        astNode *node = createIdentifierNode(name);
        free(name); 
        return node;
    }

    if(token.type == short_literal_token || token.type == int_literal_token || token.type == long_literal_token || token.type == long_long_literal_token || token.type == float_literal_token || token.type == long_double_literal_token || token.type == string_literal_token || token.type == true_token || token.type == false_token){
        astNode *node = createValueNode(&token.data.properties.value);
        advanceParser(parser);
        return node;
    }

    if(token.type == null_literal_token){
        dataValue null_val;
        null_val.type = type_null;
        
        astNode *node = createValueNode(&null_val);
        advanceParser(parser);
        return node;
    }

    if(token.type == l_paren_token){
        advanceParser(parser);
        astNode *expr = parseExpression(parser);
        if(parser->current.type != r_paren_token){
            if (expr) freeAst(expr);
            return NULL;
        }
        advanceParser(parser);
        return expr;
    }

    if(token.type == l_bracket_token){
        advanceParser(parser);
        astNode **elements = NULL;
        int count = 0;

        if(parser->current.type != r_bracket_token){
            while(1){
                astNode *expr = parseExpression(parser);
                if(!expr){
                    for(int i = 0; i < count; i++) freeAst(elements[i]);
                    free(elements);
                    return NULL;
                }

                astNode **tmp = realloc(elements, sizeof(astNode *) * (count + 1));
                if(!tmp){
                    for(int i = 0; i < count; i++) freeAst(elements[i]);
                    free(elements);
                    return NULL;
                }
                elements = tmp;
                elements[count++] = expr;

                if(parser->current.type == comma_token){
                    advanceParser(parser);
                } else {
                    break;
                }
            }
        }
        if(parser->current.type != r_bracket_token){
            for(int i = 0; i < count; i++) freeAst(elements[i]);
            free(elements);
            return NULL;
        }
        advanceParser(parser);

        astNode *body = createBodyNode(elements, count);
        free(elements);

        return createArrayNode(NULL, NULL, body);
    }
    return NULL;
}

astNode *parsePostfix(parser *parser){
    astNode *expr = parsePrimary(parser);
    
    while(parser->current.type == increment_token || parser->current.type == decrement_token || parser->current.type == l_paren_token || parser->current.type == dot_token || parser->current.type == arrow_token){
        if(parser->current.type == l_paren_token){
            advanceParser(parser);

            astNode **args = NULL;
            int count = 0;

            if(parser->current.type != r_paren_token){
                while(1){
                    astNode *arg = parseExpression(parser);
                    if(!arg){
                        for(int i = 0; i < count; i++) freeAst(args[i]);
                        free(args);
                        return NULL;
                    }

                    astNode **tmp = realloc(args, sizeof(astNode *) * (count + 1));
                    if(!tmp){
                        for(int i = 0; i < count; i++) freeAst(args[i]);
                        free(args);
                        return NULL;
                    }

                    args = tmp;
                    args[count++] = arg;

                    if(parser->current.type == comma_token){
                        advanceParser(parser);
                    } else {
                        break;
                    }
                }
            }
            if(parser->current.type != r_paren_token){
                for(int i = 0; i < count; i++) freeAst(args[i]);
                free(args);
                return NULL;
            }
            advanceParser(parser);

            astNode *args_node = createBodyNode(args, count);
            free(args);
            expr = createCallNode(expr, args_node);
        }
        else if(parser->current.type == l_bracket_token){
            advanceParser(parser);

            astNode *index = parseExpression(parser);
            if(!index){
                freeAst(expr);
                return NULL;
            }

            if(parser->current.type != r_bracket_token){
                freeAst(expr);
                freeAst(index);
                return NULL;
            }
            advanceParser(parser);

            expr = createArrayAccessNode(expr, index); 
        }
        else if(parser->current.type == dot_token || parser->current.type == arrow_token){
            token_type op_type = parser->current.type;
            advanceParser(parser);

            if(parser->current.type != identifier_token){
                freeAst(expr);
                return NULL;
            }

            char *member = strdup(parser->current.data.identifier);
            advanceParser(parser);

            if (op_type == arrow_token) {
                expr = createArrowAccessNode(expr, member);
            } else {
                expr = createDotAccessNode(expr, member);
            }
            free(member);
        }
        else {
            token op = parser->current;
            advanceParser(parser);
            opType operation;
            switch(op.type){
                case decrement_token:
                    operation = decrement_op;
                    break;
                case increment_token:
                    operation = increment_op;
                    break;
                default:
                    return expr;
            }
            expr = createDataOperationNode(expr, NULL, operation);
        }
    }
    return expr;
}

static token_type peekNextTokenType(parser *parser){
    lexer temp_lexer = *parser->lexer;
    token next = nextToken(&temp_lexer);
    token_type type = next.type;
    freeToken(&next);
    return type;
}

astNode *parseUnary(parser *parser){
    token_type current_type = parser->current.type;

    if(current_type == l_paren_token){
        token_type next_type = peekNextTokenType(parser);

        if(isTypeToken(next_type)){
            advanceParser(parser);
            astNode *type = parseType(parser);

            if(parser->current.type == r_paren_token){
                advanceParser(parser);
            }

            astNode *operand = parseUnary(parser); 
            return createCastNode(type, operand);
        }
    }
    if(current_type == not_token || current_type == minus_token || current_type == decrement_token || current_type == plus_token || current_type == increment_token || current_type == star_token || current_type == address_token || current_type == sizeof_token || current_type == bitwise_not_token){
        token op = parser->current;
        advanceParser(parser);

        if(op.type == sizeof_token){
            astNode *operand = NULL;
            if(parser->current.type == l_paren_token){
                advanceParser(parser);
                if(isTypeToken(parser->current.type)){
                    operand = parseType(parser);
                } else {
                    operand = parseExpression(parser);
                }
                if (parser->current.type == r_paren_token) {
                    advanceParser(parser);
                }
            } else {
                operand = parseUnary(parser);
            }
            return createSizeofNode(operand);
        }

        astNode *right = parseUnary(parser);
        opType operation;

        switch(op.type){
            case not_token: 
                operation = not_op;
                break;
            case minus_token:
                operation = minus_op;
                break;
            case decrement_token:
                operation = decrement_op;
                break;
            case plus_token:
                operation = plus_op;
                break;
            case increment_token:
                operation = increment_op;
                break;
            case star_token:
                operation = dereference_op;
                break;
            case address_token:
                operation = address_op;
                break;
            case bitwise_not_token:
                operation = bitwise_not_op;
                break;
            default:
                return parsePostfix(parser);
        }

        return createDataOperationNode(NULL, right, operation);
    }
    return parsePostfix(parser);
}

astNode *parseMultiplicative(parser *parser){
    astNode *left = parseUnary(parser);

    while(parser->current.type == star_token || parser->current.type == slash_token || parser->current.type == percent_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseUnary(parser);
        if(!right){
            freeAst(left);
            return NULL;
        }

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
    if(!left) return NULL;

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
    astNode *left = parseShift(parser);

    while(parser->current.type == less_token || parser->current.type == less_equal_token || parser->current.type == greater_token || parser->current.type == greater_equal_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseShift(parser);

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
    astNode *left = parseBitwiseOr(parser);

    while(parser->current.type == and_token){
        advanceParser(parser);

        astNode *right = parseBitwiseOr(parser);
        left = createDataOperationNode(left, right, and_op);
    }
    return left;
}

astNode *parseLogicalOr(parser *parser){
    astNode *left = parseLogicalAnd(parser);

    while(parser->current.type == or_token){
        advanceParser(parser);

        astNode *right = parseLogicalAnd(parser);
        left = createDataOperationNode(left, right, or_op);
    }
    return left;
}

astNode *parseShift(parser *parser){
    astNode *left = parseAdditive(parser);

    while(parser->current.type == shift_left_token || parser->current.type == shift_right_token){
        token op = parser->current;
        advanceParser(parser);

        astNode *right = parseAdditive(parser);
        opType operation = (op.type == shift_left_token) ? shift_left_op : shift_right_op;
        
        left = createDataOperationNode(left, right, operation);
    }
    return left;
}

astNode *parseBitwiseAnd(parser *parser){
    astNode *left = parseEquality(parser);

    while(parser->current.type == address_token){
        advanceParser(parser);

        astNode *right = parseEquality(parser);
        left = createDataOperationNode(left, right, bitwise_and_op);
    }
    return left;
}

astNode *parseBitwiseXor(parser *parser){
    astNode *left = parseBitwiseAnd(parser);

    while(parser->current.type == bitwise_xor_token){
        advanceParser(parser);

        astNode *right = parseBitwiseAnd(parser);
        left = createDataOperationNode(left, right, bitwise_xor_op);
    }
    return left;
}

astNode *parseBitwiseOr(parser *parser){
    astNode *left = parseBitwiseXor(parser);

    while(parser->current.type == bitwise_or_token){
        advanceParser(parser);

        astNode *right = parseBitwiseXor(parser);
        left = createDataOperationNode(left, right, bitwise_or_op);
    }
    return left;
}

astNode *parseReturnStatement(parser *parser){
    advanceParser(parser);
    astNode *value = NULL;
    
    if(parser->current.type != semicolon_token){
        value = parseExpression(parser);
    }

    if(parser->current.type != semicolon_token) return NULL;
    advanceParser(parser);
    return createReturnNode(value);
}

astNode *parseContinueStatement(parser *parser){
    advanceParser(parser);
    if(parser->current.type != semicolon_token) return NULL;
    advanceParser(parser);
    return createContinueNode();
}

astNode *parseBreakStatement(parser *parser){
    advanceParser(parser);
    if(parser->current.type != semicolon_token) return NULL;
    advanceParser(parser);
    return createBreakNode();
}

astNode *parseBody(parser *parser){
    if(parser->current.type != l_brace_token) return NULL;
    advanceParser(parser);

    astNode **elements = NULL;
    int count = 0;

    while(parser->current.type != r_brace_token && parser->current.type != eof_token){
        astNode *stmt = parseStatement(parser);

        if(!stmt){
            for(int i = 0; i < count; i++) freeAst(elements[i]);
            free(elements);
            return NULL;
        }

        astNode **tmp = realloc(elements, sizeof(astNode *) * (count + 1));
        
        if(!tmp){
            for(int i = 0; i < count; i++) freeAst(elements[i]);
            free(elements);
            return NULL;
        }

        elements = tmp;
        elements[count++] = stmt;
    }

    if(parser->current.type != r_brace_token){
        for(int i = 0; i < count; i++) freeAst(elements[i]);
        free(elements);
        return NULL;
    }

    advanceParser(parser);
    astNode *body = createBodyNode(elements, count);
    free(elements);
    return body;
}

astNode *parseIfStatement(parser *parser){
    if(parser->current.type != if_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != l_paren_token) return NULL;
    advanceParser(parser);

    astNode *condition = parseExpression(parser);
    if(!condition) return NULL;

    if(parser->current.type != r_paren_token){
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    astNode *then_branch = parseStatement(parser);
    if(!then_branch){
        freeAst(condition);
        return NULL;
    }

    astNode *else_branch = NULL;
    if(parser->current.type == else_token){
        advanceParser(parser);
        else_branch = parseStatement(parser);
        if(!else_branch){
            freeAst(condition);
            freeAst(then_branch);
            return NULL;
        }
    }

    return createIfNode(condition, then_branch, else_branch);
}

astNode *parseSwitchStatement(parser *parser){
    if(parser->current.type != switch_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != l_paren_token) return NULL;
    advanceParser(parser);

    astNode *condition = parseExpression(parser);
    if(!condition) return NULL;

    if(parser->current.type != r_paren_token){
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    astNode *body = parseStatement(parser);
    if(!body){
        freeAst(condition);
        return NULL;
    }

    return createSwitchNode(condition, body);
}

astNode *parseCaseStatement(parser *parser){
    if(parser->current.type != case_token) return NULL;
    advanceParser(parser);

    astNode *value = parseExpression(parser);
    if(!value) return NULL;

    if(parser->current.type != colon_token){
        freeAst(value);
        return NULL;
    }
    advanceParser(parser);

    return createCaseNode(value);
}

astNode *parseDefaultStatement(parser *parser){
    if(parser->current.type != default_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != colon_token) return NULL;
    advanceParser(parser);

    return createDefaultNode();
}

astNode *parseForStatement(parser *parser){
    if(parser->current.type != for_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != l_paren_token) return NULL;
    advanceParser(parser);

    astNode *initializer = NULL;
    if(parser->current.type != semicolon_token){
        initializer = parseExpression(parser);
        if(!initializer) return NULL;
    }
    if(parser->current.type != semicolon_token){
        freeAst(initializer);
        return NULL;
    }
    advanceParser(parser);

    astNode *condition = NULL;
    if(parser->current.type != semicolon_token){
        condition = parseExpression(parser);
        if(!condition){
            freeAst(initializer);
            return NULL;
        }
    }
    if(parser->current.type != semicolon_token){
        freeAst(initializer);
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    astNode *increment = NULL;
    if(parser->current.type != r_paren_token){
        increment = parseExpression(parser);
        if(!increment){
            freeAst(initializer);
            freeAst(condition);
            return NULL;
        }
    }
    if(parser->current.type != r_paren_token){
        freeAst(initializer);
        freeAst(condition);
        freeAst(increment);
        return NULL;
    }
    advanceParser(parser);

    astNode *then_branch = parseStatement(parser);
    if(!then_branch){
        freeAst(initializer);
        freeAst(condition);
        freeAst(increment);
        return NULL;
    }
    return createForNode(initializer, condition, increment, then_branch);
}

astNode *parseWhileStatement(parser *parser){
    if(parser->current.type != while_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != l_paren_token) return NULL;
    advanceParser(parser);

    astNode *condition = parseExpression(parser);
    if(!condition) return NULL;

    if(parser->current.type != r_paren_token){
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    astNode *then_branch = parseStatement(parser);
    if(!then_branch){
        freeAst(condition);
        return NULL;
    }

    return createWhileNode(condition, then_branch);
}

astNode *parseDoWhileStatement(parser *parser){
    if(parser->current.type != do_token) return NULL;
    advanceParser(parser);

    astNode *body = parseStatement(parser);
    if(!body) return NULL;

    if(parser->current.type != while_token){
        freeAst(body);
        return NULL;
    }
    advanceParser(parser);

    if(parser->current.type != l_paren_token){
        freeAst(body);
        return NULL;
    }
    advanceParser(parser);

    astNode *condition = parseExpression(parser);
    if(!condition){
        freeAst(body);
        return NULL;
    }

    if(parser->current.type != r_paren_token){
        freeAst(body);
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    if(parser->current.type != semicolon_token){
        freeAst(body);
        freeAst(condition);
        return NULL;
    }
    advanceParser(parser);

    return createDoWhileNode(body, condition);
}

astNode *parseImportStatement(parser *parser){
    if(parser->current.type != import_token) return NULL;
    advanceParser(parser);

    if(parser->current.type != string_literal_token) return NULL;
    
    char *import_name = parser->current.data.properties.value.value.str_value;
    astNode *name_node = createIdentifierNode(import_name);
    
    advanceParser(parser);

    if(parser->current.type == semicolon_token) advanceParser(parser);
    
    return createImportNode(name_node); 
}

astNode *parseFunction(parser *parser){
    if(parser->current.type != function_token) return NULL;
    advanceParser(parser);

    dataFlags flags = parseFlags(parser);

    if(parser->current.type != identifier_token) return NULL;
    char *func_name = strdup(parser->current.data.identifier);
    advanceParser(parser);

    if(parser->current.type != l_paren_token) return NULL;
    advanceParser(parser);

    int is_variadic = 0;
    astNode *params = parseParamList(parser, &is_variadic);

    if(parser->current.type != r_paren_token){
        free(func_name);
        if(params) freeAst(params);
        return NULL;
    }
    advanceParser(parser);

    astNode *return_type = parseType(parser);

    if(parser->current.type == arrow_token){
        advanceParser(parser);
        return_type = parseType(parser);
    } else {
        return_type = createIdentifierNode("void");
    }

    astNode *body = NULL;

    if(parser->current.type == l_brace_token){
        body = parseBody(parser);
    } else if(parser->current.type == semicolon_token){
        advanceParser(parser);
    }

    astNode *node = createFunctionNode(func_name, return_type, params, body, flags, is_variadic);
    free(func_name);
    return node;
}

astNode *parseParamList(parser *parser, int *is_variadic){
    if(parser->current.type == r_paren_token){
        return createBodyNode(NULL, 0);
    }

    astNode **params = NULL;
    int count = 0;

    while(parser->current.type != r_paren_token && parser->current.type != eof_token){
        astNode *param_node = NULL;

        if(parser->current.type == ellipsis_token){
            *is_variadic = 1;
            advanceParser(parser);
            break;
        }
        if(parser->current.type == self_token){
            char *param_name = strdup("self");
            advanceParser(parser);

            astNode *param_type = createIdentifierNode("self");
            param_node = createDefineNode(param_type, param_name, NULL, 0);
            free(param_name);
        } else {
            if(parser->current.type != identifier_token) {
                for(int i = 0; i < count; i++) freeAst(params[i]);
                free(params);
                return NULL;
            }

            char *param_name = strdup(parser->current.data.identifier);
            advanceParser(parser);

            if(parser->current.type != colon_token) {
                free(param_name);
                for(int i = 0; i < count; i++) freeAst(params[i]);
                free(params);
                return NULL;
            }
            advanceParser(parser);

            astNode *param_type = parseType(parser);
            
            if(!param_type) {
                free(param_name);
                for(int i = 0; i < count; i++) freeAst(params[i]);
                free(params);
                return NULL;
            }

            param_node = createDefineNode(param_type, param_name, NULL, 0);
            free(param_name);
        }

        astNode **tmp = realloc(params, sizeof(astNode *) * (count + 1));
        
        if(!tmp) {
            freeAst(param_node); 
            for(int i = 0; i < count; i++) freeAst(params[i]);
            free(params);
            return NULL;
        }
        
        params = tmp;
        params[count++] = param_node;

        if(parser->current.type == comma_token){
            advanceParser(parser);
        } else {
            break;
        }
    }
    
    astNode *body = createBodyNode(params, count);
    free(params);
    
    return body;
}

astNode *parseType(parser *parser){
    astNode *type_node = NULL;
    token t = parser->current;

    if(t.type == struct_token){
        type_node = parseStruct(parser);
    } 
    else if(t.type == union_token){
        type_node = parseUnion(parser);
    } 
    else if(t.type == enum_token){
        type_node = parseEnum(parser);
    }
    else if(t.type == typeof_token){
        advanceParser(parser);
        
        if(parser->current.type == l_paren_token){
            advanceParser(parser);
            astNode *operand = NULL;
            
            if(isTypeToken(parser->current.type)){
                operand = parseType(parser);
            } else {
                operand = parseExpression(parser);
            }
            
            if(parser->current.type == r_paren_token){
                advanceParser(parser);
            }
            
            type_node = createTypeofNode(operand);
        } else {
            return NULL;
        }
    }

    else if(t.type == void_token || t.type == short_token || t.type == int_token || t.type == float_token || t.type == double_token || t.type == string_token || t.type == bool_token || t.type == ushort_token || t.type == uint_token || t.type == ulong_token || t.type == ullong_token || t.type == identifier_token){
        type_node = createIdentifierNode(t.lexeme);
        advanceParser(parser);
    }

    else if(t.type == long_token){
        advanceParser(parser);
        
        if (parser->current.type == long_token) {
            type_node = createIdentifierNode("long long");
            advanceParser(parser);
        } 
        else if (parser->current.type == double_token) {
            type_node = createIdentifierNode("long double");
            advanceParser(parser);
        } 
        else {
            type_node = createIdentifierNode("long");
        }
    }

    if(type_node){
        while(parser->current.type == star_token){
            advanceParser(parser);
            type_node = createPointerNode(type_node);
        }

        while(parser->current.type == l_bracket_token){
            advanceParser(parser);
            astNode *size = NULL;

            if(parser->current.type != r_bracket_token) size = parseExpression(parser);
            if(parser->current.type == r_bracket_token){
                advanceParser(parser);
            } else {
                freeAst(type_node);
                if(size) freeAst(size);
                return NULL;
            }

            type_node = createArrayNode(type_node, size, NULL);
        }
        return type_node;
    }
    return NULL;
}

astNode *parseStruct(parser *parser){
    advanceParser(parser);

    char *name = NULL;
    if(parser->current.type == identifier_token){
        name = strdup(parser->current.data.identifier);
        advanceParser(parser);
    }

    if(parser->current.type != l_brace_token){
        astNode *node = createStructNode(name, NULL);
        if(name) free(name);
        return node;
    }

    astNode *body = parseBody(parser);
    astNode *node = createStructNode(name, body);
    if(name) free(name);
    return node;
}

astNode *parseImplStatement(parser *parser){
    advanceParser(parser);

    if(parser->current.type != identifier_token) return NULL;

    char *first_id = strdup(parser->current.data.identifier);
    advanceParser(parser);

    char *trait_name = NULL;
    char *target = NULL;

    if(parser->current.type == for_token){
        trait_name = first_id;
        advanceParser(parser);

        if(parser->current.type != identifier_token){
            free(trait_name);
            return NULL;
        }
        target = strdup(parser->current.data.identifier);
        advanceParser(parser);
    } else {
        target = first_id;
    }

    astNode *body = parseBody(parser);

    if(!body){
        if(trait_name) free(trait_name);
        if(target) free(target);
        return NULL;
    }

    astNode *node = createImplNode(trait_name, target, body);
    
    if(trait_name) free(trait_name);
    if(target) free(target);

    return node;
}

astNode *parseTraitStatement(parser *parser){
    advanceParser(parser);

    char *name = NULL;
    if(parser->current.type == identifier_token){
        name = strdup(parser->current.data.identifier);
        advanceParser(parser);
    }

    if(parser->current.type != l_brace_token){
        astNode *node = createTraitNode(name, NULL);
        if(name) free(name);
        return node;
    }

    astNode *body = parseBody(parser);
    astNode *node = createTraitNode(name, body);
    
    if(name) free(name);
    return node;
}

astNode *parseUnion(parser *parser){
    advanceParser(parser);

    char *name = NULL;
    if(parser->current.type == identifier_token){
        name = strdup(parser->current.data.identifier);
        advanceParser(parser);
    }

    if(parser->current.type != l_brace_token){
        astNode *node = createUnionNode(name, NULL);
        if(name) free(name);
        return node;
    }

    astNode *body = parseBody(parser);
    astNode *node = createUnionNode(name, body);
    if(name) free(name);
    return node;
}

astNode *parseEnum(parser *parser){
    advanceParser(parser);

    char *name = NULL;
    if(parser->current.type == identifier_token){
        name = strdup(parser->current.data.identifier);
        advanceParser(parser);
    }

    if(parser->current.type != l_brace_token) { 
        astNode *node = createEnumNode(name, NULL);
        if(name) free(name);
        return node; 
    }
    advanceParser(parser);

    astNode **elements = NULL;
    int count = 0;

    while(parser->current.type != r_brace_token && parser->current.type != eof_token){
        if(parser->current.type != identifier_token) break;
        
        char *enum_id = strdup(parser->current.data.identifier);
        advanceParser(parser);

        astNode *initializer = NULL;
        if(parser->current.type == equal_token){
            advanceParser(parser);
            initializer = parseExpression(parser);
        }

        astNode *type_node = createIdentifierNode("int");
        astNode *member_node = createDefineNode(type_node, enum_id, initializer, const_flag);
        free(enum_id);

        astNode **tmp = realloc(elements, sizeof(astNode *) * (count + 1));
        if(!tmp){ 
            for(int i = 0; i < count; i++) freeAst(elements[i]);
            free(elements);
            if(name) free(name);
            freeAst(member_node);
            return NULL; 
        }

        elements = tmp;
        elements[count++] = member_node;

        if(parser->current.type == comma_token){
            advanceParser(parser);
        } else {
            break;
        }
    }

    if(parser->current.type != r_brace_token){
        for(int i = 0; i < count; i++) freeAst(elements[i]);
        free(elements);
        if(name) free(name);
        return NULL;
    }
    advanceParser(parser);

    astNode *body = createBodyNode(elements, count);
    astNode *node = createEnumNode(name, body);

    free(elements);
    if(name) free(name);
    return node;
}

astNode *parseTypedef(parser *parser) {
    advanceParser(parser);

    astNode *target_type = parseType(parser);
    if(!target_type) return NULL;

    if(parser->current.type != identifier_token){
        freeAst(target_type);
        return NULL;
    }

    char *alias_name = strdup(parser->current.data.identifier);
    advanceParser(parser);

    if(parser->current.type == semicolon_token){
        advanceParser(parser);
    }

    astNode *node = createTypedefNode(target_type, alias_name);
    free(alias_name);
    
    return node;
}

dataFlags parseFlags(parser *parser) {
    dataFlags flags = 0;
    while (parser->current.type == const_token || parser->current.type == static_token || parser->current.type == extern_token) {
        if (parser->current.type == const_token) {
            flags |= const_flag;
        } else if (parser->current.type == static_token) {
            flags |= static_flag;
        } else if (parser->current.type == extern_token) {
            flags |= extern_flag;
        }
        advanceParser(parser);
    }
    return flags;
}

static int isTypeToken(token_type type){
    return type == int_token || type == short_token || type == long_token || type == float_token || type == double_token || type == void_token || type == bool_token || type == string_token || type == struct_token || type == union_token || type == enum_token || type == typeof_token;
}

astNode *parseStatement(parser *parser){
    while (parser->current.type == semicolon_token) {
        advanceParser(parser);
    }

    if (parser->current.type == r_brace_token || parser->current.type == eof_token) {
        return NULL;
    }
    switch(parser->current.type){
        case struct_token:
            return parseStruct(parser);
        case enum_token:
            return parseEnum(parser);
        case union_token:
            return parseUnion(parser);
        case typedef_token:
            return parseTypedef(parser);
        case impl_token:
            return parseImplStatement(parser);
        case trait_token:
            return parseTraitStatement(parser);
        case function_token:
            return parseFunction(parser);
        case return_token:
            return parseReturnStatement(parser);
        case continue_token:
            return parseContinueStatement(parser);
        case break_token:
            return parseBreakStatement(parser);
        case l_brace_token:
            return parseBody(parser);
        case if_token:
            return parseIfStatement(parser);
        case switch_token:
            return parseSwitchStatement(parser);
        case case_token:
            return parseCaseStatement(parser);
        case default_token:
            return parseDefaultStatement(parser);
        case for_token:
            return parseForStatement(parser);
        case while_token:
            return parseWhileStatement(parser);
        case do_token:
            return parseDoWhileStatement(parser);
        case import_token:
            return parseImportStatement(parser);
        default: {
            astNode *expr = parseExpression(parser);
            if(!expr) return NULL;

            if(parser->current.type != semicolon_token){
                freeAst(expr);
                return NULL;
            }

            advanceParser(parser);
            return expr;
        }
    }
}