// Graph.h ... interface to Graph ADT
// Written by John Shepherd, March 2013

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

// graph representation is hidden
typedef struct GraphRep *Graph;

// vertices denoted by integers 0..N-1
typedef int Vertex;
int validV (Graph, Vertex); // validity check

// edges are pairs of vertices (end-points)
typedef struct {
	Vertex v;
	Vertex w;
} Edge;
void insertEdge (Graph, Vertex, Vertex, int);
void removeEdge (Graph, Vertex, Vertex);

// operations on graphs
Graph newGraph (int nV);
void dropGraph (Graph);
Graph makeGraph (int, int **);
void showGraph (Graph, char **);
int findPath (Graph, Vertex, Vertex, int, int *);
int numberOfOutlinks(Graph g,Vertex v);
int numberOfInlinks(Graph g,Vertex w);
void setInLinksOutlinks(Graph g);
double W_out (Graph g, Vertex from, Vertex to);
void setW_Out (Graph g);
double W_in (Graph g, Vertex from, Vertex to);
void setW_in (Graph g);
void drop(Graph g);
void insertURLNames (Graph g, char **URLs);

void initialisePageRank (Graph g, double initialiseValue);
void calculatePageRank (Graph g, double d, double diffPR, int maxIterations);

void makeFileAndList (Graph g, char *fileName, double *pageRank);

void printMatrix (Graph g);
void printWOut (Graph g);
#endif
