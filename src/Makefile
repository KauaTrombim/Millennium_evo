#All era definido assim:
#g++ main.cpp ship.cpp asteroid.cpp shinyTriangle.cpp -o main -l raylib
#Devido ao setup.h não é mais necessário
allWin:
	g++ -I../raylib/include -L../raylib/lib main.cpp -o main -lraylib -lgdi32 -lwinmm

runWin: allWin
	./main

allLnx:
	g++ main.cpp -o main -l raylib

runLnx: allLnx
	./main

clean:
	rm -f main ./Testes/main *.o

test:
	g++ ./Testes/tests.cpp ./Testes/testship.cpp ./Testes/testasteroid.cpp ./Testes/shinytriangle.cpp -o main -l raylib

runtst: test
	./main
