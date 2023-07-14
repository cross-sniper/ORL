make: src/main.cpp
	g++ -o main src/main.cpp -lSDL2 -lSDL2_ttf -I/usr/include/SDL2 -I/usr/include/SDL2/SDL2_ttf

run: src/main.cpp
	g++ -o main src/main.cpp -lSDL2 -lSDL2_ttf -I/usr/include/SDL2 -I/usr/include/SDL2/SDL2_ttf
	./main