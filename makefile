run:		sortalgs
	./sortalgs

sortalgs:	main.c
	gcc -o sortalgs main.c -fopenmp
