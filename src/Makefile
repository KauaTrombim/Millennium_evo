all:
	g++ main.cpp -o main -l raylib

run: all
	./main

clean:
	rm -f main ./Testes/main *.o

test:
	g++ ./Testes/tests.cpp ./Testes/testship.cpp ./Testes/testasteroid.cpp ./Testes/shinytriangle.cpp -o main -l raylib

runtst: test
	./main
