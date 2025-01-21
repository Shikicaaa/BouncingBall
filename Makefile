all:
	gcc -I src/include -L src/lib -o main WinMain.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm -lstdc++
	