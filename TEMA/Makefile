build: serial pthreads openmp mpi_prog hibrid
	
serial:
	g++ -o serial -g src/serial.cpp

pthreads:
	g++ -o pthreads -g src/pthreads.cpp -lpthread

openmp:
	g++ -o openmp -g src/openmp.cpp -fopenmp

mpi_prog: 
	mpic++ -o mpi_prog src/mpi_prog.cpp

hibrid:
	mpic++ -o hibrid src/mpi_prog.cpp -fopenmp

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./serial

clean:
	rm serial pthreads openmp mpi_prog