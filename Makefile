TARGET = run
OBJ = 	main.o\
		main_c.o\
		makefile.o

CC = gcc
CFLAGS = -std=c11 -O3 -Wall -Wpedantic

all: $(TARGET)

main_c.o: main.c
	ld -r -b binary $< -o $@

makefile.o: Makefile
	ld -r -b binary $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
