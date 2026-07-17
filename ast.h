#ifndef AST_H
#define AST_H

typedef enum {
    type_void,
    type_bool,
    type_short,
    type_ushort,
    type_int,
    type_uint,
    type_long,
    type_ulong,
    type_long_long,
    type_ullong,
    type_float,
    type_double,
    type_long_double,
    type_string,
    type_null
} dataType;

typedef struct {
    dataType type;
    union {
        int b_value;
        short s_value;
        unsigned short us_value;
        int i_value;
        unsigned int ui_value;
        long l_value;
        unsigned long ul_value;
        long long ll_value;
        unsigned long long ull_value;
        float f_value;
        double d_value;
        long double ld_value;
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

    bitwise_and_op,
    bitwise_or_op,
    bitwise_xor_op,
    bitwise_not_op,
    shift_left_op,
    shift_right_op,

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
    plus_assignment_op,
    minus_assignment_op,
    star_assignment_op,
    slash_assignment_op,
    percent_assignment_op,

    bitwise_and_assignment_op,
    bitwise_or_assignment_op,
    bitwise_xor_assignment_op,
    shift_left_assignment_op,
    shift_right_assignment_op,
    
    dereference_op,
    address_op
} opType;

typedef enum {
    const_flag = 1 << 0,
    static_flag = 1 << 1,
    extern_flag = 1 << 2,
    volatile_flag = 1 << 3
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
    switch_node,
    case_node,
    default_node,
    for_node,
    while_node,
    do_while_node,
    break_node,
    continue_node,
    return_node,
    import_node,
    trait_node,
    impl_node,
    struct_node,
    dot_access_node,
    arrow_access_node,
    enum_node,
    union_node,
    sizeof_node,
    typeof_node,
    cast_node,
    typedef_node
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
            astNode *array;
            astNode *index;
        } array_access;

        struct {
            char *identifier;
            astNode *return_type;
            astNode *params;
            astNode *body;
            dataFlags flags;
            int is_variadic;
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
            astNode *condition;
            astNode *body;
        } switch_stmt;

        struct {
            astNode *value;
        } case_stmt;

        struct {
            astNode *initializer;
            astNode *condition;
            astNode *increment;
            astNode *then_branch;
        } for_stmt;

        struct {
            astNode *condition;
            astNode *then_branch;
        } while_stmt;

        struct {
            astNode *body;
            astNode *condition;
        } do_while_stmt;

        struct {
            astNode *value;
        } return_stmt;

        struct {
            astNode *identifier;
        } import_stmt;

        struct {
            char *identifier;
            astNode *body;
        } struct_stmt;

        struct {
            char *trait_name;
            char *target;
            astNode *body;
        } impl_stmt;

        struct {
            char *identifier;
            astNode *body;
        } trait_stmt;

        struct {
            astNode *object;
            char *member;
        } dot_access;

        struct {
            astNode *object;
            char *member;
        } arrow_access;

        struct {
            char *identifier;
            astNode *body;
        } enum_stmt;

        struct {
            char *identifier;
            astNode *body;
        } union_stmt;

        struct {
            astNode *operand;
        } sizeof_expr;

        struct {
            astNode *operand;
        } typeof_expr;

        struct {
            astNode *type;
            astNode *operand;
        } cast_expr;

        struct {
            astNode *type;
            char *alias_name;
        } typedef_stmt;
    };
} astNode;

astNode *createIdentifierNode(char *name);
astNode *createValueNode(dataValue *value);
astNode *createAssignmentNode(astNode *left, astNode *right, opType op);
astNode *createDefineNode(astNode *type, char *identifier, astNode *initializer, dataFlags flags);
astNode *createPointerNode(astNode *pointer);
astNode *createBodyNode(astNode **elements, int elements_count);
astNode *createArrayNode(astNode *type, astNode *size, astNode *elements);
astNode *createArrayAccessNode(astNode *array, astNode *index);
astNode *createFunctionNode(char *identifier, astNode *return_type, astNode *params, astNode *body, dataFlags flags, int is_variadic);
astNode *createCallNode(astNode *identifier, astNode *args);
astNode *createDataOperationNode(astNode *left, astNode *right, opType op);
astNode *createIfNode(astNode *condition, astNode *then_branch, astNode *else_branch);
astNode *createSwitchNode(astNode *condition, astNode *body);
astNode *createCaseNode(astNode *value);
astNode *createDefaultNode();
astNode *createForNode(astNode *initializer, astNode *condition, astNode *increment, astNode *then_branch);
astNode *createWhileNode(astNode *condition, astNode *then_branch);
astNode *createDoWhileNode(astNode *body, astNode *condition);
astNode *createBreakNode();
astNode *createContinueNode();
astNode *createReturnNode(astNode *value);
astNode *createImportNode(astNode *identifier);
astNode *createStructNode(char *identifier, astNode *body);
astNode *createImplNode(char *trait_name, char *target, astNode *body);
astNode *createTraitNode(char *identifier, astNode *body);
astNode *createDotAccessNode(astNode *object, char *member);
astNode *createArrowAccessNode(astNode *object, char *member);
astNode *createEnumNode(char *identifier, astNode *body);
astNode *createUnionNode(char *identifier, astNode *body);
astNode *createSizeofNode(astNode *operand);
astNode *createTypeofNode(astNode *operand);
astNode *createCastNode(astNode *type, astNode *operand);
astNode *createTypedefNode(astNode *type, char *alias_name);

void printAst(astNode *node, int level);
void freeAst(astNode *node);

#endif