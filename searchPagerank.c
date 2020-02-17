//COMP 2521 ASS2
// searchPagerank.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>


#include "searchPagerank.h"


int main (int argc, char *argv[]) {
    // error check
    if (argc == 1) exit(1);
    
    int numWords = argc - 1;
    // copy the arguments intp a temporary array
    char **tmp = malloc(sizeof(char *) * numWords);
    for (int i = 1; i < argc; i++) {
        // same as strdup
        tmp[i-1] = malloc(sizeof(char) * strlen(argv[i]) + 1);
        tmp[i-1] = strcpy(tmp[i-1], argv[i]);
    }

    // search the urls and print out the urls according to the given criteria
    searchPagerank(tmp, numWords);
    // free memory
    for (int i = 0; i < numWords; i++) free(tmp[i]);
    free(tmp);
    
    return 0;
}

// search the urls with the given words in the cmdline
// from invertedIndex.txt file then print out
// urls in appropriate order
void searchPagerank (char **words, int numWords) {
    
    char *indexFile = "invertedIndex.txt";
    char *PageRankFile = "pagerankList.txt";
    
    // get the total number of URLs
    int nURLs = numberOfURLs(PageRankFile);
    
    // array of URLs containing the word in the invertedIndex.txt file
    // not checking for repetition
    char **list = findMatchedURLs(indexFile, words, nURLs, numWords);
    
    // convert the url names in the list array to its pagerank
    // not checking for repetitions 
    double *pRank = findPageRank(PageRankFile, list, nURLs);
    
    // set tree to NULL; then insert the pRank list & list to the array
    // and initiaise each fields of the nodes accordingly
    // should have nURLs number of nodes as repetition is CHECKED in the function
    Tree t = NULL;
    t = sortList (t, list, pRank);

    // print the URL names accordingly
    printTree(t);
    dropTree(t);
    for (int i = 0; i < numWords * nURLs; i++) free(list[i]);
    free(list);
    free(pRank);
}


// return the number of URLs
int numberOfURLs (char* pageRankListFile) {
    char BUF[BUFSIZE];
    int count = 0;
    FILE *listFile = fopen(pageRankListFile, "r");
    if (listFile) {
        while (fgets(BUF, 100, listFile)) count ++;
        fclose(listFile);
    }
    return count;
}

// find the URLs related to the passed words from the cmdline argument and return the 
// array of URLs
char **findMatchedURLs (char *indexFile, char **words, int nURLs, int numWords) {
    // set BUF
    char BUF[BUFSIZE];
    char inFileBUF[BUFSIZE];

    // initialise the array into the max possible size;
    // number of URLs * number of words; all URLs are different
    int arraySize = numWords * nURLs;
    char **list = calloc((size_t)arraySize, sizeof(char *));

    int i = 0;
    // while any word in the numWords exist
    while (i < numWords) {
        FILE *file = fopen(indexFile, "r");
        if (file) {
            while (fscanf(file,"%s", BUF) != EOF) {
                // if the word read is the same with the cmdline argument word
                if (strcmp(BUF, words[i]) == 0) {
                    // get the line of the file with the URL
                    fgets(inFileBUF, BUFSIZ, file);
                    // tokenise the word
                    char *tmp = strtok(inFileBUF, " \n ");
                    // while the string is not null
                    while (tmp != NULL) {
                        // insert the word to list
                        list = insertWord(tmp, list);
                        // move to next word
                        tmp = strtok(NULL, " \n ");
                    
                    }
                    // stop scanning and move to next word
                    break;              
                }
            }
            fclose(file);
        }
        i ++;
    }
    return list;
}

// insert the truncated string to the list
char ** insertWord (char *truncdStr, char **list) {
    int i = 0;
    // look for space in array
    while (list[i] != NULL) {
        i ++;
    }
    // same as strdup
    list[i] = malloc(sizeof(char) * strlen(truncdStr) + 1);
    list[i] = strcpy(list[i], truncdStr);
    return list;
}





// convert the url names in the list array to its pagerank and return the converted list
// do not check repetitions yet
double *findPageRank(char *PageRankFile, char **list, int nURLs) {
    char BUF[BUFSIZE];
    char inFileBUF[BUFSIZE];

    // get the number of words from the input
    int i;
    for(i = 0; list[i] != NULL; i++) {}
    int numWords = i;
    
    // initialise array of double to max possible elements
    // which is -- all elements in list are different
    int arraySize = numWords * nURLs;
    double *PR_v= calloc((size_t)arraySize, sizeof(double) );
    
    // while the passed list is not null
    for (int i = 0; list[i] != NULL; i++){
        FILE *file = fopen(PageRankFile, "r");
        if (file) {

            while (fscanf(file, "%s", BUF) != EOF) {
                // not necessary but double check
                normaliseWord(BUF);
                // if the string to get double is found
                if (strcmp(BUF, list[i]) == 0) {
                    // get the line
                    fgets(inFileBUF, 100, file);
                    // normalise it -- incase there are weird chars
                    normaliseWord(inFileBUF);
                    // get the second float from the file -- which is pagerank
                    // not the number of outlinks
                    char *tmpPtr;
                    strtof(inFileBUF, &tmpPtr);
                    double f2 = strtof(tmpPtr, NULL);
                    // set the array into the double
                    PR_v[i] = f2;
                }
            }
        }
        fclose(file);
    }
    return PR_v;
}

