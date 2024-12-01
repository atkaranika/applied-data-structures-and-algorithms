# Makefile for dict_based_compression project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Target executable name
TARGET = dict

# Source file
SRC = dict.c

# Build the executable
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up build files
clean:
	rm -f $(TARGET)

# Run the program (adjust arguments as needed)
run: $(TARGET)
	./$(TARGET) input_file 4 32 output_file
