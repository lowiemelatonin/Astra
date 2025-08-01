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

astNode *createAssignmentNode(astNode *left, astNode *right, opType op);
astNode *createDefineNode(astNode *type, char *identifier, astNode *initializer, dataFlags flags);

astNode *createPointerNode(astNode *ptr){
    astNode *node = allocNode(pointer_node);
    node->pointer.ptr = ptr;
    
    if(!node->pointer.ptr) return NULL;
    return node;
}

astNode *createBodyNode(astNode **elements, int elements_count);
astNode *createArrayNode(astNode *type, astNode *size, astNode *elements);

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

astNode *createFunctionNode(char *identifier, astNode *return_type, astNode *params, astNode *body, dataFlags flags);

astNode *createCallNode(astNode *identifier, astNode *args){
    astNode *node = allocNode(call_node);
    node->call.identifier = identifier;
    node->call.args = args;
    return node;
}

astNode *createDataOperationNode(astNode *left, astNode *right, opType op);
astNode *createIfNode(astNode *condition, astNode *then_branch, astNode *else_branch);
astNode *createForNode(astNode *initializer, astNode *condition, astNode *increment, astNode *then_branch);

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