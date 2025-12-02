#All era definido assim:
#g++ main.cpp ship.cpp asteroid.cpp shinyTriangle.cpp -o main -l raylib
#Devido ao setup.h não é mais necessário
all:
	g++ main.cpp setup.cpp -o main -l raylib

run: all
	./main

clean:
	rm -f main ./Testes/main *.o

test:
	g++ ./Testes/tests.cpp ./Testes/testship.cpp ./Testes/testasteroid.cpp ./Testes/shinytriangle.cpp -o main -l raylib

runtst: test
	./main
