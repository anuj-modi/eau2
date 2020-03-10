CC = g++
CFLAGS = -g -Wall -std=c++11

SRCS = $(wildcard *.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))
DEPS = catch.hpp ../src/dataframe.h

all: test

test: $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o test $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -rf *.o ./test
