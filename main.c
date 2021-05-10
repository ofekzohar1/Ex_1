#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SQ(x) ((x)*(x))

#define K_ARGUMENT 2
#define MAX_ITER_ARGUMENT 3
#define DEFAULT_NUM_OF_ITERS 200
#define MAX_CHAR_AT_ONE_LINE 1000
#define COMMA_CHAR ','
#define END_OF_STRING '\0'

static int k, numOfVectors, dimension;

typedef struct {
    double *prevCentroid;
    double *currCentroid;
    int counter; /* Number of vectors (datapoints) in cluster */
} Cluster;

void calcDimAndNumOfVectors(); /* Calculate vectors dimension and number of total vectors */
double **initVectorsArray(); /* Insert vectors into an array */
Cluster *initClusters(double **vectorsArray); /* Initialize empty clusters array */
double vectorsNorm(const double *vec1, const double *vec2); /* Calculate the norm between 2 vectors */
int findMyCluster(double *vec, Cluster *clustersArray); /* Return the vector's closest cluster (in terms of norm) */
void assignVectorsToClusters(double **vectorsArray, Cluster *clustersArray); /* For any vector assign to his closest cluster */
int recalcCentroids(Cluster *clustersArray); /* Recalculate clusters' centroids, return number of changes */
void initCurrCentroidAndCounter(Cluster *clustersArray); /* Set curr centroid to prev centroid and reset the counter */
void printFinalCentroids(Cluster *clustersArray); /* Print clusters' final centroids */
void freeMemoryVectorsClusters(double **vectorsArray, Cluster *clustersArray); /* Free the allocated memory */
void validateAndAssignInput(int argc, char **argv, int *maxIter); /* Validate and assign k and max_iter input */

int main(int argc, char *argv[]) {
    int maxIter, i, changes;
    double **vectorsArray;
    Cluster *clustersArray;

    validateAndAssignInput(argc, argv, &maxIter);
    calcDimAndNumOfVectors();
    if (k >= numOfVectors) { /* Number of clusters can't be more than the number of vectors */
        printf("Number fo clusters (%d) can't be more than the number of datapoints (%d).", k, numOfVectors);
        return 0;
    }

    /* Initialize vectors (datapoints) and clusters arrays */
    vectorsArray = initVectorsArray();
    clustersArray = initClusters(vectorsArray);

    for (i = 0; i < maxIter; ++i) {
        initCurrCentroidAndCounter(clustersArray); /* Update curr centroid to prev centroid and reset the counter */
        assignVectorsToClusters(vectorsArray, clustersArray);
        changes = recalcCentroids(clustersArray); /* Calculate new centroids */
        if (changes == 0) { /* Centroids stay unchanged in the current iteration */
            break;
        }
    }

    printFinalCentroids(clustersArray);
    freeMemoryVectorsClusters(vectorsArray, clustersArray);
    return 0;
}

void calcDimAndNumOfVectors() {
    char line[MAX_CHAR_AT_ONE_LINE];
    char *c;
    dimension = 1;
    numOfVectors = 1;

    scanf("%s", line);
    for (c = line; *c != '\0'; c++) {
        dimension += *c == COMMA_CHAR ? 1 : 0; /* Calc vectors' dimension */
    }

    while (scanf("%s", line) != EOF) {
        (numOfVectors)++; /* Calc number of vectors (datapoints) */
    }
    rewind(stdin); /* Move back to the beginning of the input file */
}

