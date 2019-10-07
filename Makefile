all: game
	./play
game:
	g++ main.cpp -o play -std=c++11 -lpthread -lncurses