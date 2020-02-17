// Graph.c ... implementation of Graph ADT
// Written by John Shepherd, May 2013

/*
	Some functions were taken from the weekly labs;
	however modfied.
	Added some variables in the graph representation to 
	make the tasks more managable

*/



#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "graph.h"
#include "queue.h"

// graph representation (adjacency matrix)
typedef struct GraphRep {
	int nV;		 // #vertices
	int nE;		 // #edges
	int **edges; // matrix of weights (0 == no edge)
	char **vertexNames;  // array of the URLs in the collection file, which are the nodes; 
						 // to give integer value to urls (string)
	int *inLinks; 		 // number of edges pointing TO the vertex
	double *outLinks;	 // number of edges that are pointed OUT of the vertex
	double **w_out; 	 // weight of link [i][j] of outLinks according to the formula
						 // if the outlink is 0, then initialise to 0.5 for easier PR computation
	double **w_in; 		 // same as w_out but for inLinks
	double *pageRank;    // list of calculated pagerank according to the formula
} GraphRep;

// create an empty graph
Graph newGraph (int nV)
{
	assert (nV > 0);

	GraphRep *new = malloc (sizeof *new);
	assert (new != NULL);
	*new = (GraphRep){ .nV = nV, .nE = 0 };
	
	new->edges = calloc ((size_t) nV, sizeof (int *));
	assert (new->edges != NULL);
	for (int v = 0; v < nV; v++) {
		new->edges[v] = calloc ((size_t) nV, sizeof (int));
		assert (new->edges[v] != 0);
	}
	new->inLinks = calloc((size_t) nV, sizeof(int));
	new->outLinks = calloc((size_t) nV, sizeof(double));	

	new->w_out = calloc((size_t) nV, sizeof(double*));
	for (int v = 0; v < nV; v++) {
		new->w_out[v] = calloc ((size_t) nV, sizeof (double));
		assert (new->w_out[v] != 0);
	}

	new->w_in = calloc((size_t) nV, sizeof(double*));
	for (int v = 0; v < nV; v++) {
		new->w_in[v] = calloc ((size_t) nV, sizeof (double));
		assert (new->w_in[v] != 0);
	}

	new->pageRank = calloc((size_t) nV, sizeof(double));	
	//new->vertexNames = calloc((size_t) nV, sizeof(char*));
	return new;
}

void drop(Graph g) {

	free(g->inLinks);
	free(g->outLinks);
	
	for (int v = 0; v < g->nV; v++) free(g->edges[v]);
	free(g->edges);
	for (int v = 0; v < g->nV; v++) free(g->vertexNames[v]);
	free(g->vertexNames);
	for (int v = 0; v < g->nV; v++) free(g->w_out[v]);
	free(g->w_out);
	for (int v = 0; v < g->nV; v++) free(g->w_in[v]);
	free(g->w_in);
	//for (int v = 0; v < g->nV; v++) free(g->pageRank[v]);
	free(g->pageRank);
	free(g);
}

// initialise the array of strings (URL and its interger equivalent)
// into the graph representation
void insertURLNames (Graph g, char **URLs) {
	g->vertexNames = URLs;
	return;
}

// check if two vertices are connected
bool isConnected (Graph g, Vertex src, Vertex dest) {
	// if connected
	if (g->edges[src][dest] != 0) return true;
	// error check (no negative vertices)
	// or the vertex index bigger than total number of vertices
	if (src < 0 || dest < 0 || src > g->nV || dest > g->nV) return false;
	else return false;
}

// number of vertices
int nVertices (Graph g) {
	return g->nV;
}

// check validity of Vertex
int validV (Graph g, Vertex v)
{
	return (g != NULL && v >= 0 && v < g->nV);
}


// insert an Edge
// modified from lab to be DIRECTED edges
// [v][w] != [w][v]
void insertEdge (Graph g, Vertex v, Vertex w, int wt) {
	assert (g != NULL && validV (g, v) && validV (g, w));
	if (g->edges[v][w] != 0)
		// an edge already exists; do nothing.
		return;

	g->edges[v][w] = wt;
	g->nE++;
}

