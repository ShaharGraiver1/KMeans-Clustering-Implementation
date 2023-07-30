# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double EPS = 0.001;

/*struct Cord - each cord represents a Node in Linked List */
typedef struct Cord
{
    double value;
    struct Cord *next;
}Cord;

/* Create and delete Cords */
void freeCords(Cord* head) {
    Cord* current = head;
    Cord* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

Cord * createCord(double val){
    Cord * cord = (Cord*)malloc(sizeof(Cord));
    cord->value = val;
    cord->next = NULL;
    return cord;
}

Cord * copyCordLL(const Cord* curr_org){
    Cord * head_copy = createCord(curr_org->value);
    Cord * curr_copy = head_copy;
    Cord * next_org = curr_org->next;
    while (next_org != NULL){
        curr_org = next_org;
        next_org = next_org->next;
        curr_copy->next = createCord(curr_org->value);
        curr_copy = curr_copy->next;
    }
    return head_copy;

}

/*struct Vector - each vector represents Node in Linked List */
typedef struct Vector
{
    struct Vector *next;
    Cord *cords;
}Vector;

/*Create and delete Vectors*/
void freeVectors(Vector* head) {
    Vector* current = head;
    Vector* next;

    while (current != NULL) {
        next = current->next;
        freeCords(current->cords);
        free(current);
        current = next;
    }
}

Vector* createVector(Cord* head_cord){
    Vector * vector = (Vector*)malloc(sizeof(Vector));
    vector->cords = head_cord;
    vector->next = NULL;
    return vector;
}

/* Array of linked lists.
 * Cluster.c_list[i] = Linked List of vectors.
 * c_list[i][0] = current Centroid mean .
 * c_list[i][1] = one iteration back - previous Centroid mean .
 * c_list[i][2:] = Linked List of all vectors in i'th cluster */
typedef struct Cluster {
    int k;
    int d;
    Vector** c_list;
}Cluster;

Cluster* createCluster(int k, int d, Vector* curr_vec) {
    Cluster* cluster = (Cluster*)malloc(sizeof(Cluster));
    int i; /*index for loop*/
    cluster->k = k;
    cluster->d = d;
    cluster->c_list = (Vector**) malloc(k*sizeof(Vector*));
    /* set default centroids --> first k vectors in vector list */
    for (i = 0; i < k; i++) {
        Cord* cord_copy_1 = copyCordLL(curr_vec->cords);
        Vector* vec_copy_1 = createVector(cord_copy_1);
        Cord* cord_copy_2 = copyCordLL(curr_vec->cords);
        Vector* vec_copy_2 = createVector(cord_copy_2);
        vec_copy_1->next = vec_copy_2;
        cluster->c_list[i] = vec_copy_1;
        curr_vec = curr_vec->next;
    }
    return cluster;
}

/*
 * Functional Methods
 */
double distance(const Vector* x,const Vector* y){
    Cord* curr_cord_x = x->cords;
    Cord* curr_cord_y = y->cords;
    double sum =0;
    while(curr_cord_x != NULL){
        sum += pow((curr_cord_x->value - curr_cord_y->value), 2);
        curr_cord_x = curr_cord_x->next;
        curr_cord_y = curr_cord_y->next;
    }
    return pow(sum,0.5);
}

int countNumVectors(Vector* head){
    int cnt = 0;
    Vector* curr = head;

    while(curr != NULL){
        cnt ++;
        curr = curr->next;
    }
    return cnt;
}

int countCoordinates(Vector* vector){
    Cord* curr_cord = vector->cords;
    int cnt = 0;
    while(curr_cord != NULL){
        cnt ++;
        curr_cord = curr_cord->next;
    }
    return cnt;
}

Vector * input_to_vector_lst(){
    /* create one single Linked List of Vectors from input txt file */
    Vector *head_vec, *curr_vec, *last_vec, *to_delete_vec;
    Cord *head_cord, *curr_cord;
    double n;
    char c;

    head_cord = createCord(0);
    curr_cord = head_cord;

    head_vec = createVector(head_cord);
    curr_vec = head_vec;

    while (scanf("%lf%c", &n, &c) == 2)
    {
        if (c == '\n')
        {
            curr_cord->value = n;
            curr_cord->next = NULL;       /* last coordinate */
            curr_vec->cords = head_cord; /* Done Vector, now points to first coordinate */

            last_vec = curr_vec;

            head_cord = createCord(0);
            curr_cord = head_cord;

            curr_vec->next = createVector(head_cord);
            curr_vec = curr_vec->next;
            continue;
        }

        curr_cord->value = n;
        curr_cord->next = createCord(0);
        curr_cord = curr_cord->next;
    }
    to_delete_vec = last_vec->next;
    freeCords(to_delete_vec->cords); /* free memory of old prev */
    free(to_delete_vec);
    last_vec->next = NULL;
    return head_vec;
}

void appendToCluster(Vector* first_vec, Vector* new_vec){
    /* add a vector to the end of the Linked_List */
    Vector* curr_vector = first_vec;

    while (curr_vector->next != NULL) {
        curr_vector = curr_vector->next;
    }

    curr_vector->next = new_vec;
    new_vec->next = NULL;
}

void findClosestCluster(Cluster* cluster, Vector* new_vec){
    /* given new_vec find the closest cluster and append it there */
    Vector * first_centroid = cluster->c_list[0];
    int index = 0;
    double min_dist = distance(new_vec, first_centroid);
    Vector * first_vec;
    int i;
    for (i = 1; i < cluster->k; i++) {
        double curr_dist = distance(new_vec, cluster->c_list[i]);
        if(curr_dist < min_dist){
            min_dist = curr_dist;
            index = i;
        }
    }
    first_vec = cluster->c_list[index];
    appendToCluster(first_vec, new_vec);
}

Vector * calcCentroid(Cluster* cluster, int index){
    /* calculate the new Mean vector (AKA centorid)
     * for all vectors in cluster->c_list[index]:
     *      sum each coordinate to the corresponding coordinate in mean_vector
     * divide each coordinate of mean_vector by the number of vectors */

    int j; /*index for first loop*/
    int i; /*index for second loop*/
    int num_of_vectors;
    Cord * mean_cord;
    Vector* mean_vector;
    Cord* curr_cord;

    Vector * curr_vector = cluster->c_list[index];
    curr_vector = curr_vector->next->next; /* elements in index 0,1 in c_list[i] are curr and prev vectors-->skip */
    num_of_vectors = countNumVectors(curr_vector);

    mean_cord = copyCordLL(curr_vector->cords);
    mean_vector = createVector(mean_cord);
    curr_vector = curr_vector->next;

    for (j = 1; j < num_of_vectors; j++) {
        curr_cord = curr_vector->cords;
        mean_cord = mean_vector->cords;
        for (i = 0; i < cluster->d; i++){
            mean_cord->value += curr_cord->value;
            curr_cord = curr_cord->next;
            mean_cord = mean_cord->next;
        }
        curr_vector = curr_vector->next;
    }
    mean_cord = mean_vector->cords;
    for (i = 0; i < cluster->d; i++) {
        mean_cord->value = mean_cord->value/num_of_vectors;
        mean_cord = mean_cord->next;
    }
    return mean_vector;
}

void updateCentroidList(Cluster* cluster){
    /* for all centroids in cluster:
     *      calculate new mean_vector and add it LL.head.
     *      advance current mean to be previous mean.
     */
    int i;
    for (i = 0; i < cluster->k; i++) {
        Vector * curr_mean = calcCentroid(cluster, i);
        Vector * new_prev = cluster->c_list[i]; /* old curr becomes new prev */
        Vector * to_delete = new_prev->next;
        new_prev->next = to_delete->next;
        freeCords(to_delete->cords); /* free memory of old prev */
        free(to_delete);
        curr_mean->next = new_prev;
        cluster->c_list[i] = curr_mean;
    }
}

int checkConvergence(Cluster* cluster){
    /* check if the distance between curr_mean and prev_mean is less the EPS */
    int i;
    for (i = 0; i < cluster->k; ++i) {
        Vector * curr_mean = cluster->c_list[i];
        Vector * prev_mean = curr_mean->next;
        double dist = distance(curr_mean,prev_mean);
        if(dist >= EPS){
            return 0;
        }
    }
    return 1;
}

void deleteVectors(Cluster* cluster){
    /* delete all vectors from cluster->c_list and keep only curr and prev mean vectors */
    int i;
    Vector * vector_to_keep;
    Vector * vector_to_delete;
    for (i = 0; i < cluster->k; ++i) {
        vector_to_keep = cluster->c_list[i];
        vector_to_keep = vector_to_keep->next;
        vector_to_delete = vector_to_keep->next;
        freeVectors(vector_to_delete);
        vector_to_keep->next = NULL;
    }
}

void printCentroids(Cluster* cluster){
    /* print all the current centroids */
    int i;
    for (i = 0; i < cluster->k; i++) {
        Vector *curr_centroid = cluster->c_list[i];
        Cord *curr_cord = curr_centroid->cords;
        while (curr_cord != NULL) {
            printf("%.4f", curr_cord->value);
            if (curr_cord->next != NULL) {
                printf(",");
            }
            curr_cord = curr_cord->next;
        }
        printf("\n");
    }
}

void freeDataStracture(Vector* head_vector, Cluster* cluster){
    int i;
    freeVectors(head_vector);
    for (i = 0; i< cluster->k; i++){
        freeVectors(cluster->c_list[i]);
    }
    free(cluster->c_list);
    free(cluster);
}


int main(int argc, char *argv[]) {
    /* receive input from user and initiate Data Structures */
    int k = atoi(argv[1]);
    int itr = 200;
    Vector * head_vector = input_to_vector_lst();
    Vector * curr_vector = head_vector;
    int line_count = countNumVectors(head_vector);
    int error_cnt=0;
    int cnt = 0; /*cnt counts number of iteration*/
    Cluster* cluster;

    if ((argc > 3)){
        printf("An Error Has Occurred");
        printf("\n");
        freeVectors(head_vector);
        return 0;
    }

    /* check the input is valid */
    if (argc > 2){ /*iter is not provided */
        itr = atoi(argv[2]);
    }
    if ((k <= 1)||(k>=line_count)){
        error_cnt++;
        printf("Invalid number of clusters!");
        printf("\n");
    }
    if ((itr <= 1)||(itr>=1000)){
        error_cnt++;
        printf("Invalid maximum iteration!");
        printf("\n");
    }
    if (error_cnt>0){
        freeVectors(head_vector);
        return 0;
    }

    /* Main Flow */
    cluster = createCluster(k, countCoordinates(head_vector), curr_vector);
    curr_vector = head_vector;
    while(cnt < itr){   /* iterate over the file until reached limit iterations */
        while(curr_vector != NULL){ /* iterate through all vectors in the input file */
            Cord* cord_copy = copyCordLL(curr_vector->cords);
            Vector* curr_copy = createVector(cord_copy);
            findClosestCluster(cluster, curr_copy);
            curr_vector = curr_vector->next;
        }
        cnt++;
        curr_vector = head_vector;
        updateCentroidList(cluster);
        if (checkConvergence(cluster) == 1) {
            break;
        }
        deleteVectors(cluster);
    }

    /* print results and clean memory */
    printCentroids(cluster);
    freeDataStracture(head_vector, cluster);
    return 0;
}

