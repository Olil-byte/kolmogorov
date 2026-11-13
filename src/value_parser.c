#include "value_parser.h"

#include "util.h"

void value_parser_init(value_parser_t *parser)
{
	parser->state = VALUE_PARSER_STATE_INIT;

	parser->value = 0.0;
	parser->signifier = 1.0;
}

int value_parser_proc(value_parser_t *parser, char sym)
{
	int result = 1;

	switch (parser->state) {
	case VALUE_PARSER_STATE_INIT:
		if (is_digit(sym)) {
			parser->state = VALUE_PARSER_STATE_INTEGER;
			parser->value = (double)sym - '0';
		} else {
			parser->state = VALUE_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case VALUE_PARSER_STATE_INTEGER:
		if (is_digit(sym)) {
			parser->value *= 10.0;
			parser->value += (double)sym - '0';
		} else if (sym == '.') {
			parser->state = VALUE_PARSER_STATE_POINT;
			parser->signifier = 1.0;
		} else {
			parser->state = VALUE_PARSER_STATE_FINISH;
			result = 0;

			goto EXIT;
		}
		break;
	case VALUE_PARSER_STATE_POINT:
		if (is_digit(sym)) {
			parser->value *= 10.0;
			parser->value += (double)sym - '0';
			parser->signifier *= 10.0;
		} else {
			parser->state = VALUE_PARSER_STATE_FINISH;
			result = 0;

			parser->value /= parser->signifier;

			goto EXIT;
		}
		break;
	default:
		parser->state = VALUE_PARSER_STATE_ERROR;
		result = -1;

		break;
	}

EXIT:
	return result;
}


