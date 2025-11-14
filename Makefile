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

all: $(TARGET)

main.o: main.c Makefile
	$(CC) $(CFLAGS) -DBUILD_DATE=$(BUILD_DATE) -c -o $@ $<

#TODO: Can "Canned Recipes" help clean up this mess?
#https://www.gnu.org/software/make/manual/make.html#Canned-Recipes
main_c.o: main.c
	ld -r -b binary $< -o $@

makefile.o: Makefile
	ld -r -b binary $< -o $@

c.o: templates/c/main.c
	ld -r -b binary $< -o $@

makefile_c.o: templates/c/Makefile
	ld -r -b binary $< -o $@

cpp.o: templates/cpp/main.cpp
	ld -r -b binary $< -o $@

makefile_cpp.o: templates/cpp/Makefile
	ld -r -b binary $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
