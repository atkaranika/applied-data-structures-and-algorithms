// CE 6320: Applied Data Structures and Algorithms        //
// Fall 2024 UTD                                          //
// The program is given a graph and applies a heuristic   //
// to solve the clique partitioning problem               //
// Author: Karanika Athanasia                             //
//        #################################               //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_VECTOR_LENGTH 32 // Fixed length for test vectors //

//        ##########  Data Structures  ###########        //

// Structure to store each vertex of fixed size from an   //
// input file (.test)                                     //
typedef struct {
  char vector[TEST_VECTOR_LENGTH + 1]; // 32 bits + null terminator //
  int index;                           // Position in input file    //
} test_vector;

// Structure to maintain a dynamic array of vertices      //
// for subgraphs                                          //
typedef struct {
  int *vertices; // Array of vertex indices            //
  int count;     // Current number of vertices         //
  int capacity;  // Maximum capacity of vertices array //
} sub_graph;

// Structure for adjacency list node representing        //
// graph edges                                           //
typedef struct adj_list_node {
  int vertex;                     // Index of connected vertex //
  struct adj_list_node *next;     
} adj_list_node;


// Structure for adjacency list header //
typedef struct {
  adj_list_node *head;
} adj_list;

//        ##########  Function Prototypes  ###########    //
sub_graph* create_sub_graph(int initial_capacity);
adj_list_node* create_node(int vertex);
void add_vertex(sub_graph* subgraph, int vertex);
void remove_vertex(sub_graph* subgraph, int vertex_index);
void print_graph(adj_list **graph, int num_vertices, test_vector *vectors);
void print_sub_graph(sub_graph* subgraph);
int test_vectors_compatible(char *v1, char *v2);
int read_test_vectors(const char *filename, test_vector **vectors);
adj_list** create_graph(test_vector *vectors, int num_vectors);
void identify_dict(FILE *output_file, adj_list **graph, test_vector *vectors, int num_vertices, int max_cliques);

// Function Implementations //

//        ##########  create_sub_graph ###########        //
// Creates a new subgraph with specified initial capacity //
// @param initial_capacity Initial size of vertices array //
// @return Pointer to new subgraph structure              //
sub_graph* create_sub_graph(int initial_capacity) {
  sub_graph* subgraph = (sub_graph*)malloc(sizeof(sub_graph));
  if (!subgraph) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  subgraph->vertices = (int*)malloc(initial_capacity * sizeof(int));
  if (!subgraph->vertices) {
    fprintf(stderr, "Memory allocation failed\n");
    free(subgraph);
    exit(1);
  }
  subgraph->count = 0;
  subgraph->capacity = initial_capacity;
  return subgraph;
}

//        ##########  create_node ###########            //
// Creates a new adjacency list node                     //
// @param vertex Index of vertex to store in node        //
// @return Pointer to new node                           //
adj_list_node* create_node(int vertex) {
  adj_list_node *new_node = (adj_list_node*)malloc(sizeof(adj_list_node));
  if (!new_node) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  new_node->vertex = vertex;
  new_node->next = NULL;
  return new_node;
}

//        ##########  add_vertex ###########              //
// Adds a vertex to subgraph, doubling capacity if needed //
// @param subgraph Pointer to subgraph structure          //
// @param vertex Vertex index to add                      //
void add_vertex(sub_graph* subgraph, int vertex) {
  int* new_vertices;
  if (subgraph->count >= subgraph->capacity) {
    subgraph->capacity *= 2;
    new_vertices = (int*)realloc(subgraph->vertices, subgraph->capacity * sizeof(int));
    if (!new_vertices) {
      fprintf(stderr, "Memory reallocation failed\n");
      free(subgraph->vertices);
      free(subgraph);
      exit(1);
    }
    subgraph->vertices = new_vertices;
  }
  subgraph->vertices[subgraph->count++] = vertex;
}

//        ##########  remove_vertex ###########          //
// Removes vertex at specified index from subgraph       //
// @param subgraph Pointer to subgraph structure         //
// @param vertex_index Index of vertex to remove         //
void remove_vertex(sub_graph* subgraph, int vertex_index) {
  if (vertex_index < 0 || vertex_index >= subgraph->count) {
    return;
  }
  subgraph->vertices[vertex_index] = subgraph->vertices[--subgraph->count];
}

