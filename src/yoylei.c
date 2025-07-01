#include "inc/yoyleast.h"
#include "inc/yoyleerr.h"
#include "inc/yoylei.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

astnode_t* eval(yoylestate_t* state, astnode_t* node) {
    if (node == NULL) {
        fatal("Null nodes passed to eval statement");
    }

    switch (node->type) {
        case NODE_STATEMENT: {
            astnode_t* current = node;
            while (current != NULL && current->type == NODE_STATEMENT) {
                eval(state, current->statement.statement);
                current = current->statement.nextStatement;
            }
            if (current != NULL) {
                return eval(state, current);
            }
            return NULL;
        }
        case NODE_INT_LITERAL:
        case NODE_STRING_LITERAL: return node;
        case NODE_VARNAME_LITERAL: return symbol_table_lookup(&state->symbol_table, node->stringValue);
        case NODE_NIL: return NULL;
        case NODE_UNARY_EXPR: {
            astnode_t* left = eval(state, node->unaryExpr.operand);
            if (left->type == NODE_INT_LITERAL) {
                switch (node->unaryExpr.op) {
                    case '~': return new_literal_int(~(left->intValue));
                    case '!': return new_literal_int(left->intValue == 0);
                    default: fatalf("Invalid unary operator %c", node->unaryExpr.op);
                }
            } else {
                fatal("Cannot perform unary expression on non-integers");
            }
        }
        case NODE_BINARY_EXPR: {
            astnode_t* left = eval(state, node->binaryExpr.left);
            astnode_t* right = eval(state, node->binaryExpr.right);
            if (left->type == NODE_INT_LITERAL && right->type == NODE_INT_LITERAL) {
                switch (node->binaryExpr.op) {
                    case '+': return new_literal_int(left->intValue + right->intValue);
                    case '-': return new_literal_int(left->intValue - right->intValue);
                    case '*': return new_literal_int(left->intValue * right->intValue);
                    case '/': return new_literal_int(left->intValue / right->intValue);
                    case 'e': return new_literal_int(left->intValue == right->intValue);
                    case 'n': return new_literal_int(left->intValue != right->intValue);
                    case 'g': return new_literal_int(left->intValue > right->intValue);
                    case 'l': return new_literal_int(left->intValue < right->intValue);
                    case 'G': return new_literal_int(left->intValue >= right->intValue);
                    case 'L': return new_literal_int(left->intValue <= right->intValue);
                    case 'a': return new_literal_int(left->intValue != 0 && right->intValue != 0);
                    case 'o': return new_literal_int(left->intValue != 0 || right->intValue != 0);
                    case '&': return new_literal_int(left->intValue & right->intValue);
                    case '|': return new_literal_int(left->intValue | right->intValue);
                    default: fatalf("Invalid binary operator %c", node->binaryExpr.op);
                }
            } else {
                fatal("Cannot perform binary expression on non-integers");
            }
        }
        case NODE_ASSIGNMENT:
            symbol_table_set(&state->symbol_table, node->assignment.varName, eval(state, node->assignment.value));
            return NULL;
        case NODE_FUNCTION_DEF: {
            astnode_t* func = node->assignment.value;
            if (func == NULL) {
                fatalf("Cannot assign nothing to a function (%s)", node->assignment.varName);
            }
            if (!(func->type == NODE_STATEMENT)) {
                fatalf("Cannot assign a non-statement to a function (%s)", node->assignment.varName);
            }
            symbol_table_set(&state->symbol_table, node->assignment.varName, node->assignment.value);
            node->assignment.value = NULL;
            return NULL;
        }
        case NODE_FUNCTION_CALL: {
            astnode_t* func = symbol_table_lookup(&state->symbol_table, node->funcCall.funcName);
            if (func == NULL) {
                fatalf("Function does not exist (%s)", node->stringValue);
            }
            for (int i = node->funcCall.argc - 1;i>=0;i--) {
                push_stack(&state->arg_stack, node->funcCall.argv[i]);
            }
            switch (func->type) {
                case NODE_STATEMENT: eval(state, func); break;
                case NODE_CFUNC: func->cFunction(state); break;
                default: fatalf("Cannot run a non-function (%s)", node->funcCall.funcName);
            }
            return NULL;
        }
        case NODE_IF_STATEMENT:
            if (eval(state, node->ifStmt.condition)->intValue != 0) {
                eval(state, node->ifStmt.thenbranch);
            } else if (node->ifStmt.elsebranch) {
                eval(state, node->ifStmt.elsebranch);
            }
            return NULL;
        case NODE_WHILE_LOOP:
            while (eval(state, node->whileLoop.condition)->intValue != 0) {
                eval(state, node->whileLoop.dobranch);
            }
            return NULL;
        case NODE_POP_ARG:
            symbol_table_set(&state->symbol_table, node->stringValue, pop_stack(&state->arg_stack));
            return NULL;
        default: 
            fatalf("Cannot evaluate the node type %d", node->type);
    }
    return NULL;
}

