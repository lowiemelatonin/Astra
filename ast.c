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
    
    if(!node->pointer.ptr) return NULL;
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