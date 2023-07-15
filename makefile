make: src/main.cpp
	g++ -o main src/main.cpp -lSDL2 -lSDL2_ttf -lopenal -lsndfile -I/usr/include/SDL2 -I/usr/include/SDL2/SDL2_ttf
install_libs:
	sudo apt-get install libsdl2-dev libsdl2-ttf-dev libopenal-dev libsndfile1-dev


run: src/main.cpp
	g++ -o main src/main.cpp -lSDL2 -lSDL2_ttf -lopenal -lsndfile -I/usr/include/SDL2 -I/usr/include/SDL2/SDL2_ttf
	./main
