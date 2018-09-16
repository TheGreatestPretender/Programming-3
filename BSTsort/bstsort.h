//structure creating Node type
typedef struct Node {
    char *data;
    int count;
    struct Node *leftChild;
    struct Node *rightChild;
} Node;
//creating methods needed to work with Nodes
void insert(Node **bst, char *data);
void inOrder(Node *p);
int compare(char *str1, char *str2);
int compareCasewise(char *str1, char *str2);
void reclaimMem(Node *p);
