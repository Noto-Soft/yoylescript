#pragma once

typedef struct varentry_t varentry_t;
typedef struct yoylestack_t yoylestack_t;

typedef struct yoylestate_t {
    varentry_t* symbol_table;
    yoylestack_t* arg_stack;
} yoylestate_t;