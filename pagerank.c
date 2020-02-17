//COMP 2521 ASS2
// pagerank.c by HOYA LEE 
// written august 2019
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "graph.h"
#include "queue.h"
#include "set.h"
#include "pagerank.h"




int main(int argc, char *argv[]) {
    // error check
    if (argc != 4) {
        exit(1);
    }
    // set the cmd line args to variable names
    double d = atof(argv[1]);
    double diffPR = atof(argv[2]);
    int maxIterations = atoi(argv[3]);
    // error check -- if the input is an alphabet
    if (isalpha(d) != 0 ||isalpha(diffPR) != 0 || isalpha(maxIterations) != 0) {
        exit(1);
    }
    // compute pagerank then output
    pageRank(d, diffPR, maxIterations);
    
    return 0;

}

void pageRank (double d, double diffPR, int maxIterations) {
    // Open File
    char *rootFileName = "collection.txt";
    // number of total vertices
    int nV = getNumberOfVertices(rootFileName);
    // create an array of the vertices (index = vertex number)
    char **URLs = arrayOfURLs(rootFileName, nV);
    // Initialise an empty graph
    Graph g = newGraph((size_t)nV);
    // insert the array of urls into the graph
    insertURLNames(g, URLs);
    // create graph
    g = generateGraph(g, rootFileName, URLs);
    // calculate the pagerank and print it to the output file accordingly
    calculatePageRank(g, d, diffPR, maxIterations);
    
    //for (int i = 0; i < nV; i++) free(URLs[i]);    
    
    
    drop(g);
    //free(URLs);
}

// Generate a graph with 0 weight according to the 
// edges in the files
Graph generateGraph(Graph g, char* rootFileName, char **URL) {
    
    // Set buffer to perform operations on files
    char inCollection[BUFSIZE];
    char inUrl[BUFSIZE];
    char lineBUF[BUFSIZE];
 
    char *startStr = "#start section-1\n";

    int fromURL;
    int toURL;

    FILE *collectionFile = fopen(rootFileName, "r");
    if (collectionFile) {
        // read the words in the root file per string
 
        while (fscanf(collectionFile, "%s", inCollection) != EOF) {
            // store the copy of the name of the vertex 
            // since theres a need to add .txt to the string read
            char *urlName = malloc(sizeof(char) * strlen(inCollection) + 1);
            urlName = strcpy(urlName, inCollection);
            
            // add ".txt" to the end of the files
            strcat(inCollection, ".txt");    
            // open the url file
            FILE *urlFile = fopen(inCollection, "r");
            if (urlFile) {
            
                // normalise and then strcmp
                // read til #start section-1
                while (normaliseWord(fgets(lineBUF, 1000, urlFile))) {
                    if (strcmp(lineBUF, startStr) == 0) {
                        break;
                    }
                }

                // scan  each words and while havent reached end of section 1
                while (fscanf(urlFile, "%s", inUrl) && (strcmp(inUrl, "#end") != 0)) {
                    // set from url and to url indices and insert the edge between them to the graph
                    int j;
                    for (j = 0; strcmp(URL[j], urlName) != 0; j ++) {}                    
                    fromURL = j;

                    for (j = 0; strcmp(URL[j], inUrl) != 0; j ++) {}
                    toURL = j;

                    if (fromURL != toURL) {
                        insertEdge(g, fromURL, toURL, 1);
                    } 
                }
                fclose(urlFile);
            
            }

        free(urlName);
        }
        fclose(collectionFile);
    }    

    setInLinksOutlinks(g);
    setW_Out (g);
    setW_in (g);

    return g;
}


// Creates an array of the vertices;
// which are the URLs in the root File
// Assuming all URL names are different
char **arrayOfURLs(char *rootFileName, int nV) {

   char inCollection[BUFSIZE];

    // Initialise an array of strings to store the name of URLs
    char **url = malloc(sizeof(char *) * nV);
    int i = 0;
    // fill the url array with vertices
    FILE *collectionFile = fopen(rootFileName, "r");
    if (collectionFile) {
        // while not end of file
        while (fscanf(collectionFile, "%s", inCollection) != EOF) {
            // same as strdup
            url[i] = malloc(sizeof(char) * strlen(inCollection) + 1);
            url[i] = strcpy(url[i], inCollection);
            i ++;
        }
        fclose(collectionFile);
    }

    return url;
}


// Gets the total number of vertices, which is the 
// total number of fileNames to open
// Assuming all URL names are different
int getNumberOfVertices(char *rootFileName) {
   char inCollection[BUFSIZE];

    FILE *collectionFile = fopen(rootFileName, "r");
    // Initalise the number of vertices to 0 
    // This is needed to initialise the graph
    int nV = 0; 
    // Calculate the total number of vertices in a graph
    if (collectionFile) {
        while (fscanf(collectionFile, "%s", inCollection) != EOF) nV ++;

        fclose(collectionFile);
    }
    return nV;
}






// Normalise the words passed to the function follwoing the required properties
// Assime that the string is already 'malloc'-ed and strCpy-ed
char *normaliseWord (char *str) {
    // if empty string
    if (strlen(str) == 0) return str;

    // Remove white spaces in left of the string
    while (isspace(*str)) str ++;
    
    // Remove while spaces on the right of the string
    int c = strlen(str) - 1;
    while (str[c] == ' ') {
        str[c] = '\0' ;
        c--;
    }
     
    // total length of the string
    int len = strlen(str) - 1;
    // if the string ends in special characters
    if (str[len] == '.' || str[len] == ',' ||
        str[len] == ';' || str[len] == '?') {
        str[len] = '\0';
    }
    // change the string to all lowercase
    int i = 0;
    while (str[i] != '\0') {
        str[i] = tolower(str[i]);
        i ++;
    }    
    return str;
}
//////////////////////////////////////////////////////////////////////////////












