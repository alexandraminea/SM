# Conversie schema de culori RGB -> HSL

# Rulare
```console
$ make serial
$ make pthreads
$ make openmp
$ ./serial test3
$ ./pthreads test3
$ ./openmp test3
```

## Input si output

Imaginile test sunt in folederul input, iar dupa rularea programului, imaginile editate sunt adaugate in folderul out.
Cea mai mare imagine(pe care se vor face testele relevante in continuare) are dimeniunea 18848 x 13621(pixels).
<br/>
![Alt text](captures/huge.PNG?raw=true "Image")
<br/>
Pentru aceasta imagine, timpul de rulare seriala este de aproximativ 20-25s.

## Implementare seriala

Programul citeste o imagine in format .bmp (bitmap), efectueaza operatiile la nivel de pixel si compune din acestia o noua imagine.
*Obs: Pentru analiza implementarilor paralele am masurat durata executiei operatiilor de conversie.


## Implemetare cu pthreads

Timp rulare paralela:
    1 thread - 22.45s
    2 threaduri - 14.9423s
    3 threaduri - 9.12311s
    4 threaduri - 6.87993s
    5 threaduri - 5.74457s
    6 threaduri - 6.08241s

    ... aici timpii incep sa creasca

Acesti timpi sunt justificati de numerul de core-uri = 4

## Implementare openmp

Timp rulare paralela:
    1 thread    -   22.45s
    2 threaduri -   11.9753s
    3 threaduri -   9.429637
    4 threaduri -   7.42645s
    5 threaduri -   6.88417s
    6 threaduri -   8.20989s

## Implementare mpi

Procesul MASTER citeste imaginea si o scrie la final.
Se face Scatter matricei de pixeli, fiecare proces face conversia pe chunk-ul sau, iar apoi se face Gather cu matricea modificata.

    1 proc - 21.6107s
    2 proc - 11.9607s
    3 proc - 11.6162s
    4 proc - 7.7505 s
    5 proc - 8.1511 s

## Implementare hibrida MPI + OpenMP

OMP_NUM_THREADS = constant(4)

    1 proc - 21.562 s
    2 proc - 11.691 s
    3 proc - 9.8525 s
    4 proc - 6.8517 s