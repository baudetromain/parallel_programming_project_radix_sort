all:
	gcc -o build/radixsort radixsort.c -lm -fopenmp -Wall -std=gnu99

clean:
	rm -rf build/*