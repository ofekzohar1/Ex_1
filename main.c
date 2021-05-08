#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define sq(x) ((x)*(x))

#define K_ARGUMENT 2
#define MAX_ITER_ARGUMENT 3
#define DEFAULT_NUM_OF_ITERS 200
#define MAX_CHAR_AT_ONE_LINE 1000
#define COMMA_CHAR ','
#define IRRELEVANT_DIFF 0.0001

static int k, num_of_vectors, dimension;

typedef struct {
    double *prev_centroid;
    double *curr_centroid;
    int counter;
} Cluster;

void calc_dim_and_num_of_vectors() {
    char line[MAX_CHAR_AT_ONE_LINE];
    char *c;
    dimension = 1;
    num_of_vectors = 1;

    scanf("%s", &line);
    for (c = line; *c != '\0'; c++) {
        dimension += *c == COMMA_CHAR ? 1 : 0;
    }

    while (scanf("%s", &line) != EOF) {
        (num_of_vectors)++;
    }
    rewind(stdin); // Move back to the beginning of the input file
}

double **init_vectors_array() {
    int i, j;
    char ch;
    double *matrix, **vectors_array;
    // Allocate memory for vectors_array
    assert(matrix = (double *) malloc(num_of_vectors * (dimension + 1) * sizeof(double)));
    assert(vectors_array = malloc(num_of_vectors * sizeof(double *)));

    for (i = 0; i < num_of_vectors; ++i) {
        vectors_array[i] = matrix + i * (dimension + 1);
        for (j = 0; j < dimension; ++j) {
            scanf("%lf%c", &vectors_array[i][j], &ch);
            //  printf("%lf  ", vectors_array[i][j]);
        }
        // printf("\n");
    }
    return vectors_array;
}

Cluster *init_clusters(double **vectors_array) {
    int i, j;
    Cluster *clusters_array;
    // Allocate memory for clusters_array
    assert(clusters_array = (Cluster *) malloc(k * sizeof(Cluster)));

    for (i = 0; i < k; ++i) {
        assert(clusters_array[i].prev_centroid = (double *) malloc(dimension * sizeof(double)));
        assert(clusters_array[i].curr_centroid = (double *) malloc(dimension * sizeof(double)));
        clusters_array[i].counter = 0;

        for (j = 0; j < dimension; ++j) {
            clusters_array[i].curr_centroid[j] = vectors_array[i][j];
            // printf("%lf  ", clusters_array[i].prev_centroid[j]);
        }
        // printf("\n");
    }
    return clusters_array;
}

double vectors_norm(double *vec1, double *vec2) {
    double norm = 0;
    int i;
    for (i = 0; i < dimension; ++i) {
        norm += sq(vec1[i] - vec2[i]);
    }
    return norm;
}

int find_my_cluster(double *vec, Cluster *clusters_array) {
    int my_cluster, j;
    double min_norm, norm;

    my_cluster = 0;
    min_norm = vectors_norm(vec, clusters_array[0].prev_centroid);
    for (j = 1; j < k; ++j) {
        norm = vectors_norm(vec, clusters_array[j].prev_centroid);
        if (norm < min_norm) {
            my_cluster = j;
            min_norm = norm;
        }
    }
    return my_cluster;
}

void assgin_vectors_to_clusters(double **vectors_array, Cluster *clusters_array) {
    int j, i, my_cluster;
    double *vec;
    for (j = 0; j < num_of_vectors; ++j) {
        vec = vectors_array[j];
        my_cluster = find_my_cluster(vectors_array[j], clusters_array);
        vec[dimension] = my_cluster;
        for (i = 0; i < dimension; ++i) {
            clusters_array[my_cluster].curr_centroid[i] += vec[i];
        }
        clusters_array[my_cluster].counter++;
    }
}

int recalc_centroids(Cluster *clusters_array) {
    int i, j, changes = 0;
    Cluster cluster;
    for (i = 0; i < k; ++i) {
        cluster = clusters_array[i];
        for (j = 0; j < dimension; ++j) {
            cluster.curr_centroid[j] /= cluster.counter;
            changes += abs(cluster.prev_centroid[j] - cluster.curr_centroid[j]) > IRRELEVANT_DIFF ? 1 : 0;
        }
    }
    return changes;
}

void init_curr_centroid_and_counter(Cluster *clusters_array) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            clusters_array[i].prev_centroid[j] = clusters_array[i].curr_centroid[j];
            clusters_array[i].curr_centroid[j] = 0;
        }
        clusters_array[i].counter = 0;
    }
}

void print_final_centroids(Cluster* clusters_array) {
    int i, j;
    for (i = 0; i < k; ++i) {
        for (j = 0; j < dimension; ++j) {
            if (j > 0)
                printf("%c",COMMA_CHAR);
            printf("%lf.4", clusters_array[i].curr_centroid[j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int max_iter, i, changes;
    double **vectors_array;
    Cluster *clusters_array;

    if (argc < K_ARGUMENT) {
        printf("The program needs at least one argument");
        return 0;
    }
    assert ((k = atoi(argv[K_ARGUMENT - 1])) > 0);
    if (argc >= MAX_ITER_ARGUMENT) {
        assert ((max_iter = atoi(argv[MAX_ITER_ARGUMENT - 1])) > 0);
    } else {
        max_iter = DEFAULT_NUM_OF_ITERS;
    }

    calc_dim_and_num_of_vectors();
    assert(k <= num_of_vectors); // Number of clusters can't be more than the number of vectors

    vectors_array = init_vectors_array();
    clusters_array = init_clusters(vectors_array);
    print_final_centroids(clusters_array);

    for (i = 0; i < max_iter; ++i) {
        init_curr_centroid_and_counter(clusters_array);
        assgin_vectors_to_clusters(vectors_array, clusters_array);
        changes = recalc_centroids(clusters_array);
        print_final_centroids(clusters_array);
        if (changes == 0) { // Centroids stay unchanged in the current iteration
            break;
        }
    }

    print_final_centroids(clusters_array);

    return 0;
}
