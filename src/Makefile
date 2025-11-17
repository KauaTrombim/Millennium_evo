all:
	g++ main.cpp ship.cpp -o main -l raylib

run: all
	./main

clean:
	rm -f main ./Testes/main *.o

test:
	g++ ./Testes/tests.cpp ./Testes/testship.cpp -o main -l raylib

runtst: test
	./main
