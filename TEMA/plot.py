import matplotlib.pyplot as plt

# pthreads speedup
threads = [1, 2, 3, 4, 5, 6]
time1 = [22.45, 14.9423, 9.12311, 6.87993, 5.74457, 6.08241]
plt.plot(threads, time1, label = "pthreads")

time2 = [22.45, 11.9753, 9.429637, 7.42645, 6.88417, 8.20989]
plt.plot(threads, time2, label = "omp")

threads = [1, 2, 3, 4, 5]
time3 = [21.6107, 11.9607, 11.6162, 7.7505, 8.1511]
plt.plot(threads, time3, label = "mpi")

threads = [1, 2, 3, 4]
time3 = [21.562, 11.691, 9.8525, 6.8517]
plt.plot(threads, time3, label = "hibrid")


# # # naming the x axis 
plt.xlabel('number of threads/ processes')
plt.ylabel('Duration (s)') 

# # # show a legend on the plot 
plt.legend() 

# # # function to show the plot 
plt.show()