CC = g++
CFLAGS = -g -Wall -Werror -std=c++11 -pthread

TEST_SRCS = $(wildcard tests/*_test.cpp)
_OBJS = $(patsubst %.cpp,%.o,$(notdir $(TEST_SRCS))) test.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
DEPS := ./tests/catch.hpp $(shell find src/ -name *.h)
ODIR = build
TEST_OUT = ./$(ODIR)/test

default: $(ODIR) $(TEST_OUT)

all: zip test valgrind

client: $(ODIR)/client
server: $(ODIR)/server

$(ODIR):
	mkdir -p ./$(ODIR)

$(ODIR)/test.o: ./tests/test.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

$(ODIR)/%.o: ./tests/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -Isrc/ -o $@ -c $<

$(TEST_OUT): $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $(TEST_OUT) $(OBJS)

$(ODIR)/linus: src/linus.cpp $(DEPS)
	$(CC) $(CFLAGS) -Isrc/ -o $@ $<

$(ODIR)/client: $(ODIR)/linus
	cp $< $@

$(ODIR)/server: $(ODIR)/linus
	cp $< $@

.PHONY:test
test: $(ODIR) $(TEST_OUT)
	$(TEST_OUT) "~[milestone]"

.PHONY:test-all
test-all: $(ODIR) $(TEST_OUT)
	$(TEST_OUT)

.PHONY: valgrind
valgrind: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT) "~[milestone]"

.PHONY: valgrind-all
valgrind-all: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT)

.PHONY: m1
m1: $(TEST_OUT)
	$(TEST_OUT) "[m1]"

.PHONY: m4
m4: $(TEST_OUT)
	$(TEST_OUT) "[m4]"

.PHONY: m5
m5: $(TEST_OUT)
	$(TEST_OUT) "[m5]"

.PHONY: valgrind-m1
valgrind-m1: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT) "[m1]"

.PHONY: valgrind-m4
valgrind-m4: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT) "[m4]"

.PHONY: valgrind-m5
valgrind-m5: $(TEST_OUT)
	valgrind --errors-for-leak-kinds=all --error-exitcode=5 --leak-check=full $(TEST_OUT) "[m5]"

.PHONY: clean
clean:
	rm -rf $(ODIR)/* submission.zip
