#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

astNode *createValueNode(dataValue *value){
    astNode *node = allocNode(value_node);
    dataValue *val = &node->data.value;

    if(value->type == type_string){
        val->type = type_string;
        val->value.str_value = strdup(value->value.str_value);
        if(!val->value.str_value){
            free(node);
            return NULL;
        }
    } else *val = *value;
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
    node->define.initializer = initializer;
    node->define.flags = flags;
    return node;
}

astNode *createPointerNode(astNode *ptr){
    astNode *node = allocNode(pointer_node);
    node->pointer.ptr = ptr;
    
    if(!node->pointer.ptr){
        free(node);
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

astNode *createWhileNode(astNode *condition, astNode *then_branch){
    astNode *node = allocNode(while_node);
    node->while_stmt.condition = condition;
    node->while_stmt.then_branch = then_branch;
    return node;
}

astNode *createDoWhileNode(astNode *body, astNode *condition){
    astNode *node = allocNode(do_while_node);
    node->do_while_stmt.body = body;
    node->do_while_stmt.condition = condition;
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

astNode *createStructNode(char *identifier, astNode *body){
    astNode *node = allocNode(struct_node);
    node->struct_stmt.identifier = identifier ? strdup(identifier) : NULL;
    node->struct_stmt.body = body;
    return node;
}

astNode *createDotAccessNode(astNode *object, char *member) {
    astNode *node = allocNode(dot_access_node);
    if (!node) return NULL;
    
    node->dot_access.object = object;
    node->dot_access.member = strdup(member);
    if (!node->dot_access.member) {
        free(node);
        return NULL;
    }
    return node;
}

astNode *createArrowAccessNode(astNode *object, char *member) {
    astNode *node = allocNode(arrow_access_node);
    if (!node) return NULL;
    
    node->arrow_access.object = object;
    node->arrow_access.member = strdup(member);
    if (!node->arrow_access.member) {
        free(node);
        return NULL;
    }
    return node;
}

astNode *createEnumNode(char *identifier, astNode *body){
    astNode *node = allocNode(enum_node);
    node->enum_stmt.identifier = identifier ? strdup(identifier) : NULL;
    node->enum_stmt.body = body;
    return node;
}

astNode *createUnionNode(char *identifier, astNode *body){
    astNode *node = allocNode(union_node);
    node->union_stmt.identifier = identifier ? strdup(identifier) : NULL;
    node->union_stmt.body = body;
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
        case while_node:
            freeAst(node->while_stmt.condition);
            freeAst(node->while_stmt.then_branch);
            break;
        case do_while_node:
            freeAst(node->do_while_stmt.body);
            freeAst(node->do_while_stmt.condition);
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
        case struct_node:
            free(node->struct_stmt.identifier);
            freeAst(node->struct_stmt.body);
            break;
        case dot_access_node:
            freeAst(node->dot_access.object);
            free(node->dot_access.member);
            break;
        case arrow_access_node:
            freeAst(node->arrow_access.object);
            free(node->arrow_access.member);
            break;
        case enum_node:
            free(node->enum_stmt.identifier);
            freeAst(node->enum_stmt.body);
            break;
        case union_node:
            free(node->union_stmt.identifier);
            freeAst(node->union_stmt.body);
            break;
        default:
            break;
    }
    free(node);
}


// Isso aqui nn fui euq fiz, é so pra testar
// Função auxiliar para imprimir a indentação correta baseada no nível da árvore
static void printIndent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  "); // Usa 2 espaços por nível de indentação
    }
}

// Função auxiliar para converter o opType em uma string legível
static const char* getOpString(opType op) {
    switch(op) {
        case plus_op: return "+";
        case increment_op: return "++";
        case minus_op: return "-";
        case decrement_op: return "--";
        case star_op: return "*";
        case slash_op: return "/";
        case percent_op: return "%";
        case and_op: return "&&";
        case or_op: return "||";
        case not_op: return "!";
        case equal_op: return "==";
        case not_equal_op: return "!=";
        case less_op: return "<";
        case greater_op: return ">";
        case less_or_equal_op: return "<=";
        case greater_or_equal_op: return ">=";
        case assignment_op: return "=";
        case dereference_op: return "* (deref)";
        case address_op: return "& (address)";
        default: return "unknown";
    }
}

