#include <stdio.h>
#include <stdlib.h>
#define MAX_NODES 100
#include <string.h>
#define HEAP_SIZE 1024
#define MAX_NODES 100

// Structure to represent a block of memory in the heap
typedef struct Block {
    size_t size;
    int free; // 1 if the block is free, 0 if it's allocated
    struct Block *next; // Pointer to the next block in the linked list
} Block;

// Structure to represent the heap
typedef struct {
    int adjacency_matrix[MAX_NODES][MAX_NODES];
    int node_count;
} Heap;

// Define structure for a node in adjacency list
typedef struct Node {
    int data;
    struct Node* next;
    int marked;
} Node;

// Define structure for adjacency list
typedef struct {
    Node** array;
    int numVertices;
} Graph;

// The heap itself
static Block *heap_start = NULL;

// Function to initialize the heap
void init_heap() {
    heap_start = (Block *)malloc(HEAP_SIZE);
    if (heap_start == NULL) {
        fprintf(stderr, "Error: Unable to initialize heap\n");
        exit(1);
    }
    heap_start->size = HEAP_SIZE - sizeof(Block);
    heap_start->free = 1;
    heap_start->next = NULL;
}

// Function to allocate memory from the heap
void *alloc(size_t size) {
    // Traverse the linked list to find a suitable free block
    Block *curr = heap_start;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // If the block is free and large enough, allocate from it
            if (curr->size > size + sizeof(Block)) {
                // Split the block if it's larger than the requested size
                Block *new_block = (Block *)((char *)curr + sizeof(Block) + size);
               //to perform pointer arithmetic in terms of bytes rather than in terms of the size of the structure (Block structure) it points to.
               	//Block *new_block = curr + size;
                new_block->size = curr->size - size - sizeof(Block);
                new_block->free = 1;
                new_block->next = curr->next;
                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void *)(curr + 1); // Return a pointer to the allocated memory
        }
        curr = curr->next;
    }
    return NULL; // If no suitable block is found, return NULL
}

// Function to free memory allocated from the heap
void free_mem(void *ptr) {
    if (ptr == NULL)
        return;
    Block *block = (Block *)ptr - 1;
    block->free = 1;
    
    // Merge adjacent free blocks
    Block *curr = heap_start;
    while (curr) {
        if (curr->free && curr->next && curr->next->free) {
            curr->size += sizeof(Block) + curr->next->size;
            curr->next = curr->next->next;
        }
        curr = curr->next;
    }
    printf("Block freed successfully!\n");
    
}

// Function to create a new node
Node* createNode(int data) {
    Node* newNode = (Node*)alloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph with 'numVertices' vertices
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)alloc(sizeof(Graph));
    graph->numVertices = numVertices;
    graph->array = (Node**)alloc(numVertices * sizeof(Node*));
    for (int i = 0; i < numVertices; ++i)
        graph->array[i] = NULL;
    return graph;
}

// Function to add an edge to an undirected graph
void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->array[src];
    graph->array[src] = newNode;
}

// Function to convert adjacency list to adjacency matrix
void adjacency_list_to_matrix(Graph* graph, int adj_matrix[][MAX_NODES]) {
    for (int i = 0; i < graph->numVertices; i++) {
        Node* temp = graph->array[i];
        while (temp) {
            adj_matrix[i][temp->data] = 1;
            temp = temp->next;
        }
    }
    
}

// Function to print adjacency matrix
void print_adjacency_matrix(int adj_matrix[][MAX_NODES], int n_nodes) {
    printf("Adjacency Matrix:\n");
    for (int i = 0; i < n_nodes; i++) {
    		if(i!=0 && i!=4 && i!=6){
        for (int j = 0; j < n_nodes; j++) {
        		if(j!=0 && j!=4 && j!=6){
            printf("%d ", adj_matrix[i][j]);
        }}
		 printf("\n");}
    }
}

// Function to initialize the heap
void initialize_heap(Heap *heap, int adjacency_matrix[MAX_NODES][MAX_NODES], int node_count) {
    int i, j;
    heap->node_count = node_count;
    for (i = 0; i < node_count; i++) {
        for (j = 0; j < node_count; j++) {
            heap->adjacency_matrix[i][j] = adjacency_matrix[i][j];
        }
    }
}