//        ##########  print_graph ###########            //
// Prints graph representation for debugging             //
// @param graph Adjacency list representation of graph   //
// @param num_vertices Number of vertices in graph       //
// @param vectors Array of test vectors                  //
void print_graph(adj_list **graph, int num_vertices, test_vector *vectors) {
  int i;
  printf("Graph (Adjacency List):\n");
  for (i = 0; i < num_vertices; i++) {
    printf("Vertex %d (%s): ", vectors[i].index, vectors[i].vector);
    adj_list_node *temp = graph[i]->head;
    while (temp) {
      printf("%d (%s) -> ", vectors[temp->vertex].index, vectors[temp->vertex].vector);
      temp = temp->next;
    }
    printf("NULL\n");
  }
}

//        ##########  print_sub_graph ###########        //
// Prints subgraph contents for debugging                //
// @param subgraph Pointer to subgraph structure         //
void print_sub_graph(sub_graph* subgraph) {
  int i;
  printf("Active vertices (count=%d): [ ", subgraph->count);
  for (i = 0; i < subgraph->count; i++) {
    printf("%d ", subgraph->vertices[i]);
  }
  printf("]\n");
}

//        ##########  test_vectors_compatible ###########  //
// Checks if two test vectors are compatible               //
// Two vectors are compatible if they don't have           //
// conflicting bits (0/1). 'x' or 'X' represents           //
// don't care condition                                    //
// @param v1 First test vector                             //
// @param v2 Second test vector                            //
// @return 1 if compatible, 0 if not                       //
int test_vectors_compatible(char *v1, char *v2) {
  int i;
  for (i = 0; i < TEST_VECTOR_LENGTH; i++) {
    if (v1[i] != v2[i] && v1[i] != 'x' && v2[i] != 'x' && v1[i] != 'X' && v2[i] != 'X') {
      return 0;
    }
  }
  return 1;
}

//        ##########  read_test_vectors ###########      //
// Reads test vectors from input file                    //
// @param filename Name of input file                    //
// @param vectors Pointer to array where vectors stored  //
// @return Number of vectors read                        //
int read_test_vectors(const char *filename, test_vector **vectors) {
  int count = 0;
  int capacity = 7000;
  test_vector *new_vectors;
  FILE *file;
  
  *vectors = (test_vector*)malloc(capacity * sizeof(test_vector));
  if (!*vectors) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed to open file: %s\n", filename);
    free(*vectors);
    exit(1);
  }

  while (fscanf(file, "%s", (*vectors)[count].vector) != EOF) {
    (*vectors)[count].index = count + 1;
    count++;

    if (count >= capacity) {
      capacity *= 2;
      new_vectors = (test_vector*)realloc(*vectors, capacity * sizeof(test_vector));
      if (!new_vectors) {
        fprintf(stderr, "Memory reallocation failed\n");
        free(*vectors);
        fclose(file);
        exit(1);
      }
      *vectors = new_vectors;
    }
  }

  fclose(file);
  return count;
}

//        ##########  create_graph ###########           //
// Creates graph representation of test vectors          //
// Two vertices are connected if their test vectors      //
// are compatible                                        //
// @param vectors Array of test vectors                  //
// @param num_vectors Number of vectors                  //
// @return Adjacency list representation of graph        //
adj_list** create_graph(test_vector *vectors, int num_vectors) {
  adj_list **graph = (adj_list**)malloc(num_vectors * sizeof(adj_list*));
  int i,j;
  adj_list_node *new_node1, *new_node2;
  if (!graph) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  for (i = 0; i < num_vectors; i++) {
    graph[i] = (adj_list*)malloc(sizeof(adj_list));
    if (!graph[i]) {
      fprintf(stderr, "Memory allocation failed\n");
      // Clean up previously allocated memory
      for (j = 0; j < i; j++) {
        free(graph[j]);
      }
      free(graph);
      exit(1);
    }
    graph[i]->head = NULL;
  }

  for (i = 0; i < num_vectors; i++) {
    for (j = i + 1; j < num_vectors; j++) {
      if (test_vectors_compatible(vectors[i].vector, vectors[j].vector)) {
        // Add edges in both directions
        new_node1 = create_node(j);
        new_node1->next = graph[i]->head;
        graph[i]->head = new_node1;

        new_node2 = create_node(i);
        new_node2->next = graph[j]->head;
        graph[j]->head = new_node2;
      }
    }
  }

  return graph;
}

