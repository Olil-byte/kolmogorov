#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "number_parser.h"
#include "value_parser.h"

#include "arg_list.h"

typedef enum arg_parser_state
{
	ARG_PARSER_STATE_INIT,

	ARG_PARSER_STATE_N,
	ARG_PARSER_STATE_N_ASSIGN,
	ARG_PARSER_STATE_N_VALUE,

	ARG_PARSER_STATE_M,
	ARG_PARSER_STATE_MU,
	ARG_PARSER_STATE_MU_NUMBER,
	ARG_PARSER_STATE_MU_WAIT_ASSIGN,
	ARG_PARSER_STATE_MU_ASSIGN,
	ARG_PARSER_STATE_MU_VALUE,

	ARG_PARSER_STATE_L,
	ARG_PARSER_STATE_LA,
	ARG_PARSER_STATE_LAM,
	ARG_PARSER_STATE_LAMB,
	ARG_PARSER_STATE_LAMBD,
	ARG_PARSER_STATE_LAMBDA,
	ARG_PARSER_STATE_LAMBDA_NUMBER,
	ARG_PARSER_STATE_LAMBDA_WAIT_ASSIGN,
	ARG_PARSER_STATE_LAMBDA_ASSIGN,
	ARG_PARSER_STATE_LAMBDA_VALUE,

	ARG_PARSER_STATE_ERROR
} arg_parser_state_t;

typedef struct arg_parser
{
	arg_parser_state_t state;

	number_parser_t number_parser;

	value_parser_t value_parser;

	int n;

	arg_list_t *mues;
	arg_list_t *lambdas;
} arg_parser_t;

void arg_parser_init(arg_parser_t *parser);

void arg_parser_free(arg_parser_t *parser);

int arg_parser_proc(arg_parser_t *parser, char sym);

void arg_parser_result(
	arg_parser_t *parser,
	arg_list_t **mues,
	arg_list_t **lambdas,
	int *n
);

#endif

