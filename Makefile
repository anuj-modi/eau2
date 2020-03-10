CC = g++
CFLAGS = -g -Wall -Werror -std=c++11

STUFF = src tests data report Makefile Dockerfile

zip: submission.zip

submission.zip: $(STUFF)
	mkdir eau2
	cp -r $(STUFF) eau2
	zip -x '*.DS_Store' -x '*__MACOSX*' -x '*.AppleDouble*' -r submission.zip eau2
	@rm -rf eau2/

.PHONY: test
test:
	cd tests/ && make test && ./test

.PHONY: clean
clean:
	rm -rf submission.zip eau2/
	cd tests/ && make clean
