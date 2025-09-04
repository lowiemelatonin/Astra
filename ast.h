#ifndef AST_H
#define AST_H

typedef enum {
    type_int,
    type_long,
    type_float,
    type_double,
    type_string
} dataType;

typedef struct {
    dataType type;
    union {
        int i_value;
        long l_value;
        float f_value;
        double d_value;
        char* str_value;
    } value;
} dataValue;

typedef enum {
    plus_op,
    increment_op,
    minus_op,
    decrement_op,
    star_op,
    slash_op,
    percent_op,

    and_op,
    or_op,
    not_op,

    equal_op,
    not_equal_op,
    less_op,
    greater_op,
    less_or_equal_op, 
    greater_or_equal_op,

    assignment_op,

    dereference_op,
    address_op
} opType;

typedef enum {
    const_flag = 1 << 0,
    static_flag = 1 << 1
} dataFlags;

typedef enum {
    identifier_node,
    value_node,
    assignment_node,
    define_node,
    pointer_node,
    body_node,
    array_node,
    array_access_node,
    function_node,
    call_node,
    data_operation_node,
    if_node,
    for_node,
    break_node,
    continue_node,
    return_node,
    import_node
} nodeType;

typedef struct astNode astNode;

typedef struct astNode {
    nodeType type;
    union {
        struct {
            char *name;
        } identifier;

        struct {
            dataValue value;
        } data;

        struct {
            astNode *left;
            astNode *right;
            opType op;
        } assignment;

        struct {
            astNode *type;
            char *identifier;
            astNode *initializer;
            dataFlags flags;
        } define;

        struct {
            astNode *ptr;
        } pointer;

        struct {
            astNode **elements;
            int elements_count;
        } body;

        struct {
            astNode *type;
            astNode *size;
            astNode *elements;
        } array;

        struct {
            char *identifier;
            astNode *index;
        } array_access;

        struct {
            char *identifier;
            astNode *return_type;
            astNode *params;
            astNode *body;
            dataFlags flags;
        } function;

        struct {
            astNode *identifier;
            astNode *args;
        } call;

        struct {
            astNode *left;
            astNode *right;
            opType op;
        } operation;

        struct {
            astNode *condition;
            astNode *then_branch;
            astNode *else_branch;
        } if_stmt;

        struct {
            astNode *initializer;
            astNode *condition;
            astNode *increment;
            astNode *then_branch;
        } for_stmt;

        struct {} break_stmt, continue_stmt;

        struct {
            astNode *value;
        } return_stmt;

        struct {
            astNode *identifier;
        } import_stmt;

    };
} astNode;

astNode *createIdentifierNode(char *name);
astNode *createValueNode(dataValue value);
astNode *createAssignmentNode(astNode *left, astNode *right, opType op);
astNode *createDefineNode(astNode *type, char *identifier, astNode *initializer, dataFlags flags);
astNode *createPointerNode(astNode *pointer);
astNode *createBodyNode(astNode **elements, int elements_count);
astNode *createArrayNode(astNode *type, astNode *size, astNode *elements);
astNode *createArrayAccessNode(char *identifier, astNode *index);
astNode *createFunctionNode(char *identifier, astNode *return_type, astNode *params, astNode *body, dataFlags flags);
astNode *createCallNode(astNode *identifier, astNode *args);
astNode *createDataOperationNode(astNode *left, astNode *right, opType op);
astNode *createIfNode(astNode *condition, astNode *then_branch, astNode *else_branch);
astNode *createForNode(astNode *initializer, astNode *condition, astNode *increment, astNode *then_branch);
astNode *createBreakNode();
astNode *createContinueNode();
astNode *createReturnNode(astNode *value);
astNode *createImportNode(astNode *identifier);
void freeAst(astNode *node)

#endif