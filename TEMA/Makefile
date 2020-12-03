build: serial pthreads openmp
	
serial:
	g++ -o serial -g src/serial.cpp

pthreads:
	g++ -o pthreads -g src/pthreads.cpp -lpthread

openmp:
	g++ -o openmp -g src/openmp.cpp -fopenmp

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./serial

clean:
	rm serial pthreads openmp