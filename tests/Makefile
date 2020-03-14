CC = g++
CFLAGS = -g -Wall -std=c++11 -pthread

SRCS = $(wildcard *.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))
DEPS = catch.hpp $(wildcard ../src/*.h) $(wildcard ../src/sorer/*.h)

all: test

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp

test: $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o test $(OBJS)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

.PHONY: valgrind
valgrind: test
	valgrind ./test

.PHONY: clean
clean:
	rm -rf *.o ./test
