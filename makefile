run:		sortalgs
	./bin/sortalgs

sortalgs:	src/*.c
	gcc -I headers -o bin/sortalgs src/*.c -fopenmp