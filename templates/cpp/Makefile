TARGET = main
OBJ = 	main.o

CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wpedantic

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
