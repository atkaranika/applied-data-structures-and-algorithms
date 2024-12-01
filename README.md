# applied-data-structures-and-algorithms
Clique Partitioning Problem Solver
A C implementation for solving the Clique Partitioning Problem using a heuristic approach, developed for CE 6320: Applied Data Structures and Algorithms at UTD (Fall 2024).

Overview
This program implements a greedy algorithm to solve the clique partitioning problem on graphs, specifically designed for test vector compatibility analysis. It finds maximal cliques in a compatibility graph where vertices represent test vectors and edges indicate compatibility between vectors.

Features
Custom data structures for handling test vectors and graph representation
Efficient adjacency list implementation for graph storage
Greedy clique-finding algorithm
Support for "don't care" conditions ('x' or 'X') in test vectors
Duplicate test vector removal
Dynamic memory management for varying input sizes


Installation
Clone the repository:
bash

Copy
git clone [repository-url]
cd clique-partitioning-solver
Compile the program:
bash

Copy
gcc -o program main.c
Usage
Run the program using:

bash

Copy
./program <input_file> <dict_size> <vector_length> <output_file>
Parameters:
input_file: Path to the file containing test vectors
dict_size: Maximum number of cliques to find
vector_length: Length of test vectors (must be 32)
output_file: Path to file where dictionary entries will be written
Input File Format
Each line contains one test vector
Test vectors must be 32 bits long
Valid characters: '0', '1', 'x', 'X'
Output Format
Each line contains one dictionary entry
Dictionary entries are 32 bits long
Contains '0', '1', and 'X' (don't care) values
Algorithm
The program employs a greedy approach to find maximal cliques:

Selects vertex with highest degree as seed for each clique
Grows clique by iteratively adding highest-degree neighbors
Updates dictionary entries based on test vectors in the clique
Implementation Details
Data Structures
test_vector: Stores individual test vectors and their indices
sub_graph: Maintains dynamic arrays of vertices for subgraphs
adj_list_node: Represents graph edges in adjacency list
adj_list: Header structure for adjacency lists
Memory Management
Dynamic allocation for varying input sizes
Proper cleanup of allocated resources
Error handling for memory allocation failures
Limitations
Fixed test vector length of 32 bits
Maximum dictionary size depends on input compatibility
Author
Karanika Athanasia

License
This project is licensed under the MIT License - see the LICENSE file for details

Contributing