// Function to perform depth-first search (DFS) recursively
void DFS(Graph* graph, int vertex, bool* visited) {
    // Mark the current vertex as visited
    visited[vertex] = true;
    // Traverse all adjacent vertices of the current vertex
    Node* current = graph->array[vertex];
    while (current != NULL) {
        int adjacentVertex = current->data;
        if (!visited[adjacentVertex]) {
            // If the adjacent vertex is not visited, recursively call DFS
            DFS(graph, adjacentVertex, visited);
        }
        current = current->next;
    }
}

// Function to perform mark and sweep garbage collection
void mark_and_sweep(Graph* graph) {
    // Initialize the visited array
    bool visited[MAX_NODES] = {false};

    // Mark all reachable nodes starting from vertex 0
    DFS(graph, 5, visited);
	//the output of the mark and sweep may vary from one another depending on the starting vertex of the graph
    printf("\nGarbage nodes:\n");
    int sum = 0;
    for (int i = 0; i < graph->numVertices; ++i) {
    		if(i!=4 && i!=0 && i!=6){
        if (!visited[i] && i!=5) {//as we are stating from the 5th node 
            printf("Node value = %d, Memory freed = %d\n", i, (int)sizeof(Node));
           Node* current = graph->array[i];
            while (current) {
                int next_data = current->data;
                Node* temp = current;
                current = current->next;
                //free(temp); // Free the node
                free_mem((void *)temp);
                graph->array[i] = current;
                printf("Node %d freed.\n", next_data);
            }
            graph->array[i] = NULL;
            sum += sizeof(Node);
        }}
    }
    printf("Total memory freed = %d\n", sum);
}

void check(int adjacency_matrix[MAX_NODES][MAX_NODES],int n,Graph* graph){
	printf("garbage elements: \n");
	int sum=0;
	for(int i=0;i<n;i++){
		if(i!=4 && i!=0 && i!=6 && i!=5){
		int j=0, flag=1;
		while(j<n && flag==1){
			if(adjacency_matrix[j][i]==0){
				j++;
			}
			else{
				flag=0;
			}
		}
		if(flag==1){
			sum=sum+ sizeof(i)+sizeof(Node);
            printf("node value=%d , memory freed=%d\n", i, sizeof(i)+sizeof(Node));
			free(graph->array[i]);
			graph->array[i]=NULL;
			n--;
		}}}
	printf("total memory freed=%d\n", sum);
}

void printAdj_list(Graph* graph){
	int num=graph->numVertices;
	for(int i=0;i<num;i++){
		if(i!=0 && i!=4 && i!=6 ){
		Node* temp=graph->array[i];
			printf("%d->",i);
		while(temp){
			printf("%d ",temp->data); 
			temp=temp->next;
		}
		printf("\n");
		}
	}
}

int main() {
	init_heap();
    int numVertices = 11;
    Graph* graph = createGraph(numVertices);

    addEdge(graph, 1, 9);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 10);
    addEdge(graph, 3, 8);
    addEdge(graph, 3, 10);
    addEdge(graph, 5, 1);
    addEdge(graph, 7, 1);
    addEdge(graph, 7, 8);
    addEdge(graph, 8, 9);
    
    
    // Add pointers root_1 to node 5 and root_2 to node 1
    Node* root_1 = graph->array[5];
    Node* root_2 = graph->array[1];
    
	printf("MARK AND SWEEP\n");
    // Initialize adjacency matrix with all zeros
    int adj_matrix[MAX_NODES][MAX_NODES] = {0};

    // Convert adjacency list to adjacency matrix
    adjacency_list_to_matrix(graph, adj_matrix);
	printf("the required Adjacent matrix is :\n");
    // Print adjacency matrix
    print_adjacency_matrix(adj_matrix, numVertices);
	printf("the required Adjacent list is :\n");
    printAdj_list(graph);
    Heap heap;
    initialize_heap(&heap, adj_matrix, numVertices);
    // Perform mark and sweep garbage collection
    printf("Applying mark and sweep on the given graph :\n");
    mark_and_sweep(graph);
	// DFS_print_unreachable(graph,5);
    //check(adj_matrix, numVertices,graph);
    return 0;
}
