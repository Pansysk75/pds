

# Finding the Strongly Connected Components of a directed graph in a shared memory multiprocessor

*Assignment for 2022-2023 Parallel and Distributed Systems course, Aristotle University of Thessaloniki*

A strongly connected component (SCC) is a maximal subgraph of a directed graph in which there is a path 
from each vertex to any other vertex in the subgraph.  

In this assignment, I'm implementing a parallel algorithm to identify the number of SCCs in large directed graphs, using OpenCilk, OpenMP and PThreads.

## Dependencies:  
- gcc/g++ compiler
- `make` for building using Makefile
- OpenCilk installation (default directory for the OpenCilk compiler is /opt/opencilk/bin/clang++ , modify the first line of the Makefile if somewhere different).

## How to build:  
- Navigate to the directory of the Makefile and run `make` command. Executables should be created in folder `./exec/` .

## How to run:
- Obtain .mtx files that represent graphs.
- For any of the executables that were created, run `./exec/some_exe my_graph.mtx` (an additional number may be passed to indicate running the benchmark for more iterations, e.g. `exec/pthreads celegansneural.mtx 500`)
