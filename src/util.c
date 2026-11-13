#include "util.h"

int is_space(char sym)
{
	return sym == ' ' || sym == '\r' || sym == '\n' || sym == '\t';
}

int is_digit(char sym)
{
	return '0' <= sym && sym <= '9';
}

