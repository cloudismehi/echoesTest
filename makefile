raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

echoesTest: Room.o Particle.o echoesTest.o
	g++ -std=c++20 ${raylib_dep} -I . -lraylib src/obj/Room.o src/obj/Particle.o src/obj/echoesTest.o -o echoesTest
	mv echoesTest execs

echoesTest.o: src/main.cpp
	g++ -std=c++20 src/main.cpp -I . -c -o echoesTest.o
	mv echoesTest.o src/obj

Room.o: src/Room.cpp
	g++ -std=c++20 src/Room.cpp -I . -c -o Room.o
	mv Room.o src/obj

Particle.o: src/Particle.cpp
	g++ -std=c++20 src/Particle.cpp -I . -c -o Particle.o
	mv Particle.o src/obj
