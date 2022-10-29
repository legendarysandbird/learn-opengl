intro: glad.o intro.o shader.o stb_image.o camera.o mesh.o model.o
	g++ shader.o stb_image.o glad.o intro.o camera.o mesh.o model.o -o intro -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

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

mesh.o: mesh.cpp
	g++ -c mesh.cpp

model.o: model.cpp
	g++ -c model.cpp

clean:
	rm -f glad.o shader.o intro.o camera.o mesh.o model.o intro
