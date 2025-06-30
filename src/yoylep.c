#include "inc/yoylep.h"
#include "inc/yoyleerr.h"

#include <string.h>
#include <stdlib.h>

int get_precedence(TokenType type, const char* lexeme) {
    if (type == TOKEN_BINARY_OPERATOR) {
        if (strcmp(lexeme, "*") == 0 || strcmp(lexeme, "/") == 0) return 7;
        if (strcmp(lexeme, "+") == 0 || strcmp(lexeme, "-") == 0) return 6;
        if (strcmp(lexeme, "G") == 0 || strcmp(lexeme, "L") == 0 || strcmp(lexeme, "g") == 0 || strcmp(lexeme, "l") == 0) return 5;
        if (strcmp(lexeme, "e") == 0 || strcmp(lexeme, "n") == 0) return 4; 
        if (strcmp(lexeme, "&") == 0) return 3;
        if (strcmp(lexeme, "|") == 0) return 2;
    }
    return 0;
}

void parser_advance(parser_t* parser) {
    free_token(parser->current);
    parser->current = lexer_next(parser->lexer);
}

void parser_init(parser_t* parser, lexer_t* lexer) {
    parser->lexer = lexer;
    parser->current = lexer_next(lexer);
}

astnode_t* parse_primary(parser_t* parser) {
    token_t tok = parser->current;
    switch (tok.type) {
        case TOKEN_INT: {
            astnode_t* node = new_literal_int(tok.intValue);
            parser_advance(parser);
            return node;
        }
        case TOKEN_STRING: {
            astnode_t* node = new_literal_string(strdup(tok.strValue));
            parser_advance(parser);
            return node;
        }
        case TOKEN_IDENTIFIER: {
            astnode_t* node = new_literal_varname(strdup(tok.lexeme));
            parser_advance(parser);
            return node;
        }
        case TOKEN_LPAREN: {
            parser_advance(parser); 
            astnode_t* expr = parse_expression(parser);
            if (parser->current.type != TOKEN_RPAREN) {
                fatal("Expected ')' after expression");
            }
            parser_advance(parser); 
            return expr;
        }
        default:
            fatalf("Unexpected token in primary expression: %s", tok.lexeme ? tok.lexeme : "(null)");
            return NULL;
    }
}

astnode_t* parse_unary(parser_t* parser) {
    if (parser->current.type == TOKEN_UNARY_OPERATOR) {
        char op = parser->current.lexeme[0];
        parser_advance(parser);
        astnode_t* operand = parse_unary(parser);
        return new_unary_expr(operand, op);
    }
    return parse_primary(parser);
}

astnode_t* parse_binary(parser_t* parser, int min_prec) {
    astnode_t* left = parse_unary(parser);

    while (parser->current.type == TOKEN_BINARY_OPERATOR &&
           get_precedence(parser->current.type, parser->current.lexeme) >= min_prec) {

        char op = parser->current.lexeme[0];
        int prec = get_precedence(parser->current.type, parser->current.lexeme);

        parser_advance(parser);

        astnode_t* right = parse_binary(parser, prec + 1);

        left = new_binary_expr(left, right, op);
    }

    return left;
}

astnode_t* parse_expression(parser_t* parser) {
    return parse_binary(parser, 1);
}

