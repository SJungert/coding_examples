//Samuel Jungert
//A01241879
// Ubuntu Test
#include "BFSGraph.h"

BFSGraph::BFSGraph(int n_input) { // constructor
	//Private Variables
	color = new colorType[n_input];
	pre = new int[n_input];
	dis = new int[n_input];
	//Public Variables
	adj = new Vertex *[n_input];
	n = n_input;

	for (int i = 0; i < n_input; i++) {
		color[i] = WHITE;
		pre[i] = n_input;
		dis[i] = n_input;
		adj[i] = NULL;
	}
};

void BFSGraph::SetAdjLists(int * adjM) { // build the adjacency lists from the adjacency matrix adjM
	for (int i = 0; i < n; i++) {
		for (int j = n - 1; j >= 0; j--) {
			if (adjM[i*n+j] == 1){
				//create a new node and add it to the front of the adjList[i]
				Vertex *v = new Vertex(j);
				v->next = adj[i];
				adj[i] = v;
			}
		}
	}
};

void BFSGraph::PrintAdjLists() { // print the adjacency lists of the graph
	for (int i = 0; i < n; i++) {
		std::cout << i << ": ";
		Vertex * currentNode = adj[i];

		if (currentNode == NULL) {
			std::cout << "NULL" << std::endl;
		}
		else {
			while (currentNode != NULL) {
				std::cout << currentNode->id << " ";
				currentNode = currentNode->next;
			}
		}
		std::cout << std::endl;
	}
};// the following two functions are for the BFS traversal as we discussed in class

void BFSGraph::BFS(int id) { // BFS traversal, id is the source vertex, with default 0
	std::cout << "BFS Traversal: ";
	BFSVisit(id);
	for (int i = 0; i < n; i++) {
		if (color[i] == WHITE) {
			BFSVisit(i);
			//makes sure we are visiting verticies that cannot be reached from s
		}
	}
};

void BFSGraph::BFSVisit(int id) { // actually does BFS, search a connected component from id
	color[id] = GRAY;
	dis[id] = 0;
	int frontIndex = 0;
	int rearIndex = 0;
	//Queue
		int * queue = new int[n+1];
		for (int i = 0; i < n+1; i++) {
			queue[i] = -1;
		}
		int * front = &queue[frontIndex];
		int * rear = &queue[rearIndex];
	//Enqueue
	queue[0] = id;
	std::cout << *front << " ";
	while (*front != -1) {
		//Dequeue
		int u = *front;
		Vertex * v = adj[u];
		while (v != NULL) {
			if (color[v->id] == WHITE) {
				color[v->id] = GRAY;
				dis[v->id] = dis[u] + 1;
				pre[v->id] = u;
				rear = &queue[rearIndex++];
				*rear = v->id;
				std::cout << *rear << " ";
			}
			v = v->next;
		}
		color[u] = BLACK;
		front = &queue[frontIndex++];
	}
	std::cout << std::endl;
};

void BFSGraph::PrintSP(int source, int v) { // Print the shortest path from the source to v and the
	std::cout << "Shortest Path from " << source << " to " << v << ". ";
	std::cout << "FORMAT NOTE: Index(Distance)" << std::endl;
	std::cout << v << "(" << dis[v] << ")" << "<-";
	while (pre[v] != source) {
		v = pre[v];
		std::cout << v << "(" << dis[v] << ")" << "<-";
	}
	std::cout << source << "(0)" << std::endl;
};// shortest path distance from source to vs