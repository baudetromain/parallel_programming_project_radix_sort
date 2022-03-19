CC=gcc
CLFAGS=-Wall -std=gnu99 -fopenmp

SRC_DIR=src
SRC=$(wildcard *.c)
EXE=$(SRC:.c=)

BUILD_DIR=build

all: $(EXE)

clean:
	rm -r $(BUILD_DIR)/*

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $@.c