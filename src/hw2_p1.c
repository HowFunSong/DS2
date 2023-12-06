// Operations on a Fibonacci heap in C

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>

typedef struct _NODE {
  int key;
  int value;
  int degree;
  struct _NODE *left_sibling; 
  struct _NODE *right_sibling;
  struct _NODE *parent;
  struct _NODE *child;
  bool mark;
  bool visited;
} NODE;

typedef struct fibanocci_heap {
  int n;
  NODE *min;
  int phi;
  int degree;
} FIB_HEAP;

//定義所有函數
FIB_HEAP *make_fib_heap();
void insertion(FIB_HEAP *H, NODE *new, int key, int value);
NODE *extract_min(FIB_HEAP *H);
void consolidate(FIB_HEAP *H);
void fib_heap_link(FIB_HEAP *H, NODE *y, NODE *x);
NODE *find_min_node(FIB_HEAP *H);
void decrease_key(FIB_HEAP *H, NODE *node, int newkey);
void cut(FIB_HEAP *H, NODE *node_to_be_decrease, NODE *parent_node);
void cascading_cut(FIB_HEAP *H, NODE *parent_node);
void Delete_Node(FIB_HEAP *H, int dec_key, int val);
NODE *find_node(FIB_HEAP *H, NODE *n, int key, int new_key);

//定義F-heap
FIB_HEAP *make_fib_heap() {
  FIB_HEAP *H;
  H = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
  H->n = 0;
  H->min = NULL;
  H->phi = 0;
  H->degree = 0;
  return H;
}

// Printing the heap
void print_heap(FIB_HEAP *H,NODE *n) {
  if (n == NULL){

    printf("heap is empty\n");
    return;
  }
  NODE *x;
  int sibling_count = 0;
  int sibling[10] ; 
  for (x = n;; x = x->right_sibling) {
    sibling[sibling_count] = x->key;
    sibling_count ++;
    if (x == H-> min ){
      printf("Node(%d)%d is root\n", x->key, x->value);
    }else if (x->parent == NULL){
      printf("Node(%d)%d is sibling of root \n", x->key, x->value);
    }else{
      printf("Node(%d)%d parent is (%d)%d\n", x->key,x->value ,x->parent->key,x->parent->value);
    }


    if (x->child == NULL) {
      printf("Node(%d)%d with no child \n", x->key, x->value);
    } else {
      printf("NODE(%d)%d with child (%d)\n", x->key, x->value, x->child->key);
      print_heap(H,x->child);
    }
    if (x->right_sibling == n) {
      printf("Node(%d)%d siblings \n", n->key, n->value);
      for (int i = 0 ; i < sibling_count; i++){
        printf("| %d ", sibling[i]);
      }
      printf("|\n");
      break;
    }
  }
}

// Inserting nodes
void insertion(FIB_HEAP *H, NODE *new, int key, int value) {
  new = (NODE *)malloc(sizeof(NODE));
  new->key = key;
  new->value = value;
  new->degree = 0;
  new->mark = false;
  new->parent = NULL;
  new->child = NULL;
  new->visited = false;
  new->left_sibling = new;
  new->right_sibling = new;
  if (H->min == NULL) {
    H->min = new;
  } else {
    H->min->left_sibling->right_sibling = new;
    new->right_sibling = H->min;
    new->left_sibling = H->min->left_sibling;
    H->min->left_sibling = new;
    if (new->key < H->min->key) {
      H->min = new;
    }
  }
  (H->n)++;
}

// Find min node
NODE *find_min_node(FIB_HEAP *H) {
  if (H == NULL) {
    printf(" \n Fibonacci heap not yet created \n");
    return NULL;
  } else
    return H->min;
}

