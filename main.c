#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SQ(x) ((x)*(x))
#define ABS(x) ((x)>0) ? (x) : -1 * (x)

#define K_ARGUMENT 2
#define MAX_ITER_ARGUMENT 3
#define DEFAULT_NUM_OF_ITERS 200
#define MAX_CHAR_AT_ONE_LINE 1000
#define COMMA_CHAR ','
#define IRRELEVANT_DIFF 0.00005

static int k, numOfVectors, dimension;

typedef struct {
    double *prevCentroid;
    double *currCentroid;
    int counter;
} Cluster;

void calcDimAndNumOfVectors() {
    char line[MAX_CHAR_AT_ONE_LINE];
    char *c;
    dimension = 1;
    numOfVectors = 1;

    scanf("%s", line);
    for (c = line; *c != '\0'; c++) {
        dimension += *c == COMMA_CHAR ? 1 : 0;
    }

    while (scanf("%s", line) != EOF) {
        (numOfVectors)++;
    }
    rewind(stdin); /* Move back to the beginning of the input file */
}

double **initVectorsArray() {
    int i, j;
    char ch;
    double *matrix, **vectorsArray;
    /* Allocate memory for vectorsArray */
    assert(matrix = (double *) malloc(numOfVectors * (dimension + 1) * sizeof(double)));
    assert(vectorsArray = malloc(numOfVectors * sizeof(double *)));

    for (i = 0; i < numOfVectors; ++i) {
        vectorsArray[i] = matrix + i * (dimension + 1);
        for (j = 0; j < dimension; ++j) {
            scanf("%lf%c", &vectorsArray[i][j], &ch);
        }
    }
    return vectorsArray;
}

Cluster *initClusters(double **vectorsArray) {
    int i, j;
    Cluster *clustersArray;
    /* Allocate memory for clustersArray */
    assert(clustersArray = (Cluster *) malloc(k * sizeof(Cluster)));

    for (i = 0; i < k; ++i) {
        assert(clustersArray[i].prevCentroid = (double *) malloc(dimension * sizeof(double)));
        assert(clustersArray[i].currCentroid = (double *) malloc(dimension * sizeof(double)));
        clustersArray[i].counter = 0;

        for (j = 0; j < dimension; ++j) {
            clustersArray[i].currCentroid[j] = vectorsArray[i][j];
        }
    }
    return clustersArray;
}

double vectorsNorm(const double *vec1, const double *vec2) {
    double norm = 0;
    int i;
    for (i = 0; i < dimension; ++i) {
        norm += SQ(vec1[i] - vec2[i]);
    }
    return norm;
}

int findMyCluster(double *vec, Cluster *clustersArray) {
    int myCluster, j;
    double minNorm, norm;

    myCluster = 0;
    minNorm = vectorsNorm(vec, clustersArray[0].prevCentroid);
    for (j = 1; j < k; ++j) {
        norm = vectorsNorm(vec, clustersArray[j].prevCentroid);
        if (norm < minNorm) {
            myCluster = j;
            minNorm = norm;
        }
    }
    return myCluster;
}

void assginVectorsToClusters(double **vectorsArray, Cluster *clustersArray) {
    int j, i, myCluster;
    double *vec;
    for (j = 0; j < numOfVectors; ++j) {
        vec = vectorsArray[j];
        myCluster = findMyCluster(vectorsArray[j], clustersArray);
        vec[dimension] = myCluster;
        for (i = 0; i < dimension; ++i) {
            clustersArray[myCluster].currCentroid[i] += vec[i];
        }
        clustersArray[myCluster].counter++;
    }
}

int recalcCentroids(Cluster *clustersArray) {
    int i, j, changes = 0;
    Cluster cluster;
    for (i = 0; i < k; ++i) {
        cluster = clustersArray[i];
        for (j = 0; j < dimension; ++j) {
            cluster.currCentroid[j] /= cluster.counter;
            changes += (ABS(cluster.prevCentroid[j] - cluster.currCentroid[j]) > IRRELEVANT_DIFF) ? 1 : 0;
        }
    }
    return changes;
}

void initCurrCentroidAndCounter(Cluster *clustersArray) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            clustersArray[i].prevCentroid[j] = clustersArray[i].currCentroid[j];
            clustersArray[i].currCentroid[j] = 0;
        }
        clustersArray[i].counter = 0;
    }
}

void printFinalCentroids(Cluster *clustersArray) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            if (j > 0)
                printf("%c", COMMA_CHAR);
            printf("%0.4f", clustersArray[i].currCentroid[j]);
        }
        printf("\n");
    }
    printf("\n");
}

void freeMemoryVectorsClusters(double **vectorsArray, Cluster *clustersArray) {
    int i;
    /* Free clusters */
    for (i = 0; i < k; ++i) {
        free(clustersArray[i].currCentroid);
        free(clustersArray[i].prevCentroid);
    }
    free(clustersArray);

    /* Free vectors */
    free(&vectorsArray[0][0]);
    free(vectorsArray);
}

int main(int argc, char *argv[]) {
    int maxIter, i, changes;
    double **vectorsArray;
    Cluster *clustersArray;

    if (argc < K_ARGUMENT) {
        printf("The program needs at least one argument");
        return 0;
    }
    assert ((k = atoi(argv[K_ARGUMENT - 1])) > 0);
    if (argc >= MAX_ITER_ARGUMENT) {
        assert ((maxIter = atoi(argv[MAX_ITER_ARGUMENT - 1])) > 0);
    } else {
        maxIter = DEFAULT_NUM_OF_ITERS;
    }

    calcDimAndNumOfVectors();
    assert(k <= numOfVectors); /* Number of clusters can't be more than the number of vectors */

    vectorsArray = initVectorsArray();
    clustersArray = initClusters(vectorsArray);

    for (i = 0; i < maxIter; ++i) {
        initCurrCentroidAndCounter(clustersArray);
        assginVectorsToClusters(vectorsArray, clustersArray);
        changes = recalcCentroids(clustersArray);
        if (changes == 0) { /* Centroids stay unchanged in the current iteration */
            break;
        }
    }

    printFinalCentroids(clustersArray);
    freeMemoryVectorsClusters(vectorsArray, clustersArray);
    return 0;
}