// Implementação principal da função printAst
void printAst(astNode *node, int level) {
    if (!node) return;

    printIndent(level);

    switch (node->type) {
        case identifier_node:
            printf("Identifier: %s\n", node->identifier.name);
            break;

        case value_node:
            printf("Value: ");
            switch (node->data.value.type) {
                case type_bool:        printf("%s (bool)\n", node->data.value.value.b_value ? "true" : "false"); break;
                case type_short:       printf("%d (short)\n", node->data.value.value.s_value); break;
                case type_int:         printf("%d (int)\n", node->data.value.value.i_value); break;
                case type_long:        printf("%ld (long)\n", node->data.value.value.l_value); break;
                case type_long_long:   printf("%lld (long)\n", node->data.value.value.ll_value); break;
                case type_float:       printf("%f (float)\n", node->data.value.value.f_value); break;
                case type_double:      printf("%lf (double)\n", node->data.value.value.d_value); break;
                case type_long_double: printf("%Lf (long double)\n", node->data.value.value.ld_value); break;
                case type_string:      printf("\"%s\" (string)\n", node->data.value.value.str_value); break;
            }
            break;

        case data_operation_node:
            printf("Operation: '%s'\n", getOpString(node->operation.op));
            if (node->operation.left) {
                printIndent(level + 1); printf("Left:\n");
                printAst(node->operation.left, level + 2);
            }
            if (node->operation.right) {
                printIndent(level + 1); printf("Right:\n");
                printAst(node->operation.right, level + 2);
            }
            break;

        case assignment_node:
            printf("Assignment: '%s'\n", getOpString(node->assignment.op));
            printIndent(level + 1); printf("Left:\n");
            printAst(node->assignment.left, level + 2);
            printIndent(level + 1); printf("Right:\n");
            printAst(node->assignment.right, level + 2);
            break;

        case define_node:
            printf("Define (flags: %d): %s\n", node->define.flags, node->define.identifier);
            printIndent(level + 1); printf("Type:\n");
            printAst(node->define.type, level + 2);
            if (node->define.initializer) {
                printIndent(level + 1); printf("Initializer:\n");
                printAst(node->define.initializer, level + 2);
            }
            break;

        case pointer_node:
            printf("Pointer:\n");
            printAst(node->pointer.ptr, level + 1);
            break;

        case body_node:
            printf("Body (%d elements):\n", node->body.elements_count);
            for (int i = 0; i < node->body.elements_count; i++) {
                printAst(node->body.elements[i], level + 1);
            }
            break;

        case array_node:
            printf("Array:\n");
            printIndent(level + 1); printf("Type:\n");
            printAst(node->array.type, level + 2);
            printIndent(level + 1); printf("Size:\n");
            printAst(node->array.size, level + 2);
            printIndent(level + 1); printf("Elements:\n");
            printAst(node->array.elements, level + 2);
            break;

        case array_access_node:
            printf("Array Access: %s\n", node->array_access.identifier);
            printIndent(level + 1); printf("Index:\n");
            printAst(node->array_access.index, level + 2);
            break;

        case function_node:
            printf("Function: %s (flags: %d)\n", node->function.identifier, node->function.flags);
            printIndent(level + 1); printf("Return Type:\n");
            printAst(node->function.return_type, level + 2);
            printIndent(level + 1); printf("Params:\n");
            printAst(node->function.params, level + 2);
            printIndent(level + 1); printf("Body:\n");
            printAst(node->function.body, level + 2);
            break;

        case call_node:
            printf("Call:\n");
            printIndent(level + 1); printf("Target:\n");
            printAst(node->call.identifier, level + 2);
            printIndent(level + 1); printf("Args:\n");
            printAst(node->call.args, level + 2);
            break;

        case if_node:
            printf("If Statement:\n");
            printIndent(level + 1); printf("Condition:\n");
            printAst(node->if_stmt.condition, level + 2);
            printIndent(level + 1); printf("Then Branch:\n");
            printAst(node->if_stmt.then_branch, level + 2);
            if (node->if_stmt.else_branch) {
                printIndent(level + 1); printf("Else Branch:\n");
                printAst(node->if_stmt.else_branch, level + 2);
            }
            break;

        case for_node:
            printf("For Loop:\n");
            if (node->for_stmt.initializer) {
                printIndent(level + 1); printf("Init:\n");
                printAst(node->for_stmt.initializer, level + 2);
            }
            if (node->for_stmt.condition) {
                printIndent(level + 1); printf("Condition:\n");
                printAst(node->for_stmt.condition, level + 2);
            }
            if (node->for_stmt.increment) {
                printIndent(level + 1); printf("Increment:\n");
                printAst(node->for_stmt.increment, level + 2);
            }
            printIndent(level + 1); printf("Body:\n");
            printAst(node->for_stmt.then_branch, level + 2);
            break;

        case while_node:
            printf("While Loop:\n");
            printIndent(level + 1); printf("Condition:\n");
            printAst(node->while_stmt.condition, level + 2);
            printIndent(level + 1); printf("Body:\n");
            printAst(node->while_stmt.then_branch, level + 2);
            break;

        case do_while_node:
            printf("Do While Loop:\n");
            printIndent(level + 1); printf("Body:\n");
            printAst(node->do_while_stmt.body, level + 2);
            printIndent(level + 1); printf("Condition:\n");
            printAst(node->do_while_stmt.condition, level + 2);
            break;

        case break_node:
            printf("Break Statement\n");
            break;

        case continue_node:
            printf("Continue Statement\n");
            break;

        case return_node:
            printf("Return Statement:\n");
            if (node->return_stmt.value) {
                printAst(node->return_stmt.value, level + 1);
            }
            break;

        case import_node:
            printf("Import Statement:\n");
            printAst(node->import_stmt.identifier, level + 1);
            break;

        case struct_node:
            printf("Struct Definition: %s\n", node->struct_stmt.identifier ? node->struct_stmt.identifier : "Anonymous");
            printIndent(level + 1); printf("Body:\n");
            printAst(node->struct_stmt.body, level + 2);
            break;

        case dot_access_node:
            printf("Dot Member Access: %s\n", node->dot_access.member);
            printIndent(level + 1); printf("Object:\n");
            printAst(node->dot_access.object, level + 2);
            break;

        case arrow_access_node:
            printf("Arrow Member Access: %s\n", node->arrow_access.member);
            printIndent(level + 1); printf("Object:\n");
            printAst(node->arrow_access.object, level + 2);
            break;

        case enum_node:
            printf("Enum Definition: %s\n", node->enum_stmt.identifier ? node->enum_stmt.identifier : "Anonymous");
            printIndent(level + 1); printf("Body:\n");
            printAst(node->enum_stmt.body, level + 2);
            break;

        case union_node:
            printf("Union Definition: %s\n", node->union_stmt.identifier ? node->union_stmt.identifier : "Anonymous");
            printIndent(level + 1); printf("Body:\n");
            printAst(node->union_stmt.body, level + 2);
            break;
            
        default:
            printf("Unknown Node Type (%d)\n", node->type);
            break;
    }
}