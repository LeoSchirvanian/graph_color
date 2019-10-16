clean:
	rm -f *.o
	rm color

color:
	gcc -o color color_genetic.c -lm -O2
	
check:
	gcc -o check check.c
