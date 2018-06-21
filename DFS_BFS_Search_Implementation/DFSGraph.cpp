//Samuel Jungert
//A01241879
#include "DFSGraph.h"

DFSGraph::DFSGraph(int n_input) { // constructor
	 //Private Variables
	color = new colorType[n_input];
	pre = new int[n_input];
	//Public Variables
	adj = new Vertex *[n_input];
	n = n_input;
	time = 0;

	for (int i = 0; i < n_input; i++) {
		color[i] = WHITE;
		pre[i] = n_input;
		adj[i] = NULL;
	}
};

DFSGraph::~DFSGraph(){
	delete[] color;
	color = 0;
	delete[] pre;
	pre = 0;
	delete[] adj;
	adj = 0; 
};

void DFSGraph::setAdjLists(int * adjM) {
	for (int i = 0; i < n; i++) {
		for (int j = n - 1; j >= 0; j--) {
			if (adjM[i*n + j] == 1) {
				//create a new node and add it to the front of the adjList[i]
				Vertex *v = new Vertex(j);
				v->next = adj[i];
				adj[i] = v;
				delete v;
				v = 0;
			}
		}
	}
}; // build the adjacency lists from the adjacency matrix adjM

void DFSGraph::printAdjLists() { // print the adjacency lists of the graph
	for (int i = 0; i < n; i++) {
		std::cout << i << ": ";
		Vertex * currentNode = adj[i];

		if (currentNode == NULL) {
			std::cout << "NULL";
		}
		else {
			while (currentNode != NULL) {
				std::cout << currentNode->id << " ";
				currentNode = currentNode->next;
			}
		}
		std::cout << std::endl;
	}
};

void DFSGraph::DFS(int id){ // DFS traversal, id is the source vertex, with default 0
	time = 0; //global variable
	std::cout << "Depth First Traversal Vertex "  << id << ": ";
	DFSVisit(id);
	for (int i = 0; i < n; i++) {
		if (color[i] == WHITE) {
			std::cout << std::endl << "Depth First Traversal Vertex " << i<< ": ";
			DFSVisit(i);
		}
	}
	std::cout << std::endl;
};

void DFSGraph::DFSVisit(int id) {
	color[id] = GRAY;
	//time = time + 1;
	Vertex *v = adj[id];
	std::cout << id << " ";
	while (v != NULL) {
		if (color[v->id] == WHITE) {
			pre[v->id] = id;
			DFSVisit(v->id);
		}
		v = v->next;
	}
	color[id] = BLACK;
}; // actually does DFS, search a connected component from id

void DFSGraph::PrintReachableNodes(int source) {
	std::cout << "Reachable Nodes from " << source << ": ";
	std::cout << source << " ";
	colorType * tempColor = new colorType[n];
	for (int i = 0; i < n; i++) {
		tempColor[i] = WHITE;
	}
	RPrintReachableNodes(adj[source], tempColor);
	std::cout << std::endl;
	delete[] tempColor;
	tempColor = 0;
};// Print all nodes that can be reached by source

void DFSGraph::RPrintReachableNodes(Vertex* v, colorType *& tempColor) {
	if (v == NULL || tempColor[v->id] == BLACK) {
		return;
	}
	else {
			std::cout << v->id << " ";
			tempColor[v->id] = BLACK;
			RPrintReachableNodes(adj[v->id], tempColor);
			RPrintReachableNodes(v->next, tempColor);
	}
};

