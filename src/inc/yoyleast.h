#pragma once

typedef struct yoylestate_t yoylestate_t;

typedef enum {
    NODE_INT_LITERAL,
    NODE_STRING_LITERAL,
    NODE_VARNAME_LITERAL,
    NODE_UNARY_EXPR,
    NODE_BINARY_EXPR,
    NODE_ASSIGNMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_LOOP,
    NODE_FUNCTION_CALL,
    NODE_CFUNC,
    NODE_STATEMENT,
    NODE_FUNCTION_DEF,
    NODE_POP_ARG,
    NODE_NIL,
    NODE_RETURN,
} NodeType;

typedef struct astnode_t {
    int refcount;
    NodeType type;
    union {
        int intValue;
        char* stringValue;
        struct astnode_t* nodeValue;
        void (*cFunction)(yoylestate_t*);
        struct {
            struct astnode_t* operand;
            char op;
        } unaryExpr;
        struct {
            struct astnode_t* left;
            struct astnode_t* right;
            char op;
        } binaryExpr;
        struct {
            char* varName;
            struct astnode_t* value;
        } assignment;
        struct {
            struct astnode_t* statement;
            struct astnode_t* nextStatement; 
        } statement;
        struct {
            struct astnode_t* condition;
            struct astnode_t* thenbranch;
            struct astnode_t* elsebranch;
        } ifStmt;
        struct {
            struct astnode_t* condition;
            struct astnode_t* dobranch;
        } whileLoop;
        struct {
            char* funcName;
            int argc;
            struct astnode_t** argv;
        } funcCall;
    };
} astnode_t;

astnode_t* new_literal_int(int value);
astnode_t* new_literal_string(char* value);
astnode_t* new_literal_varname(char* value);
astnode_t* new_nil_value();
astnode_t* new_unary_expr(astnode_t* left, char op);
astnode_t* new_binary_expr(astnode_t* left, astnode_t* right, char op);
astnode_t* new_assignment(char* name, astnode_t* value);
astnode_t* new_statement(astnode_t* statement, astnode_t* nextStatement);
astnode_t* new_function_call(char* value, int argc, astnode_t* argv[]);
astnode_t* new_cfunc(void (*cfunc)(yoylestate_t*));
astnode_t* new_function_def(char* value, astnode_t* firstStatement);
astnode_t* new_if_statement(astnode_t* condition, astnode_t* thenbranch, astnode_t* elsev);
astnode_t* new_while_loop(astnode_t* condition, astnode_t* dobranch);
astnode_t* new_return(astnode_t* value);
astnode_t* new_pop_arg(char* variable);
void astnode_retain(astnode_t* node);
void astnode_release(astnode_t* node);