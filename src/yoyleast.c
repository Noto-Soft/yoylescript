#include "inc/yoyleast.h"
#include "inc/yoylei.h"
#include "inc/yoyleerr.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

static astnode_t* create_node() {
    astnode_t* node = (astnode_t*)malloc(sizeof(astnode_t));
    if (!node) fatal("Out of memory");
    memset(node, 0, sizeof(astnode_t));
    node->refcount = 1;
    return node;
}

astnode_t* new_literal_int(int value) {
    astnode_t* node = create_node();
    node->type = NODE_INT_LITERAL;
    node->intValue = value;
    return node;
}

astnode_t* new_literal_string(char* value) {
    astnode_t* node = create_node();
    node->type = NODE_STRING_LITERAL;
    node->stringValue = strdup(value);
    return node;
}

astnode_t* new_literal_varname(char* value) {
    astnode_t* node = create_node();
    node->type = NODE_VARNAME_LITERAL;
    node->stringValue = strdup(value);
    return node;
}

astnode_t* new_unary_expr(astnode_t* left, char op) {
    astnode_t* node = create_node();
    node->type = NODE_UNARY_EXPR;
    node->unaryExpr.operand = left;
    node->unaryExpr.op = op;
    return node;
}

astnode_t* new_binary_expr(astnode_t* left, astnode_t* right, char op) {
    astnode_t* node = create_node();
    node->type = NODE_BINARY_EXPR;
    node->binaryExpr.left = left;
    node->binaryExpr.right = right;
    node->binaryExpr.op = op;
    return node;
}

astnode_t* new_if_statement(astnode_t* condition, astnode_t* thenbranch, astnode_t* elsebranch) {
    astnode_t* node = create_node();
    node->type = NODE_IF_STATEMENT;
    node->ifStmt.condition = condition;
    node->ifStmt.thenbranch = thenbranch;
    node->ifStmt.elsebranch = elsebranch;
    return node;
}

astnode_t* new_while_loop(astnode_t* condition, astnode_t* dobranch) {
    astnode_t* node = create_node();
    node->type = NODE_WHILE_LOOP;
    node->whileLoop.condition = condition;
    node->whileLoop.dobranch = dobranch;
    return node;
}

astnode_t* new_assignment(char* name, astnode_t* value) {
    astnode_t* node = create_node();
    node->type = NODE_ASSIGNMENT;
    node->assignment.varName = strdup(name);
    node->assignment.value = value;
    return node;
}

astnode_t* new_statement(astnode_t* statement, astnode_t* nextStatement) {
    astnode_t* node = create_node();
    node->type = NODE_STATEMENT;
    node->statement.statement = statement;
    node->statement.nextStatement = nextStatement;
    return node;
}

astnode_t* new_function_call(char* value, int argc, astnode_t** argv) {
    astnode_t* node = create_node();
    node->type = NODE_FUNCTION_CALL;
    node->funcCall.funcName = strdup(value);
    node->funcCall.argc = argc;
    node->funcCall.argv = malloc(sizeof(astnode_t*) * argc);
    for (int i = 0; i < argc; i++) {
        node->funcCall.argv[i] = argv[i];
        astnode_retain(argv[i]);
    }
    return node;
}

astnode_t* new_cfunc(void (*cfunc)(yoylestate_t*)) {
    astnode_t* node = create_node();
    node->type = NODE_CFUNC;
    node->cFunction = cfunc;
    return node;
}

astnode_t* new_function_def(char* value, astnode_t* firstStatement) {
    astnode_t* node = create_node();
    node->type = NODE_FUNCTION_DEF;
    node->assignment.varName = strdup(value);
    node->assignment.value = firstStatement;
    return node;
}

astnode_t* new_pop_arg(char* variable) {
    astnode_t* node = create_node();
    node->type = NODE_POP_ARG;
    node->stringValue = strdup(variable);
    return node;
}


void astnode_retain(astnode_t* node) {
    if (node) {
        node->refcount++;
    }
}

void astnode_release(astnode_t* node) {
    if (node && --node->refcount == 0) {
        free_ast(node);
    }
}