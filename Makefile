points: glad.o points.o shader.o stb_image.o camera.o model.o mesh.o
	g++ shader.o stb_image.o glad.o camera.o points.o mesh.o model.o -o points -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

depth: glad.o depth.o shader.o stb_image.o camera.o mesh.o model.o
	g++ shader.o stb_image.o glad.o depth.o camera.o mesh.o model.o -o depth -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

intro: glad.o intro.o shader.o stb_image.o camera.o mesh.o model.o
	g++ shader.o stb_image.o glad.o intro.o camera.o mesh.o model.o -o intro -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

glad.o: glad.c
	g++ -c glad.c

intro.o: intro.cpp
	g++ -c intro.cpp 

depth.o: depth.cpp
	g++ -c depth.cpp

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

points.o: points.cpp
	g++ -c points.cpp

clean:
	rm -f glad.o shader.o intro.o camera.o mesh.o model.o depth.o points.o intro
