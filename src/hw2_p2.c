// Implementing Red-Black Tree in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum nodeColor {
  RED,
  BLACK
};

struct rbNode {
  int data, color;
  struct rbNode *link[2];
};

struct rbNode *root = NULL;

// Create a red-black tree
struct rbNode *createNode(int data) {
  struct rbNode *newnode;
  newnode = (struct rbNode *)malloc(sizeof(struct rbNode));
  newnode->data = data;
  newnode->color = RED;
  newnode->link[0] = newnode->link[1] = NULL;
  return newnode;
}

// Insert an node
void insertion(int data) {
  struct rbNode *stack[98], *ptr, *newnode, *xPtr, *yPtr;
  int dir[98], ht = 0, index;
  ptr = root;
  if (!root) {
    root = createNode(data);
    root->color = BLACK;
    return;
  }

  stack[ht] = root;
  dir[ht++] = 0;
  while (ptr != NULL) {
    // if (ptr->data == data) {
    //   printf("Duplicates Not Allowed!!\n");
    //   return;
    // }
    index = (data - ptr->data) >= 0 ? 1 : 0;
    stack[ht] = ptr;
    ptr = ptr->link[index];
    dir[ht++] = index;
  }
  stack[ht - 1]->link[index] = newnode = createNode(data);
  while ((ht >= 3) && (stack[ht - 1]->color == RED)) {
    if (dir[ht - 2] == 0) {
      yPtr = stack[ht - 2]->link[1];
      if (yPtr != NULL && yPtr->color == RED) {
        stack[ht - 2]->color = RED;
        stack[ht - 1]->color = yPtr->color = BLACK;
        ht = ht - 2;
      } else {
        if (dir[ht - 1] == 0) {
          yPtr = stack[ht - 1];
        } else {
          xPtr = stack[ht - 1];
          yPtr = xPtr->link[1];
          xPtr->link[1] = yPtr->link[0];
          yPtr->link[0] = xPtr;
          stack[ht - 2]->link[0] = yPtr;
        }
        xPtr = stack[ht - 2];
        xPtr->color = RED;
        yPtr->color = BLACK;
        xPtr->link[0] = yPtr->link[1];
        yPtr->link[1] = xPtr;
        if (xPtr == root) {
          root = yPtr;
        } else {
          stack[ht - 3]->link[dir[ht - 3]] = yPtr;
        }
        break;
      }
    } else {
      yPtr = stack[ht - 2]->link[0];
      if ((yPtr != NULL) && (yPtr->color == RED)) {
        stack[ht - 2]->color = RED;
        stack[ht - 1]->color = yPtr->color = BLACK;
        ht = ht - 2;
      } else {
        if (dir[ht - 1] == 1) {
          yPtr = stack[ht - 1];
        } else {
          xPtr = stack[ht - 1];
          yPtr = xPtr->link[0];
          xPtr->link[0] = yPtr->link[1];
          yPtr->link[1] = xPtr;
          stack[ht - 2]->link[1] = yPtr;
        }
        xPtr = stack[ht - 2];
        yPtr->color = BLACK;
        xPtr->color = RED;
        xPtr->link[1] = yPtr->link[0];
        yPtr->link[0] = xPtr;
        if (xPtr == root) {
          root = yPtr;
        } else {
          stack[ht - 3]->link[dir[ht - 3]] = yPtr;
        }
        break;
      }
    }
  }
  root->color = BLACK;
}

