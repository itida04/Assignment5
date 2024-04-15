#include <stdio.h>
#include <stdlib.h>
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
    int reference_counts[MAX_NODES];
    int node_count;
} Heap;

// Define structure for a node in adjacency list
typedef struct Node {
    int data;
    struct Node* next;
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
   // Node* newNode = (Node*)malloc(sizeof(Node));
    Node* newNode =(Node *)alloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph with 'numVertices' vertices
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)alloc(sizeof(Graph));
    graph->numVertices = numVertices;
    graph->array = (Node**)malloc(numVertices * sizeof(Node*));
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
        heap->reference_counts[i] = 0;
    }
}

// Function to increment reference count for a node
void increment_reference_count(Heap *heap, int node) {
    heap->reference_counts[node]++;
}

// Function to decrement reference count for a node
void decrement_reference_count(Heap *heap, int node) {
    heap->reference_counts[node]--;
}

// Function to mark references in the heap
void mark_references(Heap *heap) {
    int i, j;
    for (i = 0; i < heap->node_count; i++) {
        for (j = 0; j < heap->node_count; j++) {
            if (heap->adjacency_matrix[i][j] == 1) {
                increment_reference_count(heap, j);
            }
        }
    }
	heap->reference_counts[1]++;//due to root_2
	heap->reference_counts[5]++;//due to root_1
}

// Function to find garbage nodes
void find_garbage_nodes(Heap *heap) {
    int i;
    printf("Garbage nodes:\n");
    int sum=0;
    for (i = 0; i < heap->node_count; i++) {
        if ((heap->reference_counts[i] == 0) && (i!=4 && i!=0 && i!=6)) {
        	sum=sum+ sizeof(i)+sizeof(Node);
            printf("node value=%d , memory freed=%d\n", i, sizeof(i)+sizeof(Node));
        }
    }
     printf("total memory freed=%d\n", sum);
}

void printAdj_list(Graph* graph){
	int num=graph->numVertices;
	for(int i=0;i<num;i++){
		if(i!=0 && i!=4 && i!=6){
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

// Function to free and merge nodes in the graph based on their reference counts
void free_and_merge_nodes(Graph* graph, Heap *heap) {
    for (int i = 0; i < graph->numVertices; i++) {
        if (heap->reference_counts[i] == 0) {
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
        }
    }
    printf("Freeing and merging nodes completed successfully!\n");
}


int main() {
	init_heap();
    int numVertices = 11;
    Graph* graph = createGraph(numVertices);
	printf("REFERENCE COUNTING\n");
    addEdge(graph, 1, 9);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 10);
    addEdge(graph, 3, 8);
    addEdge(graph, 3, 10);
    addEdge(graph, 5, 1);
    addEdge(graph, 7, 1);
    addEdge(graph, 7, 8);
    addEdge(graph, 8, 9);
    printf("the required adjacent list is :\n");
	printAdj_list(graph);

    // Initialize adjacency matrix with all zeros
    int adj_matrix[MAX_NODES][MAX_NODES] = {0};

    // Convert adjacency list to adjacency matrix
    adjacency_list_to_matrix(graph, adj_matrix);

	printf("the required adjacent matrix is :\n");
    // Print adjacency matrix
    print_adjacency_matrix(adj_matrix, numVertices);
    
    Heap heap;
    initialize_heap(&heap, adj_matrix, numVertices);
    mark_references(&heap);
    printf("refrence counting done successfully:\n");
    printf("freeing the node with zero reference count and displaying along with the memory freed:\n");
    find_garbage_nodes(&heap);
	free_and_merge_nodes(graph,&heap);
	printf("code performs well");
    return 0;
}

