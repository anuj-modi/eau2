CC = g++
CFLAGS = -o3 -Wall -Werror -pthread
HDRS = $(wildcard *.h)

all: bench

build: bench

setup:
	if [ -f "datafile.txt" ]; \
	then \
		echo "datafile exists"; \
	else \
		wget -o datafile.zip https://ccgomes.com/datafile.zip; \
		sudo apt install zip unzip; \
		unzip datafile.zip.1; \
	fi

bench: bench.cpp $(HDRS) setup
	$(CC) $(CFLAGS) -o bench bench.cpp

run: bench setup
	./bench -n 1000 -f datafile.txt -r MaxRower
	./bench -n 1000 -f datafile.txt -r MaxRower -p 1
	echo
	./bench -n 10000 -f datafile.txt -r MaxRower
	./bench -n 10000 -f datafile.txt -r MaxRower -p 1
	echo
	./bench -n 100000 -f datafile.txt -r MaxRower
	./bench -n 100000 -f datafile.txt -r MaxRower -p 1
	echo
	./bench -n 1000000 -f datafile.txt -r MaxRower
	./bench -n 1000000 -f datafile.txt -r MaxRower -p 1
	echo
	./bench -n 5000000 -f datafile.txt -r MaxRower
	./bench -n 5000000 -f datafile.txt -r MaxRower -p 1
	echo
	./bench -n 1000 -f datafile.txt -r ConcatStrRower
	./bench -n 1000 -f datafile.txt -r ConcatStrRower -p 1
	echo
	./bench -n 10000 -f datafile.txt -r ConcatStrRower
	./bench -n 10000 -f datafile.txt -r ConcatStrRower -p 1
	echo
	./bench -n 100000 -f datafile.txt -r ConcatStrRower
	./bench -n 100000 -f datafile.txt -r ConcatStrRower -p 1
	echo
	./bench -n 1000000 -f datafile.txt -r ConcatStrRower
	./bench -n 1000000 -f datafile.txt -r ConcatStrRower -p 1
	echo
	./bench -n 5000000 -f datafile.txt -r ConcatStrRower
	./bench -n 5000000 -f datafile.txt -r ConcatStrRower -p 1

.PHONY: clean
clean:
	@rm -rf bench
	@rm -rf datafile.zip
	@rm -rf datafile.zip.1
	@rm -rf datafile.txt
