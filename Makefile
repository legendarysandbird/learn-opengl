intro: glad.o intro.o shader.o stb_image.o camera.o
	g++ shader.o stb_image.o glad.o intro.o camera.o -o intro -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

glad.o: glad.c
	g++ -c glad.c

intro.o: intro.cpp
	g++ -c intro.cpp 

shader.o: shader.cpp
	g++ -c shader.cpp

stb_image.o: stb_image.cpp
	g++ -c stb_image.cpp

camera.o: camera.cpp
	g++ -c camera.cpp

clean:
	rm -f glad.o shader.o intro.o camera.o intro