// Delete a node
void deletion(int data) {
  // Declare necessary variables and pointers
  struct rbNode *nodeStack[98], *currentNode, *replacementNode, *tempNode;
  struct rbNode *parentPtr, *siblingPtr, *nephewPtr;
  int directionStack[98], height = 0, difference, index;
  enum nodeColor nodeColor;

  // Check if the tree is empty
  if (!root) {
    printf("Tree is not available.\n");
    return;
  }

  // Start from the root and search for the node to be deleted()
  currentNode = root;
  while (currentNode != NULL) {
    if ((data - currentNode->data) == 0)
      break;
    difference = (data - currentNode->data) >= 0 ? 1 : 0;
    nodeStack[height] = currentNode;
    directionStack[height++] = difference;
    currentNode = currentNode->link[difference];
  }

  // If the node is not found, return
  if (currentNode == NULL) {
    return;
  }

  // Adjust the tree structure based on the node's children
  if (currentNode->link[1] == NULL) {
    // Case where the node has no right child
    if ((currentNode == root) && (currentNode->link[0] == NULL)) {
      // Node is the root and has no children
      free(currentNode);
      root = NULL;
    } else if (currentNode == root) {
      // Node is the root with a left child
      root = currentNode->link[0];
      free(currentNode);
    } else {
      // Node has no right child but has a left child or children
      nodeStack[height - 1]->link[directionStack[height - 1]] = currentNode->link[0];
    }
  } else {
    // Node to be deleted has a right child
    replacementNode = currentNode->link[1];
    if (replacementNode->link[0] == NULL) {
      // Case where the right child of the node to be deleted has no left child
      replacementNode->link[0] = currentNode->link[0];
      nodeColor = replacementNode->color;
      replacementNode->color = currentNode->color;
      currentNode->color = nodeColor;

      if (currentNode == root) {
        root = replacementNode;
      } else {
        nodeStack[height - 1]->link[directionStack[height - 1]] = replacementNode;
      }

      directionStack[height] = 1;
      nodeStack[height++] = replacementNode;
    } else {
      // Node to be deleted has a right child with a left child
      index = height++;
      while (1) {
        //找出右子樹的最小值
        directionStack[height] = 0;
        nodeStack[height++] = replacementNode;
        tempNode = replacementNode->link[0];
        if (!tempNode->link[0])
          break;
        replacementNode = tempNode;
      }

      directionStack[index] = 1;
      nodeStack[index] = tempNode;
      if (index > 0)
        nodeStack[index - 1]->link[directionStack[index - 1]] = tempNode;

      tempNode->link[0] = currentNode->link[0];

      replacementNode->link[0] = tempNode->link[1];
      tempNode->link[1] = currentNode->link[1];

      if (currentNode == root) {
        root = tempNode;
      }

      nodeColor = tempNode->color;
      tempNode->color = currentNode->color;
      currentNode->color = nodeColor;
    }
  }

  // Handle cases where the deleted node is black and might cause violations
  if (height < 1){
    return;
  }
  //代表replace是黑節點 需要調整 平衡
  if (currentNode->color == BLACK) {
    // Rebalance the tree to maintain red-black properties after deletion
    while (1) {
      parentPtr = nodeStack[height - 1]->link[directionStack[height - 1]];
      if (parentPtr && parentPtr->color == RED) {
        // Replacement node's parent is red, so no violation
        parentPtr->color = BLACK;
        break;
      }

      if (height < 2)
        break;

      if (directionStack[height - 2] == 0) {
        siblingPtr = nodeStack[height - 1]->link[1];

        if (!siblingPtr)
          break;

        if (siblingPtr->color == RED) {
          // Sibling is red, perform rotations and recoloring
          nodeStack[height - 1]->color = RED;
          siblingPtr->color = BLACK;
          nodeStack[height - 1]->link[1] = siblingPtr->link[0];
          siblingPtr->link[0] = nodeStack[height - 1];

          if (nodeStack[height - 1] == root) {
            root = siblingPtr;
          } else {
            nodeStack[height - 2]->link[directionStack[height - 2]] = siblingPtr;
          }
          directionStack[height] = 0;
          nodeStack[height] = nodeStack[height - 1];
          nodeStack[height - 1] = siblingPtr;
          height++;

          siblingPtr = nodeStack[height - 1]->link[1];
        }

        // Balance black nodes
        if ((!siblingPtr->link[0] || siblingPtr->link[0]->color == BLACK) &&
          (!siblingPtr->link[1] || siblingPtr->link[1]->color == BLACK)) {
          siblingPtr->color = RED;
        } else {
          if (!siblingPtr->link[1] || siblingPtr->link[1]->color == BLACK) {
            nephewPtr = siblingPtr->link[0];
            siblingPtr->color = RED;
            nephewPtr->color = BLACK;
            siblingPtr->link[0] = nephewPtr->link[1];
            nephewPtr->link[1] = siblingPtr;
            siblingPtr = nodeStack[height - 1]->link[1] = nephewPtr;
          }
          siblingPtr->color = nodeStack[height - 1]->color;
          nodeStack[height - 1]->color = BLACK;
          siblingPtr->link[1]->color = BLACK;
          nodeStack[height - 1]->link[1] = siblingPtr->link[0];
          siblingPtr->link[0] = nodeStack[height - 1];
          if (nodeStack[height - 1] == root) {
            root = siblingPtr;
          } else {
            nodeStack[height - 2]->link[directionStack[height - 2]] = siblingPtr;
          }
          break;
        }
      } else {
        siblingPtr = nodeStack[height - 1]->link[0];
        if (!siblingPtr)
          break;

        if (siblingPtr->color == RED) {
          // Sibling is red, perform rotations and recoloring
          nodeStack[height - 1]->color = RED;
          siblingPtr->color = BLACK;
          nodeStack[height - 1]->link[0] = siblingPtr->link[1];
          siblingPtr->link[1] = nodeStack[height - 1];

          if (nodeStack[height - 1] == root) {
            root = siblingPtr;
          } else {
            nodeStack[height - 2]->link[directionStack[height - 2]] = siblingPtr;
          }
          directionStack[height] = 1;
          nodeStack[height] = nodeStack[height - 1];
          nodeStack[height - 1] = siblingPtr;
          height++;

          siblingPtr = nodeStack[height - 1]->link[0];
        }
        if ((!siblingPtr->link[0] || siblingPtr->link[0]->color == BLACK) &&
          (!siblingPtr->link[1] || siblingPtr->link[1]->color == BLACK)) {
          siblingPtr->color = RED;
        } else {
          if (!siblingPtr->link[0] || siblingPtr->link[0]->color == BLACK) {
            nephewPtr = siblingPtr->link[1];
            siblingPtr->color = RED;
            nephewPtr->color = BLACK;
            siblingPtr->link[1] = nephewPtr->link[0];
            nephewPtr->link[0] = siblingPtr;
            siblingPtr = nodeStack[height - 1]->link[0] = nephewPtr;
          }
          siblingPtr->color = nodeStack[height - 1]->color;
          nodeStack[height - 1]->color = BLACK;
          siblingPtr->link[0]->color = BLACK;
          nodeStack[height - 1]->link[0] = siblingPtr->link[1];
          siblingPtr->link[1] = nodeStack[height - 1];
          if (nodeStack[height - 1] == root) {
            root = siblingPtr;
          } else {
            nodeStack[height - 2]->link[directionStack[height - 2]] = siblingPtr;
          }
          break;
        }
      }
      height--;
    }  
  }else{
    //換紅不影響 
  }
}


