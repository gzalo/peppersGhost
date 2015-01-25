all:
	g++ pghost.cpp -o pghost.exe -lmingw32 -lsdl2main -lsdl2 -lsdl2_image -lopengl32 -lglew32 -O2
	
run:
	pghost
	