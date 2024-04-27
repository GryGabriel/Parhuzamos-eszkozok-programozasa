#The compiler
CC = gcc

#Compiler flags
#	-g for debugging information
#	-Wall returns most compiler warnings
CFLAGS = -g -Wall

#The build target executable
TARGET = sequential_v2

all: $(TARGET)
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