// remove an Edge
// modified from lab to remove DIRECTED edges
// unlinks[v][w] 
void removeEdge (Graph g, Vertex v, Vertex w) {
	assert (g != NULL && validV (g, v) && validV (g, w));
	if (g->edges[v][w] == 0)
		// an edge doesn't exist; do nothing.
		return;

	g->edges[v][w] = 0;
	g->nE--;
}

// Get the number of edges that point
// towards the vertex w
int numberOfInlinks(Graph g, Vertex w) {
	int Inlinks = 0;
	int N = g->nV;
	for (int i = 0; i < N; i ++) 
		if (g->edges[i][w] != 0) Inlinks ++;
	return Inlinks;

}

// Get the number of edges that point to the 
// adjacent nodes of the vertex
int numberOfOutlinks(Graph g, Vertex v) {
	int outlinks = 0;
	int N = g->nV;
	for (int i = 0; i < N; i ++) 
		if (g->edges[v][i] != 0) outlinks ++;
	return outlinks;
}

// set the inlinks and outlinks array
// for outlinks, if the value is zero,
// set it into 0.5 for easier PR computation
// no need to do such for inLinks
void setInLinksOutlinks(Graph g) {
	for(int i = 0; i < g->nV; i ++) g->inLinks[i] = numberOfInlinks(g, i);	
	for(int i = 0; i < g->nV; i ++) g->outLinks[i] = numberOfOutlinks(g, i);
	for(int i = 0; i < g->nV; i ++) if (g->outLinks[i] == 0) g->outLinks[i] = 0.5;

}

// compute for the W_out according to the formula
double W_out (Graph g, Vertex from, Vertex to) {
	// edge does not exist
	if (g->edges[from][to] == 0) {
		return -1;
	}
    double numerator = g->outLinks[to];
    double denominator = 0;
    for (int i = 0; i < g->nV; i ++) {
		if (g->edges[from][i] != 0) 
            denominator = denominator + g->outLinks[i];
    }
	return numerator/denominator; 
}

// compute for the W_in according to the formula
double W_in (Graph g, Vertex from, Vertex to) {
	// edge does not exist
	if (g->edges[from][to] == 0) {
		return -1;
	}
    double numerator = g->inLinks[to];
    double denominator = 0;
    for (int i = 0; i < g->nV; i ++) {
		if (g->edges[from][i] != 0) 
			denominator = denominator + g->inLinks[i];
	}
    return numerator/denominator; 
}

// set the values of W_out into its respective section in the graph rep
void setW_Out (Graph g) {
//	printf("W_OUT:\n");
	for (int i = 0; i < g->nV; i ++) {
		for (int j = 0; j < g->nV; j ++) {
			if (g->edges[i][j] == 0) {
				g->w_out[i][j] = 0;
			} else {
				g->w_out[i][j] = W_out(g, i, j);
//				printf("[%d][%d] --- %.7f\n", i, j, g->w_out[i][j]);
			}	
		}
	}
}

// set the values of W_in into its respective section in the graph rep
void setW_in (Graph g) {
//	printf("W_IN:\n");
	for (int i = 0; i < g->nV; i ++) {
		for (int j = 0; j < g->nV; j ++) {
			if (g->edges[i][j] == 0) {
				g->w_in[i][j] = 0;
			} else {
				g->w_in[i][j] = W_in(g, i, j);
//				printf("[%d][%d] --- %.7f\n", i, j, g->w_in[i][j]);
			}	
		}
	}
}


// Calculate the PageRank according to the formula
void calculatePageRank (Graph g, double d, double diffPR, int maxIterations) {
    
	// check if correct args
	assert (maxIterations > 0 && g != NULL && d > 0); 
	
	int N = nVertices(g);
    // the pagerank when iterations = 0;
	double PR_Beg = 1 / N;
	// initialise every node's pagerank to be 1/N
    initialisePageRank(g, PR_Beg);

    int iteration = 0;
    double diff = diffPR;
	// create space for array of doubles
	double *tmp = malloc((size_t) N * sizeof (double));
	// code below is just the formula
    while (iteration < maxIterations && diff >= diffPR) {
        for (int i = 0; i < N; i ++) {
           
            double PR = 0;

            for (int j = 0; j < N; j ++) {
				// if edges are connected
            	if (isConnected(g, j, i)) {
            		PR = PR + g->pageRank[j] * g->w_out[j][i] * g->w_in[j][i];
	        	}
    		}
    		PR = d * PR;
        	PR = ((1 - d)/N) + PR;
        	diff = diff + fabs(PR - g->pageRank[i]);
			tmp[i] = PR;
		
		}
		// copy the value in tmp to pagerank
		for (int i = 0; i < N; i ++) {
			g->pageRank[i] = tmp[i];
		}
        iteration ++;
    }
	// remove tmp
	free(tmp);
	

	// create final output
	char *fileName = "pagerankList.txt";
	makeFileAndList (g, fileName, g->pageRank);
}




