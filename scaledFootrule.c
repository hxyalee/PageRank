//COMP 2521 ASS2
// pagerank.c
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



char **uFile (char **files, int nFiles);
char **normaliseArray (char **array, int nItems);





int main (int argc, char *argv[]) {
	int nFiles = argc - 1;
	char **files = malloc(sizeof(char *) * nFiles);
    
	for (int i = 0; i < nFiles; i ++) {
		int  j = i + 1;
		files[i] = malloc(sizeof(char) * strlen(argv[j]) + 1);
		files[i] = strcpy(files[i], argv[j]);
	}
	char **unionFile = uFile(files, nFiles);
	
    for (int i = 0; unionFile[i] != NULL; i++) printf("%s\n", unionFile[i]);

}



char **uFile (char **files, int nFiles) {
	char BUF[1000];
	int nItems = 0;
	for (int i = 0; i < nFiles; i++) {
		FILE *fp = fopen(files[i], "r" );
		if (fp) {
			while (fscanf(fp, "%s", BUF) != EOF) {
				nItems ++;
			}

			fclose(fp);
		}
	}

	char **mergedArray = malloc(sizeof(char *) * nItems + 1);
	for (int i = 0; i < nFiles; i++) {
		int j = 0;
		FILE *fp = fopen(files[i], "r");
		if (fp) {
			while (fscanf(fp, "%s", BUF) != EOF) {
				mergedArray[j] = malloc(sizeof(char) * strlen(BUF) + 1);
				mergedArray[j] = strcpy(mergedArray[i], BUF);
				j ++;
			}

			fclose(fp);
		}
	}


    mergedArray = normaliseArray(mergedArray, nItems);
	return mergedArray;

}



char **normaliseArray (char **array, int nItems) {
	int i, j, k;
	int n = nItems;
	for (i = 0; i < n; i++) {

        for (j = i+1; j < n; j++) {
        
            if(strcmp(array[j], array[i]) == 0) {

                for(k = j; k < n; k++) {
                    array[k] = malloc(sizeof(char) * strlen(array[k+1] + 1));

                    array[k] = strcpy(array[k], array[k+1]);
                }
                n--;
                j --;  
            } 

        }
    }
    printf("xxxxxxxxxxxxxxxxxxxxxx\n"); 
    for (i = 0; array[i] != NULL; i++) printf("%s\n", array[i]);
    
    return array;
}

