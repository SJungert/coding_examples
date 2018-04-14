//Samuel Jungert
//A01241879
#ifndef DFSGraph_H
#define DFSGraph_H
#include "BFSGraph.h"

class DFSGraph
{
private:
	colorType * color; // record the colors of the vertices during DFS
	int * pre; // record the predecessors during DFS
public:
	int n; // the number of vertices, the ids of the vertices are from 0 to n-1
	Vertex ** adj; // adj[i] points the head of the adjacency list of vertex i
	DFSGraph(int n_input); // constructor
	void setAdjLists(int * adjM); // build the adjacency lists from the adjacency matrix adjM
	void printAdjLists(); // print the adjacency lists of the graph
						  // the following two functions are for the DFS traversal as we discussed in class
	void DFS(int id = 0); // DFS traversal, id is the source vertex, with default 0
	void DFSVisit(int id); // actually does DFS, search a connected component from id
	void PrintReachableNodes(int source); // Print all nodes that can be reached by source
	void RPrintReachableNodes(Vertex *, colorType *&);

	//global variable time
	int time;
};

#endif // !DFSGraph_H