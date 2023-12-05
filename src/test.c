#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#define MAX_CHILDREN 10

// Structure representing a node in the F-heap
typedef struct Node {
    int key;
    int value;
    struct Node *parent;
    struct Node *child;
    struct Node *left;
    struct Node *right;
    bool mark;
    int degree;
} Node;

Node* pairwiseCombine(Node* heap);


// Function to create a new node
Node* createNode(int key, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->value = value;
    newNode->parent = NULL;
    newNode->child = NULL;
    newNode->left = newNode;
    newNode->right = newNode;
    newNode->mark = false;
    newNode->degree = 0;
    return newNode;
}

// Function to find a node with specific key and value in the F-heap
Node* findNode(Node* heap, int key, int value) {
    if (heap == NULL) {
        return NULL; // Heap is empty
    }

    Node* currentNode = heap;
    do {
        if (currentNode->key == key && currentNode->value == value) {
            return currentNode; // Node found
        }

        // Recursively search in the child nodes if they exist
        Node* child = currentNode->child;
        if (child != NULL) {
            Node* foundNode = findNode(child, key, value);
            if (foundNode != NULL) {
                return foundNode; // Node found in child subtree
            }
        }

        currentNode = currentNode->right; // Move to the next node in the circular doubly linked list
    } while (currentNode != heap);

    return NULL; // Node not found in the entire heap
}


// Function to merge two heaps
Node* mergeHeaps(Node* a, Node* b) {
    if (a == NULL) return b;
    if (b == NULL) return a;

    Node* aNext = a->right;
    Node* bPrev = b->left;

    a->right = b;
    b->left = a;
    aNext->left = bPrev;
    bPrev->right = aNext;

    return a->key < b->key ? a : b;
}

// Function to insert a new node into the heap
Node* insert(Node* heap, int key, int value) {
    Node* newNode = createNode(key, value);
    heap = mergeHeaps(heap, newNode);
    return heap;
}

// Function to remove a node from the heap
Node* removeNode(Node* node) {
    node->left->right = node->right;
    node->right->left = node->left;
    return node;
}

// Function to link a child node to a parent node
Node* link(Node* heap, Node* child, Node* parent) {
    heap = removeNode(child);
    child->parent = parent;
    if (parent->child == NULL) {
        parent->child = child;
        child->left = child;
        child->right = child;
    } else {
        Node* parentChild = parent->child;
        Node* parentChildPrev = parentChild->left;
        parentChild->left = child;
        child->right = parentChild;
        child->left = parentChildPrev;
        parentChildPrev->right = child;
    }
    parent->degree++;
    child->mark = false;
    return heap;
}

// Function to perform cascading cuts
Node* cascadingCut(Node* heap, Node* node) {
    Node* parent = node->parent;
    if (parent != NULL) {
        if (!node->mark) {
            node->mark = true;
        } else {
            heap = link(heap, node, parent);
            heap = cascadingCut(heap, parent);
        }
    }
    return heap;
}

// Function to decrease the key of a node by y
Node* decreaseKey(Node* heap, int key, int value, int y) {
    Node* currentNode = findNode(heap, key, value); // Assume findNode function exists

    if (currentNode == NULL) {
        printf("Node not found.\n");
        return heap; // Node not found in the heap
    }
    // Decrease the key by y
    currentNode->key -= y;

    if (currentNode == heap){
        return heap;
    }
    // Perform necessary adjustments to maintain heap property
    Node* parent = currentNode->parent;
    if (parent != NULL && currentNode->key < parent->key) {
        heap = cascadingCut(heap, currentNode); // Perform cascading cut
    }
    heap = mergeHeaps(heap, currentNode);
    return heap;
}

// Function to extract the minimum node from the heap
Node* extractMin(Node* heap, int* minKey, int* minValue) {
    if (heap == NULL) {
        *minKey = INT_MIN;
        *minValue = INT_MIN;
        return NULL;
    }

    Node* minNode = heap;
    Node* child = minNode->child;
    // int numChildren = minNode->degree;

    if (child != NULL) {
        Node* temp = child;
        do {
            Node* next = temp->right;
            heap = mergeHeaps(heap, temp);
            temp->parent = NULL;
            temp = next;
        } while (temp != child);
    }

    heap = removeNode(minNode);
    if (minNode == minNode->right) {
        heap = NULL;
    } else {
        heap = minNode->right;
        heap = pairwiseCombine(heap);
    }

    *minKey = minNode->key;
    *minValue = minNode->value;
    free(minNode);
    return heap;
}

