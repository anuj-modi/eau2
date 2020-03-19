CC = g++
CFLAGS = -g -Wall -Werror -std=c++11 -pthread

STUFF = src tests data report Makefile Dockerfile

TEST_SRCS = $(wildcard tests/*_test.cpp)
_OBJS = $(patsubst %.cpp,%.o,$(notdir $(TEST_SRCS))) test.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
DEPS := ./tests/catch.hpp $(shell find src/ -name *.h)
ODIR = build
TEST_OUT = ./$(ODIR)/test

all: zip test valgrind

zip: submission.zip

submission.zip: $(STUFF)
	mkdir eau2
	cp -r $(STUFF) eau2
	zip -x '*.DS_Store' -x '*__MACOSX*' -x '*.AppleDouble*' -r submission.zip eau2
	@rm -rf eau2/

$(ODIR):
	mkdir -p ./build

build/test.o: $(ODIR) tests/test.cpp
	$(CC) $(CFLAGS) -o $@ -c tests/test.cpp

$(ODIR)/%.o: ./tests/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -Isrc/ -o $@ -c $<

$(TEST_OUT): $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $(TEST_OUT) $(OBJS)

.PHONY:test
test: $(ODIR) $(TEST_OUT)
	$(TEST_OUT)

.PHONY: valgrind
valgrind: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT)

.PHONY: clean
clean:
	rm -rf build/* submission.zip
