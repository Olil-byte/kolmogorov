.PHONY: all debug release clean

CC = gcc
CFLAGS_COMMON = -Wall -ansi -pedantic
CPPFLAGS = -Isrc/

TARGET = kolmogorov

SRCS = src/main.c \
	src/util.c \
	src/number_parser.c \
	src/value_parser.c \
	src/arg_parser.c \
	src/arg_list.c

OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
DEPS = $(patsubst src/%.c, obj/%.d, $(SRCS))

all: release

debug: CFLAGS = $(CFLAGS_COMMON) -g -O0 -DDEBUG
debug: bin/$(TARGET)

release: CFLAGS = $(CFLAGS_COMMON) -O2
release: bin/$(TARGET)

bin/$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ \
		-lm \
		-o $@

obj/%.d: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MT $(patsubst %.d,%.o,$@) $(CPPFLAGS) $< > $@

obj/%.o: src/%.c src/%.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf obj bin