// Union operation
FIB_HEAP *unionHeap(FIB_HEAP *H1, FIB_HEAP *H2) {
  FIB_HEAP *Hnew;
  Hnew = make_fib_heap();
  Hnew->min = H1->min;

  NODE *temp1, *temp2;
  temp1 = Hnew->min->right_sibling;
  temp2 = H2->min->left_sibling;

  Hnew->min->right_sibling->left_sibling = H2->min->left_sibling;
  Hnew->min->right_sibling = H2->min;
  H2->min->left_sibling = Hnew->min;
  temp2->right_sibling = temp1;

  if ((H1->min == NULL) || (H2->min != NULL && H2->min->key < H1->min->key))
    Hnew->min = H2->min;
  Hnew->n = H1->n + H2->n;
  return Hnew;
}

// Calculate the degree
int cal_degree(int n) {
  int count = 0;
  while (n > 0) {
    n = n / 2;
    count++;
  }
  return count;
}

// Consolidate function
void consolidate(FIB_HEAP *H) {
  if (H->min == H->min->right_sibling){
    return;
  }
  int degree, i, d;
  degree = cal_degree(H->n);
  NODE *A[degree], *x, *y, *z;
  for (i = 0; i <= degree; i++) {
    A[i] = NULL;
  }
  x = H->min;
  do {
    d = x->degree;
    while (A[d] != NULL) {
      y = A[d];
      if (x->key > y->key) {
        NODE *exchange_help;
        exchange_help = x;
        x = y;
        y = exchange_help;
      }
      if (y == H->min)
        H->min = x;
      fib_heap_link(H, y, x);
      if (y->right_sibling == x)
        H->min = x;
      A[d] = NULL;
      d++;
    }
    A[d] = x;
    x = x->right_sibling;
  } while (x != H->min);

  H->min = NULL;
  for (i = 0; i < degree; i++) {
    if (A[i] != NULL) {
      A[i]->left_sibling = A[i];
      A[i]->right_sibling = A[i];
      if (H->min == NULL) {
        H->min = A[i];
      } else {
        H->min->left_sibling->right_sibling = A[i];
        A[i]->right_sibling = H->min;
        A[i]->left_sibling = H->min->left_sibling;
        H->min->left_sibling = A[i];
        if (A[i]->key < H->min->key) {
          H->min = A[i];
        }
      }
      if (H->min == NULL) {
        H->min = A[i];
      } else if (A[i]->key < H->min->key) {
        H->min = A[i];
      }
    }
  }
}

// Linking
void fib_heap_link(FIB_HEAP *H, NODE *y, NODE *x) {
  y->right_sibling->left_sibling = y->left_sibling;
  y->left_sibling->right_sibling = y->right_sibling;

  if (x->right_sibling == x)
    H->min = x;

  y->left_sibling = y;
  y->right_sibling = y;
  y->parent = x;

  if (x->child == NULL) {
    x->child = y;
  }
  y->right_sibling = x->child;
  y->left_sibling = x->child->left_sibling;
  x->child->left_sibling->right_sibling = y;
  x->child->left_sibling = y;
  if ((y->key) < (x->child->key))
    x->child = y;

  (x->degree)++;
}

// Extract min
NODE *extract_min(FIB_HEAP *H) {
  if (H->min == NULL){
    // printf("\n --The heap is empty--\n");
    return NULL;
  }else {
    NODE *temp = H->min;
    NODE *pntr;
    pntr = temp;
    NODE *x = NULL;
    if (temp->child != NULL) {
      x = temp->child;
      do {
        pntr = x->right_sibling;
        (H->min->left_sibling)->right_sibling = x;
        x->right_sibling = H->min;
        x->left_sibling = H->min->left_sibling;
        H->min->left_sibling = x;
        if (x->key < H->min->key)
          H->min = x;
        x->parent = NULL;
        x = pntr;
      } while (pntr != temp->child);
    }

    (temp->left_sibling)->right_sibling = temp->right_sibling;
    (temp->right_sibling)->left_sibling = temp->left_sibling;
    H->min = temp->right_sibling;

    if (temp == temp->right_sibling && temp->child == NULL)
      H->min = NULL;
    else {
      H->min = temp->right_sibling;
      // printf("call consolidate\n");
      consolidate(H);
    }
    // printf("finish consolidate\n");
    H->n = H->n - 1;
    return temp;
  }
  return H->min;
}

