run:		sortalgs
	./sortalgs

sortalgs:	main.c
	gcc  main.c utils.c mergesort.c -o sortalgs -fopenmp
