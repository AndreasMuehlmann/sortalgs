run:		sortalgs
	./bin/sortalgs

sortalgs:	src/*.c
	gcc -o bin/sortalgs src/*.c -fopenmp
