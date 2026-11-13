#ifndef VALUE_PARSER_H
#define VALUE_PARSER_H

typedef enum value_parser_state
{
	VALUE_PARSER_STATE_INIT,

	VALUE_PARSER_STATE_INTEGER,
	VALUE_PARSER_STATE_POINT,
	VALUE_PARSER_STATE_DECIMAL,

	VALUE_PARSER_STATE_FINISH,

	VALUE_PARSER_STATE_ERROR
} value_parser_state_t;

typedef struct value_parser
{
	value_parser_state_t state;

	double value;
	double signifier;
} value_parser_t;

void value_parser_init(value_parser_t *parser);

int value_parser_proc(value_parser_t *parser, char sym);

#endif

