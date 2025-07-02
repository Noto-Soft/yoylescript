CC-TARGET:=gcc
CC-WIN:=x86_64-w64-mingw32-gcc
CC-LIN:=gcc
OUT:=./yoyle
CFFLAGS:=-o $(OUT) -I./src/inc
CLFLAGS:=
CSOURCES=$(shell find src -type f -name '*.c')

.PHONY: default all test clean

default: all test

all:
	$(CC) $(CFFLAGS) $(CSOURCES) $(CLFLAGS)
	$(CC-WIN) $(CFFLAGS) $(CSOURCES) $(CLFLAGS)
	$(CC-LIN) $(CFFLAGS) $(CSOURCES) $(CLFLAGS)

test:
	@$(OUT) example.yoyle

clean:
	rm -f $(OUT)