CC:=gcc
OUT:=./yoyle
CFFLAGS:=-o $(OUT) -I./src/inc
CLFLAGS:=-lfl
CSOURCES=$(shell find src -type f -name '*.c')

.PHONY: default all test clean

default: all test

all:
	$(CC) $(CFFLAGS) $(CSOURCES) $(CLFLAGS)

test:
	@$(OUT) example.yoyle

clean:
	rm -f $(OUT)