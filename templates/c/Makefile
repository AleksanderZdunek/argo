TARGET = main
OBJ = 	main.o

CC = gcc
CFLAGS = -std=c11 -O3 -Wall -Wpedantic

all: $(TARGET)

-include *.d
%.o: %.c Makefile
	$(CC) -MMD $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET) $(OBJ:.o=.d)
