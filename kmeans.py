import sys

DEFAULT_ITER = 200
MIN_ARGUMENTS = 2
COMMA = ','


class Cluster:
    def __init__(self, central):
        self.prev_central = [0] * len(central)
        self.curr_central = central
        self.counter = 0

    def init_curr_central(self):
        self.prev_central = self.curr_central
        self.curr_central = [0] * len(self.prev_central)
        self.counter = 0


# returns a distance between 2 vectors
def distance(vector_1, vector_2):
    distance_between_vectors = 0
    for i in range(len(vector_1)):
        distance_between_vectors += ((vector_1[i] - vector_2[i]) ** 2)
    return distance_between_vectors


# receives a list of clusters, transforms in the loop to the central_vector
# builds a list of distances from each cluster to a vector
# uses the distance function
# returns the index of the closest centroids
def finding_cluster(list_of_clusters, vector):
    distance_list = []
    for cluster in list_of_clusters:
        distance_between_central_to_vector = distance(cluster.prev_central, vector)
        distance_list.append(distance_between_central_to_vector)
    return distance_list.index(min(distance_list))


# reads txt file and converts it to a list of vectors
# returns a list of vectors
def build_list_of_vectors(k):
    list_of_vectors = []
    while True:
        try:
            for line in input().split('\n'):
                vector_build = [float(x) for x in line.split(COMMA)]
                vector_build.append(0)
                list_of_vectors.append(vector_build)
        except EOFError:
            break
    amount_of_vectors = len(list_of_vectors)
    if k >= amount_of_vectors:
        print(f"K must be smaller than the number of vectors: K = {k}, number of vectors = {amount_of_vectors}")
        exit()  # End program k >= n
    return list_of_vectors


# void - initialize clusters from the list of vectors
# param - clusters, all vectors, amount of clusters, dimensions
def create_centrals_for_clusters(list_of_vectors, k):
    list_of_clusters = [Cluster(list_of_vectors[i][:-1]) for i in range(k)]
    return list_of_clusters


# initialize clusters after each iteration
# using Cluster class init curr central method
def init_curr_centroid_and_counter(list_of_clusters):
    for cluster in list_of_clusters:
        cluster.init_curr_central()


# uses an extra dimension in the vectors to assign which cluster they are in
# creates for the cluster a new curr central that later on will be adjusted by his amount of vectors
def assign_vectors_to_clusters(list_of_vectors, list_of_clusters, dimensions):
    for vector in list_of_vectors:
        my_cluster = finding_cluster(list_of_clusters, vector)
        vector[dimensions] = my_cluster
        list_of_clusters[my_cluster].counter += 1
        for i in range(dimensions):
            list_of_clusters[my_cluster].curr_central[i] += vector[i]


# calculates the new central after knowing the amount of new vectors in his cluster
# returns a value that shows if the vector is changed more thr the relevant difference
def recalc_centroids(list_of_clusters, dimensions):
    is_centroid_changed = False
    for cluster in list_of_clusters:
        for i in range(dimensions):
            cluster.curr_central[i] /= cluster.counter
            if cluster.prev_central[i] != cluster.curr_central[i]:
                is_centroid_changed = True
    return is_centroid_changed


# prints new central after adjusting for the relevant structure
def print_centrals(list_of_clusters):
    for cluster in list_of_clusters:
        for i in range(len(cluster.curr_central)):
            cluster.curr_central[i] = "{:.4f}".format(cluster.curr_central[i])
        print(*cluster.curr_central, sep=COMMA)


def validate_and_assign_input_user():
    if len(sys.argv) < MIN_ARGUMENTS:
        print(f"Amount of arguments should be at least {MIN_ARGUMENTS - 1}: amount of arguments = {len(sys.argv) - 1}")
        exit()  # End program, min arguments
    if (not sys.argv[1].isdigit()) or int(sys.argv[1]) < 1:
        print(f"K input has to be a number and should exceed 0: k = {sys.argv[1]}")
        exit()  # End program, not valid k
    k = int(sys.argv[1])
    max_iter = DEFAULT_ITER
    if len(sys.argv) > MIN_ARGUMENTS:
        if (sys.argv[2].isdigit()) and int(sys.argv[2]) > 0:
            max_iter = sys.argv[2]
        else:
            print(f"max_iter input has to be a number and should exceed 0: max_iter = {sys.argv[2]}")
            exit()  # End program, not valid max_iter
    return k, max_iter


def main():
    k, max_iter = validate_and_assign_input_user()
    list_of_vectors = build_list_of_vectors(k)
    list_of_clusters = create_centrals_for_clusters(list_of_vectors, k)
    dimensions = len(list_of_vectors[0]) - 1  # vectors have extra dimension to hold their cluster allocation

    for num_iteration in range(int(max_iter)):
        init_curr_centroid_and_counter(list_of_clusters)
        assign_vectors_to_clusters(list_of_vectors, list_of_clusters, dimensions)
        is_centroid_changed = recalc_centroids(list_of_clusters, dimensions)
        if not is_centroid_changed:  # The centroids convened
            break
    print_centrals(list_of_clusters)


if __name__ == '__main__':
    main()