// check if the node with the URL name is in the string
bool existTree (Tree t, char *name) {
    if (t == NULL) return false;
    if (strcmp(t->name, name) == 0) return true;
    bool res1 = existTree(t->right, name);
    bool res2 = existTree(t->left, name);
    return res1 || res2;
}

// insert the values in a tree according to repetitions;
// same repetitions -- according to pagerank
Tree sortList (Tree t, char **list, double *toPrint) {
    for (int i = 0; list[i] != 0; i++) {
        // rep count start at 0 since we'll be comparing same string once
        // ex. str[i] and str[j] where i and j are the same number
        double reps = 0;
        if (existTree(t, list[i]) == false) {
            // the node is not in the tree; means we can 
            // execute this function without worrying about duplicates
            for (int j = 0; list[j] != NULL; j++) {
                // get total number of repetitions
                if (strcmp(list[i], list[j]) == 0) { 
                    reps ++;
                }
            }
            // insert the word and its values to the tree
            t = insertTree(t, list[i], toPrint[i], reps);
        }
    }
    return t;
}

// recursive call to print the lemenets in a tree in descending order
int printTree(Tree t) {
    if (t == NULL) { 
        return 0;
    }
    int c = 1;
    
    if (t->left && t->right) c ++;
    c = c + printTree(t->right);
    // if the count is greater than 30 dont print anything
    if (c <= 30) printf("%s\n", t->name);
    c = c + printTree(t->left);
    return c;
}



// recursively insert to a tree in a sorted order
Tree insertTree (Tree t, char *URL, double PR, double reps) {
    // current node is NULL
    if (t == NULL) {
        t = malloc(sizeof(struct TreeRep));
        t->rep = 1;
        t->name = malloc(sizeof(char) * strlen(URL) + 1);
        t->name = strcpy(t->name, URL);
        t->rep = reps;
        t->value = PR;
        t->left = t->right = NULL;
        return t;
    }
    // sort according to repetitions
    double diff = reps - t->rep;
    // if same number of repetitions
    if (diff == 0 ) {
        // sort by pagerank
        diff = PR - t->value;
    } else {
        if (diff < 0) t->left = insertTree(t->left, URL, PR, reps);
        else if (diff > 0) t->right = insertTree(t->right, URL, PR, reps);
       
        return t;
    }

    
    // this bit wont occur but in case
    if (diff == 0) return t;
    else if (diff < 0) t->left = insertTree(t->left, URL, PR, reps);
    else if (diff > 0) t->right = insertTree(t->right, URL, PR, reps);
    

    return t;
}

void dropTree (Tree t) {
    if (t == NULL) return;
    dropTree(t->left);
    free(t->name);
    free(t);
    dropTree(t->right);
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
        str[c] = '\0';
        c--;
    }
     
    // total length of the string
    int len = strlen(str) - 1;
    // if the string ends in special characters
    if (str[len] == '.' || str[len] == ',' ||
        str[len] == ';' || str[len] == '?') {
        str[len] = '\0';
    }



    for (int j = 0; str[j] != '\0'; j++) {
        if (str[j] == ',') {
            str[j] = ' ';
        }
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









































































































// I AM A FUCKING FAILURE
/*
char **getURLName(char *PageRankFile) {
    char BUF[100];
    FILE *file = fopen(PageRankFile, "r");
    int urlCount = 0;
    if (file) {
        while (fgets(BUF, 100, file)) {
        urlCount ++;
        }
        fclose(file);
    }

    char **urlNames = malloc(sizeof(char*) * urlCount);
    int i = 0;

    file = fopen(PageRankFile, "r");
    if (file) {
        while (fgets(BUF, 100, file)) {
            char *tmp = strtok(BUF, ",");
            int j;
            for (j = 0; tmp[j] != '\n'; j++) {
    
                printf("%d\n", j);
                urlNames[i][j] = tmp[j];
    
            }
            urlNames[i][j] = '\0';
            i++;
        }
        fclose(file);
    }
    return urlNames;
}


double *pageRankList (char *PageRankFile) {
    char BUF[100];
    FILE *file = fopen(PageRankFile, "r");
    int urlCount = 0;
    if (file) {
        while (fgets(BUF, 100, file)) {
        urlCount ++;
        }
        fclose(file);
    }

    double *pageRank = malloc(sizeof(double) * urlCount);
    int i = 0;


    file = fopen(PageRankFile, "r");
    if (file) {
        while (fgets(BUF, 100, file)) {
            char *tmp = strtok(BUF, " ");
            
            for (int j = 0; tmp[j] != ' ', j++) {
                continue;
            }


        }
        fclose(file);
    }
    return pageRank;

}
*/
