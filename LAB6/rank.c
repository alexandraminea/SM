#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 100000
#define NUM_THREADS 4

void compareVectors(int* a, int* b)
{
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

int cmp(const void* a, const void* b)
{
    int A = *(int*)a;
    int B = *(int*)b;
   return A - B;
}

int main(int argc, char *argv[])
{
   double t1,t2;
   int v[N], w[N];
   int i, j, pos, tid;

   for (i=0; i<N; i++)
      v[i] = N - i;

   omp_set_num_threads(NUM_THREADS);

   #pragma omp parallel private(j,i,tid, pos)
   {
      tid = omp_get_thread_num();
      int rank;
      int start = tid * N / NUM_THREADS;
      int end = tid * N / NUM_THREADS + N / NUM_THREADS;
      t1 = omp_get_wtime();
      for (j = start; j < end; j++)
      {
         rank = v[j];
         pos = 0;
         #pragma omp parallel for reduction (+:pos)
         for (i = 0; i < N; i++)
            if ( rank > v[i] )
               pos++;
         w[pos] = rank;
      }
      t2 = omp_get_wtime();
   }

   int* vQSort = malloc(sizeof(*vQSort) * N);

   for (i = 0; i != N; ++i)
   {
      vQSort[i] = v[i];
   }
   qsort(vQSort, N, sizeof(int), cmp);

   compareVectors(w, vQSort);
   printf("Duration: %f\n",t2-t1);
   return 0;
}