// Print the inorder traversal of the tree
void inorderTraversal(struct rbNode *node) {
    if (node) {
    inorderTraversal(node->link[0]);
    if (node->color == RED){
        printf("%d(RED) ", node->data);
    }else{
        printf("%d(BLACK) ", node->data);
    }
    inorderTraversal(node->link[1]);
    }
    return;
}

void searchNode(struct rbNode *node, int val,bool* found)
{
    if (node) {
        searchNode(node->link[0],val, found);
        if (node->data == val){
            if (node->color == RED){
                printf("red\n");
            }else{
                printf("black\n");
            }
            *found = true; 
            return ;
        }
        searchNode(node->link[1],val, found);
    }
    return;
};

void binarySearchNode(struct rbNode *node, int val) {
    while (node != NULL) {
        if (node->data == val) {
            if (node->color == RED) {
                printf("red\n");
            } else {
                printf("black\n");
            }
            return;
        }

        // Move to the left or right child based on the comparison
        if (val < node->data) {
            node = node->link[0]; // Go left
        } else {
            node = node->link[1]; // Go right
        }
    }

    // Value not found in the tree
    printf("Not found\n");
    
}
// Driver code


int main() {
    char operation[10];
    int value;
    bool *found;
    while (1) {
        if (scanf("%s", operation) == EOF || strcmp(operation, "quit") == 0) {
            break; // Exit the loop if "quit" operation is encountered or EOF
        }

        if (strcmp(operation, "insert") == 0) {
            scanf("%d", &value);
            insertion(value);
        }else if (strcmp(operation, "delete") == 0) {
            scanf("%d", &value);
            deletion(value);
        }else if (strcmp(operation, "search") == 0) {
            found = false; 
            scanf("%d", &value);
            binarySearchNode(root, value);
        }else if (strcmp(operation, "print") == 0) {
            inorderTraversal(root);
            printf("\n");
        } else if (strcmp(operation, "quit") == 0) {
            break;
        }
    }
  
    return 0;
}