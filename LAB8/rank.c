
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define N 100000
#define NUM_THREADS 4

static int *v, *res;

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

static void* rankSort(void* arg)
{
    int i, j, rank, x;
    long my_rank = (long)arg;
    int start = my_rank * N / NUM_THREADS;
    int end = (my_rank + 1) * N / NUM_THREADS;

    for (i = start; i < end; i++) {
        rank = 0;
        x = v[i];
        for (j = 0; j < N; j++) {
            if (x > v[j]) rank++;
        }
        res[rank] = x;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    long i;
    struct timeval start, end;

    v = (int *)malloc(N * sizeof(int));
    res = (int *)malloc(N * sizeof(int));

    pthread_t* tids = (pthread_t*)malloc(NUM_THREADS * sizeof(pthread_t));

    for (i = 0; i < N; i++)
        v[i] = N - i;

    gettimeofday(&start, NULL);

    for (i = 1; i < NUM_THREADS; i++) {
        pthread_create(&tids[i], NULL, rankSort, (void*)i);
    }

    rankSort(0);

    for (i = 1; i < NUM_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    gettimeofday(&end, NULL);
    printf("Duration: %.4f s\n", end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0);

    int* vQSort = malloc(sizeof(*vQSort) * N);
    for (i = 0; i != N; ++i)
    {
        vQSort[i] = v[i];
    }
    qsort(vQSort, N, sizeof(int), cmp);

    compareVectors(res, vQSort);

    // for(i = 0; i < N; i++)
    //     printf("%d ", res[i]);

    free(v);
    free(res);
    free(tids);
    return 0;
}