// Function to perform pairwise combining of roots with the same degree
Node* pairwiseCombine(Node* heap) {
    if (heap == NULL) return NULL;

    const int maxDegree = 1 + (int)(log(MAX_CHILDREN) / log(2));
    Node* array[maxDegree];
    for (int i = 0; i < maxDegree; i++) {
        array[i] = NULL;
    }

    Node* current = heap;
    int numRoots = 0;
    do {
        numRoots++;
        current = current->right;
    } while (current != heap);

    while (numRoots > 0) {
        int degree = current->degree;
        Node* next = current->right;
        while (array[degree] != NULL) {
            Node* other = array[degree];
            if (current->key > other->key) {
                Node* temp = current;
                current = other;
                other = temp;
            }
            heap = link(heap, other, current);
            array[degree] = NULL;
            degree++;
        }
        array[degree] = current;
        current = next;
        numRoots--;
    }

    heap = NULL;
    for (int i = 0; i < maxDegree; i++) {
        if (array[i] != NULL) {
            if (heap == NULL) {
                heap = array[i];
            } else {
                heap = mergeHeaps(heap, array[i]);
            }
        }
    }

    return heap;
}

// Function to free the allocated memory for the heap
void freeHeap(Node* heap) {
    if (heap == NULL) return;

    Node* current = heap;
    do {
        Node* temp = current;
        current = current->right;
        freeHeap(temp->child);
        free(temp);
    } while (current != heap);
}


// Function to delete a node from the heap based on key and value
Node* deleteNode(Node* heap, int keyToDelete, int valueToDelete, int* minKey, int* minValue) {
    Node* nodeToDelete = findNode(heap, keyToDelete, valueToDelete);
    if (nodeToDelete == NULL) {
        // Node with key and value not found in the heap
        return heap;
    }
    
    int dif = keyToDelete - heap->key + 1;
    heap = decreaseKey(heap, keyToDelete, valueToDelete, dif); 
    
    // Remove the node from the heap
    heap = extractMin(heap,minKey,minValue);

    // // Perform cascading cuts to maintain heap property
    // Node* parent = nodeToDelete->parent;
    // if (parent != NULL) {
    //     heap = cascadingCut(heap, nodeToDelete);
    // }

    // // Handle root pointer if necessary
    // if (heap == nodeToDelete) {
    //     if (nodeToDelete->right == nodeToDelete) {
    //         heap = NULL;
    //     } else {
    //         heap = nodeToDelete->right;
    //         heap = pairwiseCombine(heap); // Perform pairwise combining if needed
    //     }
    // }

    // free(nodeToDelete); // Free the memory of the deleted node
    return heap;
}


int main() {
    Node* heap = NULL;
    int minKey, minValue;
    char operation[20];

    while (1) {
        if (scanf("%s", operation) == EOF || strcmp(operation, "quit") == 0) {
            break; // Exit the loop if "quit" operation is encountered or EOF
        }

        int x, val, y;
        if (strcmp(operation, "insert") == 0) {
            scanf("%d %d", &x, &val);
            heap = insert(heap, x, val);
        } else if (strcmp(operation, "delete") == 0) {
            scanf("%d %d", &x, &val);
            
            heap = decreaseKey(heap, x, val, 6000);
            heap = extractMin(heap, &minKey, &minValue);

        } else if (strcmp(operation, "decrease") == 0) {
            scanf("%d %d %d", &x, &val, &y);
            heap = decreaseKey(heap, x , val, y);
        } else if (strcmp(operation, "extract") == 0) {
            heap = extractMin(heap, &minKey, &minValue);
            printf("(%d)%d\n", minKey, minValue);
        }
    }






    freeHeap(heap); // Free memory allocated for the heap
    return 0;
}

//降低鍵值有問題
//cascading cut