//                         ##########  identify_dict ###########                             //
// identify_dict - Identifies dictionary entries using a greedy clique-finding approach      //
// This function implements a greedy algorithm to find maximal cliques in the compatibility  //
// graph, where each clique represents a potential dictionary entry. The algorithm:          //
// 1. Selects vertex with highest degree as seed for each clique                             //
// 2. Grows clique by iteratively adding highest-degree neighbors                            //
// 3. Updates dictionary entries based on test vectors in the clique                         //
void identify_dict(FILE *output_file, adj_list **graph, test_vector *vectors, int num_vertices, int max_cliques) {
  int *visited = (int*)calloc(num_vertices, sizeof(int));
  int num_cliques_found = 0;                                // Track number of cliques found  //
  char current_dict_entry[TEST_VECTOR_LENGTH + 1];          // Buffer for building dictionary entries //
  int max_degree;
  int max_vertex;
  adj_list_node *temp;
  int i,j;
  int degree, vertex;
  int sub_max_degree = -1;
  int sub_max_vertex = -1;
  int max_index = -1;
  sub_graph *new_subgraph;
  sub_graph *subgraph;
  int clique_size = 1;

  if (!visited) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  // Main loop - continue until we find max_cliques or no more cliques possible //
  while (num_cliques_found < max_cliques) {
    // Reset dictionary entry template with don't-care bits //
    memset(current_dict_entry, 'X', TEST_VECTOR_LENGTH);
    current_dict_entry[TEST_VECTOR_LENGTH] = '\0';
    clique_size = 1;

    // Find unvisited vertex with highest degree (most connections) //
    // This vertex likely leads to larger cliques                   //
    max_degree = -1;
    max_vertex = -1;
    
    for (i = 0; i < num_vertices; i++) {
      if (!visited[i]) {
        degree = 0;
        // Count unvisited neighbors to get current degree //
        temp = graph[i]->head;
        while (temp) {
          if (!visited[temp->vertex]) {
            degree++;
          }
          temp = temp->next;
        }
        if (degree > max_degree) {
          max_degree = degree;
          max_vertex = i;
        }
      }
    }

    // Exit if no unvisited vertices remain //
    if (max_vertex == -1) break;

    // Initialize subgraph with unvisited neighbors of max_vertex //
    subgraph = create_sub_graph(num_vertices);
    temp = graph[max_vertex]->head;
    while (temp) {
      // Add only unvisited neighbors of max_vertex
      if (!visited[temp->vertex]) {
        add_vertex(subgraph, temp->vertex);
      }
      temp = temp->next;
    }

    // Update dictionary entry with bits from max_vertex vector //
    // Only update positions that are still don't-care ('X')    //
    for (j = 0; j < TEST_VECTOR_LENGTH; j++) {
      if (current_dict_entry[j] == 'X') {
        current_dict_entry[j] = vectors[max_vertex].vector[j];
      }
    }
    visited[max_vertex] = 1;

    // Iteratively grow the clique by adding highest degree vertices //
    while (subgraph->count > 0) {
      // Find vertex in subgraph with highest degree among remaining vertices //
      sub_max_degree = -1;
      sub_max_vertex = -1;
      max_index = -1;

      // Find vertex with maximum degree in subgraph //
      for (i = 0; i < subgraph->count; i++) {
        vertex = subgraph->vertices[i];
        degree = 0;
        adj_list_node *temp = graph[vertex]->head;
        
        // Count unvisited neighbors //
        while (temp) {
          if (!visited[temp->vertex]) {
          // Count unvisited neighbors that are in the subgraph //
          for (j = 0; j < subgraph->count; j++) {
            if (subgraph->vertices[j] == temp->vertex) {
              degree++;
              break;
            }
          }
          }
          temp = temp->next;
        }

        if (degree > sub_max_degree) {
          sub_max_degree = degree;
          sub_max_vertex = vertex;
          max_index = i;
        }
      }

      if (sub_max_vertex == -1) break;

      // Update dictionary entry and mark vertex as visited //
      for (j = 0; j < TEST_VECTOR_LENGTH; j++) {
        if (current_dict_entry[j] == 'X') {
          current_dict_entry[j] = vectors[sub_max_vertex].vector[j];
        }
      }
      visited[sub_max_vertex] = 1;
      clique_size++;
      remove_vertex(subgraph, max_index);

      // Create new subgraph containing only vertices connected to sub_max_vertex  //
      // This ensures we maintain clique property                                  //
      new_subgraph = create_sub_graph(subgraph->count);
      temp = graph[sub_max_vertex]->head;
      while (temp) {
        if (!visited[temp->vertex]) {
          for (i = 0; i < subgraph->count; i++) {
            if (subgraph->vertices[i] == temp->vertex) {
              add_vertex(new_subgraph, temp->vertex);
              break;
            }
          }
        }
          temp = temp->next;
      }

      // Replace old subgraph with new filtered subgraph //
      free(subgraph->vertices);
      free(subgraph);
      subgraph = new_subgraph;
    }

    if (clique_size == 1){
      break;
    }
    // Write dictionary entry to file //
    fprintf(output_file, "%s\n", current_dict_entry);
    printf("Clique %d: %s\n", num_cliques_found + 1, current_dict_entry);
    printf("Clique size %d \n", clique_size);
    num_cliques_found++;

    // Clean up subgraph //
    free(subgraph->vertices);
    free(subgraph);
  }

  if (num_cliques_found < max_cliques) {
    printf("Only %d dictionary entries are possible\n", num_cliques_found);
  }

  free(visited);
}

