/****************************************************
 *
 * Authors: Renato Borges & Ramayane Braga
 *
 * To run: ./dijkstra < USA-road-d.NY.gr > log.txt
 *
 ****************************************************/

#include <math.h>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include "CPUTimer.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;

list< pair<int, int> > *graph;

// total number of vertices and arcs
int n, m;

// number of vertices to test
int k;

// the vertices with the min, max and middle degrees in the graph
int *min_degree_vertices;
int *max_degree_vertices;
int *mid_degree_vertices;

/**
 * Defines a node in the heap.
 */
typedef struct {
	int vertex;
	int dist;
} node;

/**
 * Defines the heap structure, which is a 1-dimensional matrix of nodes.
 */
typedef struct {
	node *info;
	int size;
} heap;

node* get_child(heap *h, int i, int child) {
	if (3 * i + child > h->size) {
		return NULL;
	}
	return &(h->info[3 * i + child]);
}

node new_node(int vertex, int dist) {
	node n;
	n.vertex = vertex;
	n.dist = dist;
	return n;
}

/**
 * Returns the position of the min child if exists; otherwise, -1.
 */
int find_min_child(heap *h, int i) {
	int father_value = h->info[i].dist;
	
	node *child1 = get_child(h, i, 1);
	node *child2 = get_child(h, i, 2);
	node *child3 = get_child(h, i, 3);
	
	if (child1 != NULL && child1->dist < father_value) {
		if (child2 != NULL && child1->dist < child2->dist) {
			if (child3 != NULL && child1->dist < child3->dist) {
				return 3 * i + 1;
			}
			return 3 * i + 3;
		}
		return 3 * i + 2;
	}
	
	if (child2 != NULL && child2->dist < father_value) {
		if (child3 != NULL && child2->dist < child3->dist) {
			return 3 * i + 2;
		}
		return 3 * i + 3;
	}
	
	if (child3 != NULL && child3->dist < father_value) {
		return 3 * i + 3;
	}
	
	return -1;
}

/**
 * Remove and return the top node of the heap h.
 */
node pop(heap *h) {
	if (h->size <= 0) {
		return new_node(-1, -1);
	}
	
	node top = h->info[0];
	
	// brings last node to root and decreases heap size
	h->info[0] = h->info[h->size - 1];
	h->size -= 1;
	
	// finds new top node right positioning
	int current_pos = 0;
	while (true) {
		// find min child
		int min_child_pos = find_min_child(h, current_pos);
		
		if (min_child_pos == -1) {
			break;
		}
		
		// swap
		node temp = h->info[current_pos];
		h->info[current_pos] = h->info[min_child_pos];
		h->info[min_child_pos] = temp;
		
		current_pos = min_child_pos;
	}
	
	return top;
}

/**
 * Insert the node u in the heap h.
 */
void insert(heap *h, node u) {
	h->size += 1;
	h->info[h->size - 1] = u;
	
	// reorganize
	int current_pos = h->size - 1;
	while (current_pos > 0 && h->info[current_pos].dist < h->info[(current_pos - 1) / 3].dist) {
		node temp = h->info[current_pos];
		h->info[current_pos] = h->info[(current_pos - 1) / 3];
		h->info[(current_pos - 1) / 3] = temp;
		current_pos = (current_pos - 1) / 3;
	}
}

heap* init(int size) {
	heap *h = (heap*) malloc(sizeof(heap));
	h->info = (node*) malloc(sizeof(node) * size);
	h->size = 0;
	return h;
}

int empty(heap *h) {
	return h->size == 0 ? 1 : 0;
}

