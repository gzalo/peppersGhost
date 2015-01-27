all:
	g++ pghost.cpp -o pghost.exe -lSDL2 -lSDL2_image -lGL -lGLEW -O2
	
run:
	./pghost.exe
	