astnode_t* parse_statement(parser_t* parser) {
    switch (parser->current.type) {
        case TOKEN_IDENTIFIER: {
            char* varName = strdup(parser->current.lexeme);
            parser_advance(parser);

            switch (parser->current.type) {
                case TOKEN_ASSIGN: {
                    parser_advance(parser);

                    astnode_t* value = parse_expression(parser);
                    if (!value) return NULL;

                    if (parser->current.type != TOKEN_SEMICOLON) fatal("Expected semicolon after assignment of variable");
                    parser_advance(parser);

                    return new_assignment(varName, value);
                }
                case TOKEN_LPAREN: {
                    parser_advance(parser); 

                    astnode_t** argv = NULL;
                    int argc = 0;

                    if (parser->current.type != TOKEN_RPAREN) {
                        while (1) {
                            astnode_t* arg = parse_expression(parser);
                            if (!arg) {
                                fatal("Failed to parse function call argument");
                            }

                            argc++;
                            argv = realloc(argv, sizeof(astnode_t*) * argc);
                            argv[argc - 1] = arg;

                            if (parser->current.type == TOKEN_COMMA) {
                                parser_advance(parser); 
                            } else if (parser->current.type == TOKEN_RPAREN) {
                                break; 
                            } else {
                                fatal("Expected ',' or ')' after function call argument");
                            }
                        }
                    }

                    if (parser->current.type != TOKEN_RPAREN) {
                        fatal("Expected ')' at end of function call arguments");
                    }
                    parser_advance(parser); 

                    if (parser->current.type != TOKEN_SEMICOLON) fatal("Expected semicolon after call of function");
                    parser_advance(parser);

                    return new_function_call(varName, argc, argv);
                }
                default: fatalf("Expected '=' or '(' after identifier %s", varName);
            }
        }
        case TOKEN_FUNC: {
            parser_advance(parser);
            if (parser->current.type != TOKEN_IDENTIFIER) {
                fatal("'function' keyword not followed by an identifier");
            }

            char* funcName = strdup(parser->current.lexeme);
            parser_advance(parser);

            if (parser->current.type != TOKEN_LBRACE) {
                fatal("Function declaration is lacking an opening brace");
            }

            parser_advance(parser); 

            astnode_t* funcBody = NULL;
            astnode_t* lastStatement = NULL;

            while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
                astnode_t* stmt = parse_statement(parser);
                if (!stmt) fatal("Failed to parse statement inside function body");

                astnode_t* wrappedStmt = new_statement(stmt, NULL);

                if (!funcBody) {
                    funcBody = wrappedStmt;
                    lastStatement = wrappedStmt;
                } else {
                    lastStatement->statement.nextStatement = wrappedStmt;
                    lastStatement = wrappedStmt;
                }
            }

            if (parser->current.type != TOKEN_RBRACE) {
                fatal("Function body not closed with '}'");
            }

            parser_advance(parser); 

            
            return new_function_def(funcName, funcBody);
        }
        case TOKEN_IF: {
            parser_advance(parser); 

            
            astnode_t* condition = parse_expression(parser);
            if (!condition) {
                fatal("Failed to parse condition expression in if statement");
            }

            if (parser->current.type != TOKEN_LBRACE) {
                fatal("If statement is lacking an opening brace");
            }

            parser_advance(parser); 

            astnode_t* ifBody = NULL;
            astnode_t* lastStatement = NULL;

            while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
                astnode_t* stmt = parse_statement(parser);
                if (!stmt) fatal("Failed to parse statement inside if branch");

                astnode_t* wrappedStmt = new_statement(stmt, NULL);

                if (!ifBody) {
                    ifBody = wrappedStmt;
                    lastStatement = wrappedStmt;
                } else {
                    lastStatement->statement.nextStatement = wrappedStmt;
                    lastStatement = wrappedStmt;
                }
            }

            if (parser->current.type != TOKEN_RBRACE) {
                fatal("If branch not closed with '}'");
            }

            parser_advance(parser); 

            astnode_t* elseBody = NULL;

            
            if (parser->current.type == TOKEN_ELSE) {
                parser_advance(parser); 

                if (parser->current.type != TOKEN_LBRACE) {
                    fatal("Else statement is lacking an opening brace");
                }

                parser_advance(parser); 

                elseBody = NULL;
                lastStatement = NULL;

                while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
                    astnode_t* stmt = parse_statement(parser);
                    if (!stmt) fatal("Failed to parse statement inside else branch");

                    astnode_t* wrappedStmt = new_statement(stmt, NULL);

                    if (!elseBody) {
                        elseBody = wrappedStmt;
                        lastStatement = wrappedStmt;
                    } else {
                        lastStatement->statement.nextStatement = wrappedStmt;
                        lastStatement = wrappedStmt;
                    }
                }

                if (parser->current.type != TOKEN_RBRACE) {
                    fatal("Else branch not closed with '}'");
                }

                parser_advance(parser); 
            }

            return new_if_statement(condition, ifBody, elseBody);
        }
        case TOKEN_WHILE: {
            parser_advance(parser); 

            
            astnode_t* condition = parse_expression(parser);
            if (!condition) {
                fatal("Failed to parse condition expression in while loop");
            }

            if (parser->current.type != TOKEN_LBRACE) {
                fatal("While loop is lacking an opening brace");
            }

            parser_advance(parser); 

            astnode_t* doBody = NULL;
            astnode_t* lastStatement = NULL;

            while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
                astnode_t* stmt = parse_statement(parser);
                if (!stmt) fatal("Failed to parse statement inside while loop body");

                astnode_t* wrappedStmt = new_statement(stmt, NULL);

                if (!doBody) {
                    doBody = wrappedStmt;
                    lastStatement = wrappedStmt;
                } else {
                    lastStatement->statement.nextStatement = wrappedStmt;
                    lastStatement = wrappedStmt;
                }
            }

            if (parser->current.type != TOKEN_RBRACE) {
                fatal("While loop body not closed with '}'");
            }

            parser_advance(parser); 

            return new_while_loop(condition, doBody);
        }
    }

    return NULL;
}

astnode_t* parse_program(parser_t* parser) {
    astnode_t* first = NULL;
    astnode_t* last = NULL;

    while (parser->current.type != TOKEN_EOF) {
        astnode_t* stmt = parse_statement(parser);
        if (!stmt) {
            fatal("Failed to parse statement");
        }
        
        astnode_t* wrapped = new_statement(stmt, NULL);

        if (!first) {
            first = wrapped;
            last = wrapped;
        } else {
            last->statement.nextStatement = wrapped;
            last = wrapped;
        }
    }
    return first;
}