void cut(FIB_HEAP *H, NODE *node_to_be_decrease, NODE *parent_node) {
  NODE *temp_parent_check;

  if (node_to_be_decrease == node_to_be_decrease->right_sibling){
      parent_node->child = NULL;
      // return;
  }

  node_to_be_decrease->left_sibling->right_sibling = node_to_be_decrease->right_sibling;
  node_to_be_decrease->right_sibling->left_sibling = node_to_be_decrease->left_sibling;
  if (node_to_be_decrease == parent_node->child)
    parent_node->child = node_to_be_decrease->right_sibling;
  (parent_node->degree)--;

  node_to_be_decrease->left_sibling = node_to_be_decrease;
  node_to_be_decrease->right_sibling = node_to_be_decrease;
  H->min->left_sibling->right_sibling = node_to_be_decrease;
  node_to_be_decrease->left_sibling = H->min->left_sibling;
  node_to_be_decrease->right_sibling = H->min;
  H->min->left_sibling = node_to_be_decrease;

  node_to_be_decrease->parent = NULL;
  node_to_be_decrease->mark = false;
}

void cascading_cut(FIB_HEAP *H, NODE *current_node) {
  NODE *parent;

  if (current_node == H->min){
    (current_node->degree) --;
    // printf("%d %d",current_node->key, H->min->key);
    return;
  }
  parent = current_node->parent;
  // printf(" CurrentNode  : %d, ParentNode: %d, heap-min : %d\n", current_node->key ,parent->key ,H->min->key);
  if (parent != NULL) {
    if (current_node->mark == false) {
      current_node->mark = true;
    } else {
      // printf("\n cut called");
      cut(H, current_node, parent);
      // printf("\n cascading cut called\n");
      // printf("\n now (%d), after cut at (%d) left side \n", current_node->key, current_node->right_sibling->key);
      // printf("\n Process parent (%d)\n", parent->key);
      cascading_cut(H, parent);
    }
  }
  return ;
}

void decrease_key(FIB_HEAP *H, NODE *node_to_be_decrease, int new_key) {
  NODE *parent_node;
  if (H == NULL) {
    printf("\n FIbonacci heap not created ");
    return;
  }
  if (node_to_be_decrease == NULL) {
    printf("Node is not in the heap");
    
  }else {
    if (node_to_be_decrease->key < new_key) {
      printf("\n Invalid new key for decrease key operation \n ");
    } else {
      node_to_be_decrease->key = new_key;
      parent_node = node_to_be_decrease->parent;
      if (node_to_be_decrease == H->min){
        return;
      }
      if ((parent_node != NULL) && (node_to_be_decrease->key < parent_node->key)) {
        // printf("\n cut called");
        cut(H, node_to_be_decrease, parent_node);
        // printf("\n cascading cut called\n");
        // printf("\n now (%d), after cut at (%d) left side \n", node_to_be_decrease->key, node_to_be_decrease->right_sibling->key);
        // printf("\n Process parent %d\n", parent_node->key);
        cascading_cut(H, parent_node);
      }

      // printf("\n Finish Decrease Fix-up \n");
      if (node_to_be_decrease->key < H->min->key) {
        H->min = node_to_be_decrease;
      }
      // printf("\nH->min : %d\n", H->min->key);
      
    }
  }
  return ;
}

NODE* find_node(FIB_HEAP *H, NODE *n, int key, int new_key) {
  NODE *find_use = n;
  NODE *f = NULL;
  find_use->visited = true;
  if (find_use->key == key) {
    find_use->visited = false;
    f = find_use;
    return f;
  }
  if (find_use->child != NULL) {
    find_node(H, find_use->child, key, new_key);
  }
  if ((find_use->right_sibling->visited != true)) {
    find_node(H, find_use->right_sibling, key, new_key);
  }

  find_use->visited = false;
  
}

