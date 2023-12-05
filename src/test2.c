#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
    int key;
    Color color;
    struct Node *left, *right, *parent;
} Node;
//
Node* createNode(int key);
Node* BSTInsert(Node* root, Node* newNode);
Node* minimum(Node* x);
void leftRotate(Node** root, Node* x);
void rightRotate(Node** root, Node* y);
void RBInsertFixup(Node** root, Node* z);
void RBTransplant(Node** root, Node* u, Node* v);
void RBDeleteFixup(Node** root, Node* x);
void RBInsert(Node** root, int key);
Node* search(Node* root, int key);
void RBDelete(Node** root, int key);
void inorderTraversal(Node* root);
//
Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode != NULL) {
        newNode->key = key;
        newNode->color = RED;
        newNode->left = newNode->right = newNode->parent = NULL;
    }
    return newNode;
}

Node* BSTInsert(Node* root, Node* newNode) {
    Node *parent = NULL, *current = root;
    while (current != NULL) {
        parent = current;
        if (newNode->key < current->key)
            current = current->left;
        else
            current = current->right;
    }
    newNode->parent = parent;
    if (parent == NULL)
        root = newNode;
    else if (newNode->key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;

    return root;
}

void leftRotate(Node** root, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rightRotate(Node** root, Node* y) {
    Node* x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL)
        *root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void RBInsertFixup(Node** root, Node* z) {
    while (z != *root && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node* y = z->parent->parent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(root, z->parent->parent);
            }
        } else {
            Node* y = z->parent->parent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

void RBTransplant(Node** root, Node* u, Node* v) {
    if (u->parent == NULL)
        *root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v != NULL)
        v->parent = u->parent;
}



void RBDeleteFixup(Node** root, Node* x) {
    while (x != *root && x != NULL && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w != NULL) {
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(root, x->parent);
                    w = x->parent->right;
                }
                if ((w->left == NULL || w->left->color == BLACK) && (w->right == NULL || w->right->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right == NULL || w->right->color == BLACK) {
                        if (w->left != NULL)
                            w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(root, w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right != NULL)
                        w->right->color = BLACK;
                    leftRotate(root, x->parent);
                    x = *root;
                }
            }
        } else {
            Node* w = x->parent->left;
            if (w != NULL) {
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(root, x->parent);
                    w = x->parent->left;
                }
                if ((w->right == NULL || w->right->color == BLACK) && (w->left == NULL || w->left->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left == NULL || w->left->color == BLACK) {
                        if (w->right != NULL)
                            w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(root, w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left != NULL)
                        w->left->color = BLACK;
                    rightRotate(root, x->parent);
                    x = *root;
                }
            }
        }
    }
    if (x != NULL)
        x->color = BLACK;
}

Node* minimum(Node* x) {
    while (x->left != NULL)
        x = x->left;
    return x;
}

void RBInsert(Node** root, int key) {
    Node* z = createNode(key);
    *root = BSTInsert(*root, z);
    RBInsertFixup(root, z);
}

Node* search(Node* root, int key) {
    while (root != NULL && root->key != key) {
        if (key < root->key)
            root = root->left;
        else
            root = root->right;
    }
    return root;
}

void RBDelete(Node** root, int key) {
    Node *z = *root, *x, *y;
    while (z != NULL) {
        if (key == z->key)
            break;
        else if (key < z->key)
            z = z->left;
        else
            z = z->right;
    }
    if (z == NULL) {
        printf("Key not found in the tree.\n");
        return;
    }
    y = z;
    Color y_original_color = y->color;
    if (z->left == NULL) {
        x = z->right;
        RBTransplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        RBTransplant(root, z, z->left);
    } else {
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else {
            RBTransplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        RBTransplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);
    if (y_original_color == BLACK)
        RBDeleteFixup(root, x);
}


void inorderTraversal(Node* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        printf("%d(%s) ", root->key, root->color == RED ? "RED" : "BLACK");
        inorderTraversal(root->right);
    }
}

int main() {
    Node* root = NULL;
    Node* searchNode = NULL;
    char operation[10];
    int value;
    while (1) {
        if (scanf("%s", operation) == EOF || strcmp(operation, "quit") == 0) {
            break; // Exit the loop if "quit" operation is encountered or EOF
        }

        if (strcmp(operation, "insert") == 0) {
            scanf("%d", &value);
            RBInsert(&root,value);
        }else if (strcmp(operation, "delete") == 0) {
            scanf("%d", &value);
            RBDelete(&root,value);
        }else if (strcmp(operation, "search") == 0) {
            scanf("%d", &value);
            searchNode = search(root, value);
            if (searchNode ==NULL){
              printf("not found\n");
            }else if (searchNode->color == BLACK)
            {
              printf("black\n");
            }else{
              printf("red\n");
            }

        }else if (strcmp(operation, "print") == 0) {
            inorderTraversal(root);
            printf("\n");
        } else if (strcmp(operation, "quit") == 0) {
            break;
        }
    }
}
