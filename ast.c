#include "ast.h"
#include <stdlib.h>
#include <string.h>

static astNode *allocNode(nodeType type){
    astNode *node = malloc(sizeof(astNode));
    if(!node) return NULL;

    node->type = type;
    return node;
}

astNode *createIdentifierNode(char *name){
    astNode *node = allocNode(identifier_node);
    node->identifier.name = strdup(name);

    if(!node->identifier.name){
        free(node);
        return NULL;
    }
    return node;
}

astNode *createValueNode(dataValue value){
    astNode *node = allocNode(value_node);
    dataValue *val = &node->data.value;

    if(value.type == type_string){
        val->type = type_string;
        val->value.str_value = strdup(value.value.str_value);
        if(!val->value.str_value){
            free(node);
            return NULL;
        }
    } else *val = value;
    return node;
}

astNode *createAssignmentNode(astNode *left, astNode *right, opType op){
    astNode *node = allocNode(assignment_node);
    node->assignment.left = left;
    node->assignment.right = right;
    node->assignment.op = op;
    return node;
}

astNode *createDefineNode(astNode *type, char *identifier, astNode *initializer, dataFlags flags){
    astNode *node = allocNode(define_node);
    node->define.type = type;
    node->define.identifier = strdup(identifier);
    if(!node->define.identifier){
        free(node);
        return NULL;
    }
    node->define.flags = flags;
    return node;
}

astNode *createPointerNode(astNode *ptr){
    astNode *node = allocNode(pointer_node);
    node->pointer.ptr = ptr;
    
    if(!node->pointer.ptr){
        free(node->pointer.ptr);
        return NULL;
    }
    return node;
}

astNode *createBodyNode(astNode **elements, int elements_count){
    astNode *node = allocNode(body_node);

    node->body.elements = malloc(sizeof(astNode *) *elements_count);
    if(!node->body.elements){
        free(node);
        return NULL;
    }
    for(int i = 0; i < elements_count; i++){
        node->body.elements[i] = elements[i];
    }
    node->body.elements_count = elements_count;
    return node;
}

astNode *createArrayNode(astNode *type, astNode *size, astNode *elements){
    astNode *node = allocNode(array_node);
    node->array.type = type;
    node->array.size = size;
    node->array.elements = elements;
    return node;
}

astNode *createArrayAccessNode(char *identifier, astNode *index){
    astNode *node = allocNode(array_access_node);
    node->array_access.identifier = strdup(identifier);
    if(!node->array_access.identifier){
        free(node);
        return NULL;
    }

    node->array_access.index = index;
    return node;
}

astNode *createFunctionNode(char *identifier, astNode *return_type, astNode *params, astNode *body, dataFlags flags){
    astNode *node = allocNode(function_node);
    node->function.identifier = strdup(identifier);
    if(!node->function.identifier){
        free(node);
        return NULL;
    }

    node->function.return_type = return_type;
    node->function.params = params;
    node->function.body = body;
    node->function.flags = flags;
    return node;
}

astNode *createCallNode(astNode *identifier, astNode *args){
    astNode *node = allocNode(call_node);
    node->call.identifier = identifier;
    node->call.args = args;
    return node;
}

astNode *createDataOperationNode(astNode *left, astNode *right, opType op){
    astNode *node = allocNode(data_operation_node);
    node->operation.left = left;
    node->operation.right = right;
    node->operation.op = op;
    return node;
}

astNode *createIfNode(astNode *condition, astNode *then_branch, astNode *else_branch){
    astNode *node = allocNode(if_node);
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

astNode *createForNode(astNode *initializer, astNode *condition, astNode *increment, astNode *then_branch){
    astNode *node = allocNode(for_node);
    node->for_stmt.initializer = initializer;
    node->for_stmt.condition = condition;
    node->for_stmt.increment = increment;
    node->for_stmt.then_branch = then_branch;
    return node;
}

astNode *createBreakNode(){
    astNode *node = allocNode(break_node);
    return node;
}

astNode *createContinueNode(){
    astNode *node = allocNode(continue_node);
    return node;
}

astNode *createReturnNode(astNode *value){
    astNode *node = allocNode(return_node);
    node->return_stmt.value = value;
    return node;
}

astNode *createImportNode(astNode *identifier){
    astNode *node = allocNode(import_node);
    node->import_stmt.identifier = identifier;
    return node;
}

void freeAst(astNode *node){
    if(!node) return;

    switch(node->type){
        case identifier_node:
            free(node->identifier.name);
            break;
        case value_node:
            if(node->data.value.type == type_string){
                free(node->data.value.value.str_value);
            }
            break;
        case assignment_node:
            freeAst(node->assignment.left);
            freeAst(node->assignment.right);
            break;
        case define_node:
            freeAst(node->define.type);
            free(node->define.identifier);
            freeAst(node->define.initializer);
            break;
        case pointer_node:
            freeAst(node->pointer.ptr);
            break;
        case body_node:
            for(int i = 0; i < node->body.elements_count; i++){
                freeAst(node->body.elements[i]);
            }
            free(node->body.elements);
            break;
        case array_node:
            freeAst(node->array.type);
            freeAst(node->array.size);
            freeAst(node->array.elements);
            break;
        case array_access_node:
            free(node->array_access.identifier);
            freeAst(node->array_access.index);
            break;
        case function_node:
            free(node->function.identifier);
            freeAst(node->function.return_type);
            freeAst(node->function.params);
            freeAst(node->function.body);
            break;
        case call_node:
            freeAst(node->call.identifier);
            freeAst(node->call.args);
            break;
        case data_operation_node:
            freeAst(node->operation.left);
            freeAst(node->operation.right);
            break;
        case if_node:
            freeAst(node->if_stmt.condition);
            freeAst(node->if_stmt.then_branch);
            freeAst(node->if_stmt.else_branch);
            break;
        case for_node:
            freeAst(node->for_stmt.initializer);
            freeAst(node->for_stmt.condition);
            freeAst(node->for_stmt.increment);
            freeAst(node->for_stmt.then_branch);
            break;
        case break_node:
            break;
        case continue_node:
            break;
        case return_node:
            freeAst(node->return_stmt.value);
            break;
        case import_node:
            freeAst(node->import_stmt.identifier);
            break;
        default:
            break;
    }
    free(node);
}