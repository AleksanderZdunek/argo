TARGET = argo
OBJ = 	main.o\
		main_c.o\
		makefile.o\
		c.o\
		makefile_c.o\
		cpp.o\
		makefile_cpp.o

CC = gcc
CFLAGS = -std=c11 -O3 -Wall -Wpedantic
BUILD_DATE = \"$(shell date +%Y%m%d:%H%M)\"

.PHONY: clean all

all: $(TARGET)

main.o: main.c Makefile
	$(CC) $(CFLAGS) -DBUILD_DATE=$(BUILD_DATE) -c -o $@ $<

main_c.o: main.c
makefile.o: Makefile
c.o: boilerplate/c/main.c
makefile_c.o: boilerplate/c/Makefile
cpp.o: boilerplate/cpp/main.cpp
makefile_cpp.o: boilerplate/cpp/Makefile
%.o:
	ld -r -b binary $< -o $@

%:
	$(error No rule to make target '$@')

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
