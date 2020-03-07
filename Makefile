CC = g++
CFLAGS = -g -Wall -Werror -std=c++11


zip: submission.zip

submission.zip:
	mkdir eau2
	cp -r src tests Makefile data report eau2
	zip -x '*.DS_Store' -x '*__MACOSX*' -x '*.AppleDouble*' -r submission.zip eau2
	@rm -rf eau2/

.PHONY: clean
clean:
	rm -rf submission.zip eau2/
