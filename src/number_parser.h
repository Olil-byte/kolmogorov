#ifndef NUMBER_PARSER_H
#define NUMBER_PARSER_H

typedef enum number_parser_state
{
	NUMBER_PARSER_STATE_INIT,

	NUMBER_PARSER_STATE_VALUE,

	NUMBER_PARSER_STATE_FINISH,

	NUMBER_PARSER_STATE_ERROR
} number_parser_state_t;

typedef struct number_parser
{
	number_parser_state_t state;

	int number;
} number_parser_t;

void number_parser_init(number_parser_t *parser);

int number_parser_proc(number_parser_t *parser, char sym);

#endif

