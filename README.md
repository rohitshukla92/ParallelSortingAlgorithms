# ParallelSortingAlgorithms
Parallel Quicksort and Radixsort implementations using Intel Cilk Plus.

Quicksort involves parallel partition to find the pivot element such that the element less than the pivot are on the left of it and elements greater than it are on the right of it. The base case is kept to be 30 and sizes of matrix below 30 are sorted using merge sort. 
