//Samuel Jungert
//A01241879
#include "BFSGraph.h"
#include "DFSGraph.h"
#include <fstream>
#include <string>

void GetInputfromFile(int *(&arrayPtr), int &size, const std::string & fileName);


int main() {
	int * M1;
	int * M2;
	int size1 = 0;
	int size2 = 0;
	const std::string BFS = "Assign8BFSInput.txt";
	const std::string DFS = "Assign8DFSInput.txt";

	GetInputfromFile(M1, size1, BFS);
	GetInputfromFile(M2, size2, DFS);

	std::cout << "-------------Breadth First Search Output------------" << std::endl;
	BFSGraph Graph1(size1);
	Graph1.SetAdjLists(M1);
	Graph1.PrintAdjLists();
	Graph1.BFS();
	for (int i = 1; i < size1; i++) {
		Graph1.PrintSP(0, i);
	}
	std::cout << std::endl;

	std::cout << "-------------Depth First Search Output------------" << std::endl;
	DFSGraph Graph2(size2);
	Graph2.setAdjLists(M2);
	Graph2.printAdjLists();
	Graph2.DFS();
	Graph2.PrintReachableNodes(0);
	
	/*
	for (int i = 0; i < size1; i++) {
		std::cout << "Row " << i << ":" << std::endl;
		for (int j = 0; j < size1; j++) {
			std::cout << M1[i*size1 + j] << std::endl;
			}
		}

	for (int i = 0; i < size2; i++) {
		std::cout << "Row " << i << ":" << std::endl;
		for (int j = 0; j < size2; j++) {
			std::cout << M2[i*size1 + j] << std::endl;
		}
	}
	*/

	system("pause");
	return 0;
}

void GetInputfromFile(int *(&arrayPtr), int &size, const std::string & fileName) {
	std::ifstream inputFile;
	int recievedInteger = 0;
	inputFile.open(fileName);

	if (inputFile.fail()) {
		std::cout << "The file was not found." << std::endl;
	}

	inputFile >> size;
	arrayPtr = new int[size*size];

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			inputFile >> arrayPtr[i*size + j];
		}
	}
	inputFile.close();
}
