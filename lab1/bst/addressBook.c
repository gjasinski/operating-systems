#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstTree.h"

BstNode* createBstNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  BstNode* node = (BstNode*)calloc(1, sizeof(BstNode));
  node->data[0] = fname;
  node->data[1] = lname;
  node->data[2] = bdate;
  node->data[3] = email;
  node->data[4] = address;
  node->data[5] = telephone;
  node->parent = node->left = node->right = NULL;
  return node;
}

BstTree* createTree(int orderby){
  if(orderby == 1 || orderby == 2 || orderby == 3 || orderby == 5){
    BstTree* tree = (BstTree*)calloc(1, sizeof(BstTree));
    tree->tree = NULL;
    tree->orderedby = orderby;
    return tree;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

void addBstNode(BstTree* tree, BstNode* node){
  if(tree->tree == NULL){
    tree->tree = node;
  }
  else{
    addBstNodeRecusive(tree->tree, node, tree->orderedby);
  }
}

void addBstNodeRecusive(BstNode* root, BstNode* node, int orderedby){
  if(strcmp(root->data[orderedby], node->data[orderedby]) > 0){
    if(root->left == NULL){
      node->parent = root;
      root->left = node;
    }
    else{
      addBstNodeRecusive(root->left, node, orderedby);
    }
  }
  else{
    if(root->right == NULL){
      node->parent = root;
      root->right = node;
    }
    else{
      addBstNodeRecusive(root->right, node, orderedby);
    }
  }
}

void removeBstTree(BstNode* tree){
  if(tree != NULL){
    removeBstTree(tree->left);
    removeBstTree(tree->right);
    free(tree);
  }
}

void removeAddressBookTree(BstTree* tree){
  removeBstTree(tree->tree);
  free(tree);
}

BstNode* findContactBstTree(BstTree* tree, char* query){
  return findBstNode(tree->tree, query, tree->orderedby);
}

BstNode* findBstNode(BstNode* node, char* query, int orderedby){
  if(node == NULL) return NULL;
  if(strcmp(node->data[orderedby], query) == 0) return node;
  if(strcmp(node->data[orderedby], query) > 0){
    return findBstNode(node->left, query, orderedby);
  }
  else{
    return findBstNode(node->right, query, orderedby);
  }
}

BstNode* bstSuccessor(BstNode* proot){
    if(proot->right) return bstMin(proot->right);
    while(proot->parent && proot->parent->right == proot){
        proot=proot->parent;
    }
    return proot->parent;
}

BstNode* bstMin(BstNode* proot){
    if(!proot) return NULL;
    while(proot->left){
        proot = proot->left;
    }
    return proot;
}

void bstFindAndRemove(BstTree* proot, char* query){
    BstNode* ptr = findBstNode(proot->tree, query, proot->orderedby);

    if(ptr == NULL) return;
    if(ptr == proot->tree){
        BstNode* guard = (BstNode*)calloc(1, sizeof(BstNode));
        guard->left = proot->tree;
        guard->right = guard->parent = NULL;
        ptr->parent = guard;
        bstRemoveNode(ptr);
        proot->tree = guard->left;
        if(proot->tree) proot->tree->parent = NULL;
        free(guard);
    }
    else bstRemoveNode(ptr);
}

void bstRemoveNode(BstNode* proot){
    BstNode* ptr;
    if(!proot->left && !proot->right) ptr = NULL;
    else{
      if(!proot->left) ptr = proot->right;
      if(!proot->right) ptr = proot->left;
      if(proot->left && proot->right){
        ptr = bstSuccessor(proot);
        if(ptr->parent->left == ptr) ptr->parent->left = ptr->right;
        else ptr->parent->right = ptr->right;
        ptr->left = proot->left;
        ptr->right = proot->right;
      }
      if(ptr->left) ptr->left->parent = ptr;
      if(ptr->right) ptr->right->parent = ptr;
    }
    if(ptr) ptr->parent = proot->parent;
    if(proot->parent->left == proot) proot->parent->left = ptr;
        else proot->parent->right = ptr;
    free(proot);
}


BstTree* reorderAddresBook(BstTree* addressBook, int orderby){
  if(orderby == 1 || orderby == 2 || orderby == 3 || orderby == 5){
    BstTree* newAddressBook = createTree(orderby);
    reorderAddresBookRecursive(newAddressBook, addressBook->tree);
    free(addressBook);
    return newAddressBook;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

void reorderAddresBookRecursive(BstTree* tree, BstNode* node){
  if(node){
    reorderAddresBookRecursive(tree, node->left);
    reorderAddresBookRecursive(tree, node->right);
    node-> left = node->right = node->parent = NULL;
    addBstNode(tree, node);
  }
}


void printBstNode(BstNode* node){
  for(int i = 0; i < 6; i++){
    printf("%s ", node->data[i]);
  }
  printf("\n");
}

void printTree(BstNode* node){
  if(node){
    printTree(node->left);
    printBstNode(node);
    printTree(node->right);
  }
}
