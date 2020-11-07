#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int N;

/**
 * 	@author cristian.chilipirea
 *
 */

void compareVectors(int *a, int *b)
{
	// DO NOT MODIFY
	int i;
	for (i = 0; i < N; i++)
	{
		if (a[i] != b[i])
		{
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int *v)
{
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N - 1]);
	for (i = 0; i < N; i++)
	{
		printf("%*i", displayWidth, v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b)
{
	// DO NOT MODIFY
	int A = *(int *)a;
	int B = *(int *)b;
	return A - B;
}

int main(int argc, char *argv[])
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

	N = nProcesses;
	int aux;
	int minim;
	int i;

	if (rank == 0)
	{ // This code is run by a single process

		int *v = (int *)malloc(sizeof(int) * N);
		int *vQSort = (int *)malloc(sizeof(int) * N);

		// generate the vector v with random values
		// DO NOT MODIFY
		srand(42);
		for (i = 0; i < N; i++)
			v[i] = rand() % N;

		// make copy to check it against qsort
		// DO NOT MODIFY
		for (i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);

		minim = v[0];

		for (i = 1; i < nProcesses; i++) {
			if (v[i] > minim)
			{
				MPI_Send(&(v[i]), 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			}
			else
			{
				MPI_Send(&minim, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
				minim = v[i];
			}
		}

		v[0] = minim;

		MPI_Status status;
		for (i = 1; i < nProcesses; i++)
		{

			MPI_Recv(&aux, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			v[status.MPI_SOURCE] = aux;
		}

		compareVectors(v, vQSort);
	}

	else
	{

		for (i = 0; i < nProcesses - rank; i++)
		{

			MPI_Recv(&aux, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (i == 0)
				minim = aux;
			else
			{

				if (minim < aux)
					MPI_Send(&aux, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
				else
				{
					MPI_Send(&minim, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
					minim = aux;
				}
			}
		}

		MPI_Send(&minim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}