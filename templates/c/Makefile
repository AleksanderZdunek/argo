TARGET = main
OBJ = 	main.o

CC = gcc
CFLAGS = -std=c11 -O3 -Wall -Wpedantic

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