//iterative find
NODE* findNode(FIB_HEAP *H, NODE *n,int key, int value) {
    if (H== NULL) {
        return NULL; // Heap is empty
    }

    NODE* currentNode = n;
    do {
        if (currentNode->key == key && currentNode->value == value) {
            return currentNode; // Node found
        }

        // Recursively search in the child nodes if they exist
        NODE* child = currentNode->child;
        if (child != NULL) {
            NODE* foundNode = findNode(H, child, key, value);
            if (foundNode != NULL) {
                return foundNode; // Node found in child subtree
            }
        }

        currentNode = currentNode->right_sibling; // Move to the next node in the circular doubly linked list
    } while (currentNode != n);

    return NULL; // Node not found in the entire heap
}



FIB_HEAP *insertion_procedure() {
  FIB_HEAP *temp;
  int no_of_nodes, ele, i, val;
  NODE *new_node;
  temp = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
  temp = NULL;
  if (temp == NULL) {
    temp = make_fib_heap();
  }
  printf(" \n enter number of nodes to be insert = ");
  scanf("%d", &no_of_nodes);
  for (i = 1; i <= no_of_nodes; i++) {
    printf("\n node %d and its key, value = ", i);
    scanf("%d %d", &ele, &val);
    insertion(temp, new_node, ele,val);
  }
  return temp;
}
void Delete_Node(FIB_HEAP *H, int dec_key, int val) {
  NODE *p = NULL;
  NODE *heap = H->min;
  p = find_node(H, H->min, dec_key, val);
  if (p==NULL){ return;} 
  decrease_key(H, p, -50000);
  // printf("find");

  heap = extract_min(H);
  if (heap != NULL){
    // printf("\n --Node deleted--\n");
    return ;
  }else{
    // printf("\n Node not deleted:some error");
    return;
  }
}

int main(int argc, char **argv) {
    NODE *new_node, *min_node, *extracted_min, *node_to_be_decrease, *find_use;
    FIB_HEAP *heap, *h1, *h2;
    int new_key,del_key, dec_key, ele, i, no_of_nodes, val, decreaseVal;
    char operation[20];
    heap = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
    heap = NULL;
    heap = make_fib_heap();

    while (1) {
            if (scanf("%s", operation) == EOF || strcmp(operation, "quit") == 0) {
                break; // Exit the loop if "quit" operation is encountered or EOF
            }
            if (strcmp(operation, "insert") == 0) {
                scanf("%d %d", &new_key, &val);
                insertion(heap, new_node, new_key, val);
            } else if (strcmp(operation, "delete") == 0) {

                // printf("deletion called\n");
                scanf("%d %d", &del_key, &val);
                node_to_be_decrease = findNode(heap, heap->min, del_key, val);
                // printf("find node %d\n", node_to_be_decrease->key);
                if (node_to_be_decrease == NULL){ continue;}
                new_key = -5000;
                // printf("decrease called\n");
                decrease_key(heap, node_to_be_decrease, new_key);
                // printf("extracted_min called\n");
                // printf("decrease (%d)%d success\n", del_key, val);
                // print_heap(heap,heap->min);
                extracted_min = extract_min(heap);
                if (extracted_min == NULL){continue;}
                // printf("delete (%d)%d success\n", del_key, val);

            } else if (strcmp(operation, "decrease") == 0) {

                scanf("%d %d %d", &dec_key, &val, &decreaseVal);
                node_to_be_decrease = findNode(heap, heap->min, dec_key, val);
                if (node_to_be_decrease==NULL){ continue;} 
                // printf("find node %d\n", node_to_be_decrease->key);
                new_key = dec_key - decreaseVal;
                decrease_key(heap, node_to_be_decrease, new_key);

            } else if (strcmp(operation, "extract") == 0) {

                extracted_min = extract_min(heap);
                if (extracted_min == NULL){continue;}
                printf("(%d)%d\n", extracted_min->key, extracted_min->value);

            } else if (strcmp(operation, "print")==0){

                print_heap(heap,heap->min);

            } else if (strcmp(operation, "quit")==0){

                free(new_node);
                free(min_node);
                free(extracted_min);
                free(node_to_be_decrease);
                free(find_use);
                free(heap);
                exit(0);
            }
        }
    
}