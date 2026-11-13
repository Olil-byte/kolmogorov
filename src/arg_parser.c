#include "arg_parser.h"

#include <stdio.h>

#include "util.h"

void arg_parser_init(arg_parser_t *parser)
{
	if (!parser) {
		return;
	}

	parser->state = ARG_PARSER_STATE_INIT;

	number_parser_init(&parser->number_parser);
	value_parser_init(&parser->value_parser);

	parser->n = -1;

	parser->mues = 0;
	parser->lambdas = 0;
}

void arg_parser_free(arg_parser_t *parser)
{
	arg_list_free(&parser->mues);
	arg_list_free(&parser->lambdas);
}

int arg_parser_proc(arg_parser_t *parser, char sym)
{
	/* while processing result is 1 */
	int result = 1;

	int child_result;

	switch (parser->state) {
	case ARG_PARSER_STATE_INIT:
		if (is_space(sym)) {
			break;
		} else if (sym == 'n') {
			if (parser->n != -1) {
				fprintf(
					stderr,
					"warning: n assigned more than once"
				);
			}
			parser->state = ARG_PARSER_STATE_N;
		} else if (sym == 'm') {
			parser->state = ARG_PARSER_STATE_M;
		} else if (sym == 'l') {
			parser->state = ARG_PARSER_STATE_L;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;

	case ARG_PARSER_STATE_N:
		if (is_space(sym)) {
			break;
		} else if (sym == '=') {
			parser->state = ARG_PARSER_STATE_N_ASSIGN;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_N_ASSIGN:
		if (is_space(sym)) {
			break;
		} else if (is_digit(sym)) {
			parser->state = ARG_PARSER_STATE_N_VALUE;

			number_parser_init(&parser->number_parser);
			number_parser_proc(&parser->number_parser, sym);
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_N_VALUE:
		child_result = number_parser_proc(&parser->number_parser, sym);
		if (child_result == 0) {
			parser->state = ARG_PARSER_STATE_INIT;
			result = arg_parser_proc(parser, sym);

			parser->n = parser->number_parser.number;
		} else if (child_result == 1) {
			break;
		} else if (child_result == -1) {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;

	case ARG_PARSER_STATE_M:
		if (sym == 'u') {
			parser->state = ARG_PARSER_STATE_MU;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_MU:
		if (is_digit(sym)) {
			parser->state = ARG_PARSER_STATE_MU_NUMBER;

			number_parser_init(&parser->number_parser);
			number_parser_proc(&parser->number_parser, sym);
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_MU_NUMBER:
		child_result = number_parser_proc(&parser->number_parser, sym);
		if (child_result == 0) {
			parser->state = ARG_PARSER_STATE_MU_WAIT_ASSIGN;
			result = arg_parser_proc(parser, sym);
		} else if (child_result == 1) {
			break;
		} else if (child_result == -1) {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_MU_WAIT_ASSIGN:
		if (is_space(sym)) {
			break;
		} else if (sym == '=') {
			parser->state = ARG_PARSER_STATE_MU_ASSIGN;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_MU_ASSIGN:
		if (is_space(sym)) {
			break;
		} else if (is_digit(sym)) {
			parser->state = ARG_PARSER_STATE_MU_VALUE;

			value_parser_init(&parser->value_parser);
			value_parser_proc(&parser->value_parser, sym);
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_MU_VALUE:
		child_result = value_parser_proc(&parser->value_parser, sym);
		if (child_result == 0) {
			parser->state = ARG_PARSER_STATE_INIT;
			result = arg_parser_proc(parser, sym);

			arg_list_add(
				&parser->mues,
				parser->number_parser.number,
				parser->value_parser.value
			);
		} else if (child_result == 1) {
			break;
		} else if (child_result == -1) {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;

	case ARG_PARSER_STATE_L:
		if (sym == 'a') {
			parser->state = ARG_PARSER_STATE_LA;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_LA:
		if (sym == 'm') {
			parser->state = ARG_PARSER_STATE_LAM;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
		case ARG_PARSER_STATE_LAM:
		if (sym == 'b') {
			parser->state = ARG_PARSER_STATE_LAMB;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
		case ARG_PARSER_STATE_LAMB:
		if (sym == 'd') {
			parser->state = ARG_PARSER_STATE_LAMBD;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
		case ARG_PARSER_STATE_LAMBD:
		if (sym == 'a') {
			parser->state = ARG_PARSER_STATE_LAMBDA;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;

	case ARG_PARSER_STATE_LAMBDA:
		if (is_digit(sym)) {
			parser->state = ARG_PARSER_STATE_LAMBDA_NUMBER;

			number_parser_init(&parser->number_parser);
			number_parser_proc(&parser->number_parser, sym);
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_LAMBDA_NUMBER:
		child_result = number_parser_proc(&parser->number_parser, sym);
		if (child_result == 0) {
			parser->state = ARG_PARSER_STATE_LAMBDA_WAIT_ASSIGN;
			result = arg_parser_proc(parser, sym);
		} else if (child_result == 1) {
			break;
		} else if (child_result == -1) {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_LAMBDA_WAIT_ASSIGN:
		if (is_space(sym)) {
			break;
		} else if (sym == '=') {
			parser->state = ARG_PARSER_STATE_LAMBDA_ASSIGN;
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_LAMBDA_ASSIGN:
		if (is_space(sym)) {
			break;
		} else if (is_digit(sym)) {
			parser->state = ARG_PARSER_STATE_LAMBDA_VALUE;

			value_parser_init(&parser->value_parser);
			value_parser_proc(&parser->value_parser, sym);
		} else {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case ARG_PARSER_STATE_LAMBDA_VALUE:
		child_result = value_parser_proc(&parser->value_parser, sym);
		if (child_result == 0) {
			parser->state = ARG_PARSER_STATE_INIT;
			result = arg_parser_proc(parser, sym);

			arg_list_add(
				&parser->lambdas,
				parser->number_parser.number,
				parser->value_parser.value
			);

		} else if (child_result == 1) {
			break;
		} else if (child_result == -1) {
			parser->state = ARG_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	default:
		parser->state = ARG_PARSER_STATE_ERROR;
		result = -1;

		goto EXIT;
	}

EXIT:
	return result;
}

void arg_parser_result(
	arg_parser_t *parser,
	arg_list_t **mues,
	arg_list_t **lambdas,
	int *n
)
{
	*mues = parser->mues;
	parser->mues = 0;

	*lambdas = parser->lambdas;
	parser->lambdas = 0;

	*n = parser->n;
}