#define PRINTLF(printlf) if (printlf) printf("\n");

void print_astnode(astnode_t* node, bool printlf) {
    if (!node) {
        printf("(nil)");
        PRINTLF(printlf);
        return;
    }

    switch (node->type) {
        case NODE_INT_LITERAL:
            printf("%d", node->intValue);
            PRINTLF(printlf);
            break;

        case NODE_STRING_LITERAL:
            if (node->stringValue) {
                printf("%s", node->stringValue);
                PRINTLF(printlf);
            } else {
                printf("(nil string)");
                PRINTLF(printlf);
            }
            break;

        default:
            printf("(unsupported node type for print)");
            PRINTLF(printlf);
            break;
    }
}

void _free_ast(astnode_t* node) {
    if (!node) return;
    switch(node->type) {
        case NODE_BINARY_EXPR:
            free_ast(node->binaryExpr.left);
            free_ast(node->binaryExpr.right);
            break;
        case NODE_ASSIGNMENT:
        case NODE_FUNCTION_DEF:
            free(node->assignment.varName);
            free_ast(node->assignment.value);
            break;
        case NODE_FUNCTION_CALL:
            free(node->funcCall.funcName);
            break;
        case NODE_STRING_LITERAL:
        case NODE_VARNAME_LITERAL:
        case NODE_POP_ARG:
            free(node->stringValue);
            break;
        case NODE_STATEMENT:
            free_ast(node->statement.statement);
            free_ast(node->statement.nextStatement);
            break;
        default:
            break;
    }
    free(node);
}

astnode_t* symbol_table_lookup(varentry_t** symbol_table, const char* name) {
    varentry_t* current = *symbol_table;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void symbol_table_set(varentry_t** symbol_table, const char* name, astnode_t* value) {
    varentry_t* current = *symbol_table;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            astnode_release(current->value);  
            current->value = value;
            astnode_retain(value);            
            return;
        }
        current = current->next;
    }
    varentry_t* entry = malloc(sizeof(varentry_t));
    entry->name = strdup(name);
    entry->value = value;
    astnode_retain(value);  
    entry->next = *symbol_table;
    *symbol_table = entry;
}


void _free_symbol_table(varentry_t* entry) {
    while (entry) {
        varentry_t* next = entry->next;
        free(entry->name);
        astnode_release(entry->value);
        free(entry);
        entry = next;
    }
}

void push_stack(yoylestack_t** stack, astnode_t* value) {
    yoylestack_t* new_node = (yoylestack_t*)malloc(sizeof(yoylestack_t));
    if (!new_node) {
        perror("malloc failed");
        exit(1);
    }
    new_node->value = value;
    new_node->next = NULL;

    if (*stack == NULL) {
        *stack = new_node;
    } else {
        yoylestack_t* current = *stack;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

astnode_t* pop_stack(yoylestack_t** stack) {
    if (*stack == NULL) {
        return NULL;  
    }

    yoylestack_t* current = *stack;
    yoylestack_t* prev = NULL;

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    astnode_t* value = current->value;

    if (prev == NULL) {
        
        *stack = NULL;
    } else {
        prev->next = NULL;
    }

    free(current);
    return value;
}

void _free_stack(yoylestack_t* stack) {
    while (stack != NULL) {
        yoylestack_t* next = stack->next;
        free(stack);
        stack = next;
    }
}