double **initVectorsArray() {
    int i, j;
    char ch;
    double *matrix, **vectorsArray;
    /* Allocate memory for vectorsArray */
    matrix = (double *) malloc(numOfVectors * (dimension + 1) * sizeof(double));
    assert(matrix != NULL);
    vectorsArray = malloc(numOfVectors * sizeof(double *));
    assert(vectorsArray != NULL);

    for (i = 0; i < numOfVectors; ++i) {
        vectorsArray[i] = matrix + i * (dimension + 1); /* Set VectorsArray to point to 2nd dimension array */
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
    clustersArray = (Cluster *) malloc(k * sizeof(Cluster));
    assert(clustersArray != NULL);

    for (i = 0; i < k; ++i) {
        clustersArray[i].prevCentroid = (double *) malloc(dimension * sizeof(double));
        clustersArray[i].currCentroid = (double *) malloc(dimension * sizeof(double));
        assert(clustersArray[i].prevCentroid != NULL && clustersArray[i].currCentroid != NULL);
        clustersArray[i].counter = 0;

        for (j = 0; j < dimension; ++j) {
            clustersArray[i].currCentroid[j] = vectorsArray[i][j]; /* Assign the first k vectors to their corresponding clusters */
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
    for (j = 1; j < k; ++j) { /* Find the min norm == closest cluster */
        norm = vectorsNorm(vec, clustersArray[j].prevCentroid);
        if (norm < minNorm) {
            myCluster = j;
            minNorm = norm;
        }
    }
    return myCluster;
}

void assignVectorsToClusters(double **vectorsArray, Cluster *clustersArray) {
    int j, i, myCluster;
    double *vec;
    for (j = 0; j < numOfVectors; ++j) {
        vec = vectorsArray[j];
        myCluster = findMyCluster(vectorsArray[j], clustersArray);
        vec[dimension] = myCluster; /* Set vector's cluster to his closest */
        for (i = 0; i < dimension; ++i) {
            clustersArray[myCluster].currCentroid[i] += vec[i]; /* Summation of the vectors Components */
        }
        clustersArray[myCluster].counter++; /* Count the number of vectors for each cluster */
    }
}

int recalcCentroids(Cluster *clustersArray) {
    int i, j, changes = 0;
    Cluster cluster;
    for (i = 0; i < k; ++i) {
        cluster = clustersArray[i];
        for (j = 0; j < dimension; ++j) {
            cluster.currCentroid[j] /= cluster.counter; /* Calc the mean value */
            changes += cluster.prevCentroid[j] != cluster.currCentroid[j] ? 1 : 0; /* Count the number of changed centroids' components */
        }
    }
    return changes;
}

void initCurrCentroidAndCounter(Cluster *clustersArray) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            clustersArray[i].prevCentroid[j] = clustersArray[i].currCentroid[j]; /* Set prev centroid to curr centroid */
            clustersArray[i].currCentroid[j] = 0; /* Reset curr centroid */
        }
        clustersArray[i].counter = 0; /* Reset counter */
    }
}

void printFinalCentroids(Cluster *clustersArray) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            if (j > 0)
                printf("%c", COMMA_CHAR);
            printf("%0.4f", clustersArray[i].currCentroid[j]); /* Print with an accuracy of 4 digits after the dot */
        }
        printf("\n");
    }
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
    free(*vectorsArray);
    free(vectorsArray);
}

void validateAndAssignInput(int argc, char **argv, int *maxIter) {
    char *nextCh;
    if (argc < K_ARGUMENT) {
        printf("The program needs at least one argument.");
        exit(0);
    }
    k = strtol(argv[K_ARGUMENT - 1], &nextCh, 10);
    if (k < 1 || *nextCh != END_OF_STRING) { /* Contain not only digits or integer less than 1 */
        printf("K argument must be an integer number greater than 0.");
        exit(0);
    }
    if (argc >= MAX_ITER_ARGUMENT) {
        *maxIter = strtol(argv[MAX_ITER_ARGUMENT - 1], &nextCh, 10);
        if (*maxIter < 1 || *nextCh != END_OF_STRING) { /* Contain not only digits or integer less than 1 */
            printf("Max iteration argument must be an integer number greater than 0.");
            exit(0);
        }
    } else {
        *maxIter = DEFAULT_NUM_OF_ITERS;
    }
}
