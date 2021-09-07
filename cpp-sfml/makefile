all: watch spiral

watch: clean
	g++ -c watch.cpp
	g++ watch.o -o watch -lsfml-graphics -lsfml-window -lsfml-system

spiral: clean
	g++ -c spiral.cpp
	g++ spiral.o -o spiral -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm -f watch watch.o spiral spiral.o