// remove_duplicates - Removes duplicate test vectors from the array //
int remove_duplicates(test_vector **vectors, int num_vectors) {
  int i, j, k;
  int new_size = num_vectors;
  
  for (i = 0; i < new_size; i++) {
    for (j = i + 1; j < new_size; ) {
      if (strcmp((*vectors)[i].vector, (*vectors)[j].vector) == 0) {
        // Shift remaining vectors left //
        for (k = j; k < new_size - 1; k++) {
          (*vectors)[k] = (*vectors)[k + 1];
        }
        new_size--;
      } else {
        j++;
      }
    }
  }
  
  // Reallocate memory to new size if needed
  if (new_size < num_vectors) {
    *vectors = realloc(*vectors, new_size * sizeof(test_vector));
  }
  return new_size;
}

int main(int argc, char *argv[]) {
  test_vector *vectors;
  int num_vectors;
  int dict_size = atoi(argv[2]);
  int vector_length = atoi(argv[3]);
  int i;
  adj_list **graph;
  FILE *output_file;

  if (argc != 5) {
    fprintf(stderr, "Usage: %s <input_file> <dict_size> <vector_length> <output_file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  if (vector_length != TEST_VECTOR_LENGTH) {
    fprintf(stderr, "Unsupported vector length\n");
    free(vectors);
    return EXIT_FAILURE;
  }
  num_vectors = read_test_vectors(argv[1], &vectors);
  output_file = fopen(argv[4], "w");
  if (!output_file) {
    fprintf(stderr, "Failed to open output file: %s\n", argv[4]);
    free(vectors);
    return EXIT_FAILURE;
  }
  num_vectors = remove_duplicates(&vectors, num_vectors);

  graph = create_graph(vectors, num_vectors);
  identify_dict(output_file, graph, vectors, num_vectors, dict_size);

  // Cleanup //
  for (i = 0; i < num_vectors; i++) {
    adj_list_node *current = graph[i]->head;
    while (current) {
      adj_list_node *next = current->next;
      free(current);
      current = next;
    }
    free(graph[i]);
  }
  free(graph);
  free(vectors);
  fclose(output_file);

  return EXIT_SUCCESS;
}