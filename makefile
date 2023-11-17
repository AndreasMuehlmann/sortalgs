run:		sortalgs
	./sortalgs && rm sortalgs

sortalgs:	main.c
	gcc  main.c utils.c mergesort.c quicksort.c psrs.c -o sortalgs -fopenmp
