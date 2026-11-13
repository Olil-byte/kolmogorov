#include "number_parser.h"

#include "util.h"

void number_parser_init(number_parser_t *parser)
{
	parser->state = NUMBER_PARSER_STATE_INIT;

	parser->number = 0;
}

int number_parser_proc(number_parser_t *parser, char sym)
{
	int result = 1;

	switch (parser->state) {
	case NUMBER_PARSER_STATE_INIT:
		if (is_digit(sym)) {
			parser->state = NUMBER_PARSER_STATE_VALUE;
			parser->number = (int)sym - '0';
		} else {
			parser->state = NUMBER_PARSER_STATE_ERROR;
			result = -1;

			goto EXIT;
		}
		break;
	case NUMBER_PARSER_STATE_VALUE:
		if (is_digit(sym)) {
			/* handle leading zeroes */
			if (parser->number == 0) {
				parser->state = NUMBER_PARSER_STATE_ERROR;
				result = -1;

				goto EXIT;
			}
			parser->number *= 10;
			parser->number += (int)sym - '0';
		} else {
			parser->state = NUMBER_PARSER_STATE_FINISH;
			result = 0;

			goto EXIT;
		}
		break;
	default:
		parser->state = NUMBER_PARSER_STATE_ERROR;
		result = -1;

		break;
	}

EXIT:
	return result;
}
