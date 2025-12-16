CC := clang
CFLAGS := -Wall -Wextra
SOURCES := main.c lox.c util.c scanner.c token.c token_type.c parser.c expr.c interpreter.c
OBJECTS := $(SOURCES:.c=.o)
DEPS := $(OBJECTS:.o=.d)
TARGET := lox

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MP

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)