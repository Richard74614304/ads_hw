CXX = gcc
TARGET = shopping
OBJ = ./src/Shopping_with_coupons.o

CXXFLAGS = -Wall

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

./src/%.o: ./src/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