int* dijkstra(int source, char log_filename[]) {
	FILE *log_file = fopen(log_filename, "w");
	
	if (log_file == 0) {
		cout << "Can't open file: " << log_filename << endl;
		exit(1);
	}
	
	heap *h = init(n);
	
	int *dist = (int*) malloc(sizeof(int) * n);
	int visited[n];
	
	// dijkstra's initialization step
	for (int i = 0; i < n; i++) {
		dist[i] = numeric_limits<int>::max();
		visited[i] = 0;
	}
	
	// set source node
	dist[source] = 0;
	visited[source] = 1;
	node u = new_node(source, 0);
	insert(h, u);
	
	int iteration = 1;
	
	while (!empty(h)) {
		// saves only every thousand iterations to makes plotting easier
		if (iteration % 1000 == 0) {
			fprintf(log_file, "%d;%d;%d\n", iteration/1000, h->size, int(ceil(log(2 * h->size + 1) / log(3)) - 1));
		}
		
		// get vertex with min distance
		u = pop(h);
		visited[u.vertex] = 1;
		
		// process each neighbour v of vertex
		list< pair<int, int> >::iterator itr = graph[u.vertex].begin();
		while (itr != graph[u.vertex].end()) {
		
			int v = (*itr).first;
			int v_weight = (*itr).second;
			
			if (!visited[v]) {
				// if there is a shorter path to v through u, then update the distance
				if ((dist[u.vertex] + v_weight) < dist[v]) {
					dist[v] = dist[u.vertex] + v_weight;
					insert(h, new_node(v, dist[v]));
				}
			}
			
			++itr;
		}
		
		iteration++;
	}

	fclose(log_file);
	
	return dist;
}

/**
 * This graph was used to test the dijkstra's implementation.
 * At beginning I was using a matrix to represent the graph,
 * but due to performance issue I later changed it to a adjacency list.
 */
int** init_graph() {
	int SIZE = 5;
	int** graph = (int**) malloc(sizeof(int*) * SIZE);

	for (int i = 0; i < SIZE; i++) {
		graph[i] = (int*) malloc(sizeof(int) * SIZE);
		for (int j = 0; j < SIZE; j++) {
			graph[i][j] = 0;
		}
	}

	graph[0][1] = 1;
	graph[0][2] = 12;
	graph[1][2] = 3;
	graph[1][3] = 8;
	graph[2][0] = 2;
	graph[2][3] = 2;
	graph[2][4] = 10;
	graph[3][4] = 10;
	
	return graph;
}

int compare (const void * a, const void * b) {
	node node_a = *(node*)a;
	node node_b = *(node*)b;
	return (node_a.dist - node_b.dist);
}

void build_graph_from_file() {
	int v1, v2, weight;

	cin >> n >> m;
	
	cout << "Number of vertices (n)\t=\t" << n << endl;
	cout << "Number of arcs (m)\t=\t" << m << endl;
	
	// a node is used here as a (vertex, degree) pair
	node* degree = new node[n];
	
	graph = new list< pair<int, int> >[n];
	
	for (int i = 0; i < n; i++) {
		degree[i] = new_node(i, 0);
	}
	
	
	// read all lines until end of file
	for (int i = 0; i < m; i++) {
		cin >> v1 >> v2 >> weight;
		
		// adds to adjacency list and increases degree
		graph[v1 - 1].push_back(make_pair(v2 - 1, weight));
		degree[v1 - 1] = new_node(v1 - 1, degree[v1 - 1].dist + 1);
	}
	
	// code below finds the min, max and middle degree vertices
	
	qsort(degree, n, sizeof(node), compare);
	
	int max = degree[n - 1].dist;
	int min = degree[0].dist;
	
	int i = n - 2;
	int j = 1;
	
	while (max == degree[i].dist) {
		i--;
	}
	
	while (min == degree[j].dist) {
		j++;
	}
	
	// normalizing to the correct amount of nodes with maximum degree
	i = n - i - 1;
	
	k = j < i ? j : i;
	cout << "Number of vertices with minimum degree found\t=\t" << j << endl;
	cout << "Number of vertices with maximum degree found\t=\t" << i << endl;
	cout << "k\t\t\t=\t" << k << endl;
	
	// select the nodes with min, mid and max degrees
	min_degree_vertices = new int[k];
	mid_degree_vertices = new int[k];
	max_degree_vertices = new int[k];
	
	// finds the position of the vertex with middle degree
	int pos_mid = 0;
	int mid_degree = (degree[0].dist + degree[n - 1].dist) / 2;
	while (pos_mid < n && degree[pos_mid].dist != mid_degree) {
		pos_mid++;
	}

	// finally set the vertices
	for (int it = 0; it < k; it++) {
		min_degree_vertices[it] = degree[it].vertex;
		max_degree_vertices[it] = degree[n - it - 1].vertex;
		mid_degree_vertices[it] = degree[pos_mid + it].vertex;
	}
	
	cout << "Minimum degree\t\t=\t" << degree[0].dist << endl;
	cout << "Middle degree\t\t=\t" << mid_degree << endl;
	cout << "Maximum degree\t\t=\t" << degree[n - 1].dist << endl;
	
	delete[] degree;
}

