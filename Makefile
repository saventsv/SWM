# Simple Makefile for PlaceHolderWM

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -std=c11

# Libraries to link
LIBS = -lX11

# Target executable
TARGET = mywm

# Source files
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) /usr/local/bin/$(TARGET)
