#include <stdio.h>
#include <stdlib.h>
#define MAX_NODES 100

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

// Function to create a new node
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph with 'numVertices' vertices
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
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


int main() {
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

    return 0;
}