// output of the task
// prints the url name, number of outlinks, and pagerank
// in descending order according to pagerank
void makeFileAndList (Graph g, char *fileName, double *pageRank) {
	
	// make a temporary array of pageRank
	// in order to order-print them
	double *tmp = calloc((size_t)g->nV, sizeof(double));
	for (int i = 0; i < g->nV; i++) {
		tmp[i] = pageRank[i];
	}
	
	// make a text file
    // and print as required 
    FILE *output = fopen(fileName, "w");
    if (output) {
		int counter = 0;
		while (counter < g -> nV) {
			double max = 0;
			int outLinks = 0;
			// set tmp[i] array to hold the highest unused pagerank value
			// and set the outlinks to 0 if its 0.5
			// as it was initialised to 0.5 if it was 0 for convenience
			for (int i = 0; i < g-> nV; i++) {
				if (tmp[i] > max) {
					max = tmp[i];
					if (g->outLinks[i] == 0.5) outLinks = 0;
					else outLinks = g->outLinks[i];	
				}
			}
			// set the name of vertex with highest pagerank
			char *name;
			for (int i = 0; i < g-> nV; i++) {
				if (tmp[i] == max) {
					name = malloc(sizeof(char) * strlen(g->vertexNames[i]) + 1);
					name = strcpy(name, g->vertexNames[i]);
				}
			}
			// set tmp[i] to zero since it has been used already
			for (int i = 0; i < g-> nV; i++) if (tmp[i] == max) tmp[i] = 0;	

			// print the acquired values to the opened file above
			fprintf(output, "%s, %d, %.7f\n", name, outLinks, max);
			free(name);
			// **note that if the pagerank values are the same, it will print according to 
			// the list in the collection file
			counter ++;
		}		

		fclose(output);
    }

	// delete tmp from memory
	free(tmp);
    return;
}

// initialse all the values in the pagerank array into the starting value 1/N
void initialisePageRank (Graph g, double initialiseValue) {
	for (int i = 0; i < g->nV; i ++) g->pageRank[i] = initialiseValue;
}





/////////////////////////////////////////////////////////////////////////////////////


//				FUNCTIONS TO HELP TEST IF THE OUTPUTS ARE CORRECT


/////////////////////////////////////////////////////////////////////////////////////
// display graph, using names for vertices
void showGraph (Graph g, char **names)
{
	assert (g != NULL);
	printf ("#vertices=%d, #edges=%d\n\n", g->nV, g->nE);
	int v, w;
	for (v = 0; v < g->nV; v++) {
		printf ("%d %s\n", v, names[v]);
		for (w = 0; w < g->nV; w++) {
			if (g->edges[v][w]) {
				printf ("\t%s (%d)\n", names[w], g->edges[v][w]);
			}
		}
		printf ("\n");
	}
}

// prints the adjacent matrix of the graph
void printMatrix (Graph g) {
	for (int i = 0; i < g->nV; i ++) {
		for (int j = 0; j < g->nV; j ++) {
			printf("%d\t", g->edges[i][j]);
		}
		printf("\n");
	}
}
// print the values of W_out
void printWOut (Graph g) {
	for (int i = 0; i < g->nV; i ++) {
		for (int j = 0; j < g->nV; j ++) {
			if (g->edges[i][j] == 0) {
				continue;
			} else {
				printf("[%d][%d] -- %lf\n", i, j, W_out(g, i, j));
			}	
		}
	}
}








// Some function in the lab
// I dont think its necessary but commenting it in case
/*
// make an edge
static Edge mkEdge (Graph g, Vertex v, Vertex w)
{
	assert (g != NULL && validV (g, v) && validV (g, w));
	Edge new = {v, w}; // struct assignment
	return new;
}
*/