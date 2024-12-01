# Clique Partitioning Problem Solver

A C implementation for solving the Clique Partitioning Problem using a heuristic approach, developed for CE 6320: Applied Data Structures and Algorithms at UTD (Fall 2024).

## Overview

This program implements a greedy algorithm to solve the clique partitioning problem on graphs, specifically designed for test vector compatibility analysis. It finds maximal cliques in a compatibility graph where vertices represent test vectors and edges indicate compatibility between vectors.

## Features

- Custom data structures for handling test vectors and graph representation
- Efficient adjacency list implementation for graph storage  
- Greedy clique-finding algorithm
- Support for "don't care" conditions ('x' or 'X') in test vectors
- Duplicate test vector removal
- Dynamic memory management for varying input sizes