/*
void print_heap(heap *h) {
	for (int i = 0; i < h->size; i++) {
		cout << "(" << h->info[i].vertex << "," << h->info[i].dist << ")" << endl;
	}
	cout << endl;
}
*/

int main(int argc, char* argv[]) {
	
	/* The code commented below is to test the heap
	 
	heap *h = init(15);
	
	insert(h, new_node(0, 8));
	insert(h, new_node(0, 7));
	insert(h, new_node(0, 5));
	insert(h, new_node(0, 2));
	insert(h, new_node(0, 9));
	insert(h, new_node(0, 6));
	insert(h, new_node(0, 1));
	insert(h, new_node(0, 3));
	
	print_heap(h);
	
	node v;
	
	v = pop(h);
	cout << "Popped (" << v.vertex << "," << v.dist << ")" << endl;
	
	v = pop(h);
	cout << "Popped (" << v.vertex << "," << v.dist << ")" << endl;
	
	v = pop(h);
	cout << "Popped (" << v.vertex << "," << v.dist << ")" << endl << endl;
	
	print_heap(h);
	*/
	
	build_graph_from_file();
	
	CPUTimer timer;
	
	float min_degree_exec_time = 0;
	float mid_degree_exec_time = 0;
	float max_degree_exec_time = 0;
	
	for (int i = 0; i < k; i++) {
		char result_min[12];
		char result_mid[12];
		char result_max[12];
		
		sprintf(result_min, "result_min%d", i);
		sprintf(result_mid, "result_mid%d", i);
		sprintf(result_max, "result_max%d", i);
		
		timer.start();
		dijkstra(min_degree_vertices[i], result_min);
		timer.stop();
		
		min_degree_exec_time += timer.getCronoTotalSecs();
		cout << "Execution time for minimum degree vertex " << i << " (seconds)\t=\t" << timer.getCronoTotalSecs() << endl;
		
		timer.reset();
		
		timer.start();
		dijkstra(mid_degree_vertices[i], result_mid);
		timer.stop();
		
		mid_degree_exec_time += timer.getCronoTotalSecs();
		cout << "Execution time for middle degree vertex " << i << " (seconds)\t=\t" << timer.getCronoTotalSecs() << endl;
		
		timer.reset();
		
		timer.start();
		dijkstra(max_degree_vertices[i], result_max);
		timer.stop();
		
		max_degree_exec_time += timer.getCronoTotalSecs();
		cout << "Execution time for maximum degree vertex " << i << " (seconds)\t=\t" << timer.getCronoTotalSecs() << endl;
		
		timer.reset();
	}
	
	cout << "Average execution time for minimum degree (seconds)\t=\t" << min_degree_exec_time/k << endl;
	cout << "Average execution time for middle degree (seconds)\t=\t" << mid_degree_exec_time/k << endl;
	cout << "Average execution time for maximum degree (seconds)\t=\t" << max_degree_exec_time/k << endl;
	
	delete[] min_degree_vertices;
	delete[] mid_degree_vertices;
	delete[] max_degree_vertices;
	delete[] graph;
	
	return 0;
}
