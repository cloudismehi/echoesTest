raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

echoesTest: echoesTest.o
	g++ -std=c++20 ${raylib_dep} -lraylib obj/echoesTest.o -o echoesTest
	mv echoesTest execs

echoesTest.o: main.cpp
	g++ -std=c++20 main.cpp -c -o echoesTest.o
	mv echoesTest.o obj
