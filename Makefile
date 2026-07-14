CC=clang
CXX=clang++

CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -Werror -g
CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -Werror -g
LDFLAGS=-g -lm

.PHONY: all clean

all: wasmi

clean:
	rm -f wasmi wasmi.o

wasmi: wasmi.o
	$(CXX) $(LDFLAGS) -o $@ $^

wasmi.o: wasmi.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<
