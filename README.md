# KMeans-Clustering-Implementation
KMeans-Clustering-Implementation: Efficient K-Means clustering in C and Python. Partition data into K clusters for analysis and insights

## Overview

This repository contains two implementations of the K-Means clustering algorithm, one in C and another in Python. K-Means is a popular unsupervised machine learning technique used to partition data into K clusters, where K represents the number of desired clusters. The implementations efficiently calculate cluster centroids and iteratively update them until convergence or a specified maximum number of iterations. The algorithms aim to find optimal cluster centers that minimize the sum of squared distances between data points and their corresponding centroids.

## File Descriptions

- `kmeans_clustering.c`: C implementation of the K-Means clustering algorithm.
- `kmeans_clustering.py`: Python script implementing the K-Means clustering algorithm.

## How to Use

### C Implementation

1. Compile the C program using a C compiler.
2. Run the compiled executable with the required command-line arguments:
   - The number of clusters (K).
   - An optional maximum number of iterations.
   - The input file containing data points as vectors.

### Python Implementation

1. Make sure you have Python installed on your system.
2. Run the Python script with the necessary command-line arguments:
   - The number of clusters (K).
   - An optional maximum number of iterations.
   - The input file containing data points as vectors.

Both implementations will perform K-Means clustering on the input data, display the resulting centroids, and terminate when convergence is achieved or the specified maximum number of iterations is reached.

