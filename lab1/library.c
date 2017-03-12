#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "library.h"

int main(void){
  return 0;
}

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

void addBstNode(BstTree* tree, BstNode* node){
  if(tree->tree == NULL){
    tree->tree = node;
  }
  else{
    addBstNodeRecusive(tree->tree, node, tree->orderedby);
  }
}

void addContactToAddressBookTree(BstTree* tree, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  if(tree) addBstNode(tree, createBstNode(fname, lname, bdate, email, address, telephone));
}

void removeBstTree(BstNode* tree){
  if(tree != NULL){
    removeBstTree(tree->left);
    removeBstTree(tree->right);
    for(int i = 0; i < 6; i++){
      free(tree->data[i]);
    }
    free(tree);
  }
}

void removeAddressBookTree(BstTree* tree){
  removeBstTree(tree->tree);
  free(tree);
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

BstNode* findBstTree(BstTree* tree, char* query){
  return findBstNode(tree->tree, query, tree->orderedby);
}

BstNode* bstMin(BstNode* proot){
    if(!proot) return NULL;
    while(proot->left){
        proot = proot->left;
    }
    return proot;
}

BstNode* bstSuccessor(BstNode* proot){
    if(proot->right) return bstMin(proot->right);
    while(proot->parent && proot->parent->right == proot){
        proot=proot->parent;
    }
    return proot->parent;
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

void reorderAddresBookRecursive(BstTree* tree, BstNode* node){
  if(node){
    reorderAddresBookRecursive(tree, node->left);
    reorderAddresBookRecursive(tree, node->right);
    node-> left = node->right = node->parent = NULL;
    addBstNode(tree, node);
  }
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

void printBstNode(BstNode* node){
  for(int i = 0; i < 6; i++){
    printf("%s ", node->data[i]);
  }
  printf("\n");
}

void findContactBstTree(BstTree* tree, char* query){
  if(tree){
    printBstNode(findBstTree(tree, query));
  }
}
void printTree(BstNode* node){
  if(node){
    printTree(node->left);
    printBstNode(node);
    printTree(node->right);
  }
}


//--------------------------
List* createList(){
  List* list = (List*)calloc(1, sizeof(List));
  Node* node = (Node*)calloc(1, sizeof(Node));
  list->first = node;
  node = (Node*)calloc(1, sizeof(Node));
  list->last = node;

  list->first->next = list->last;
  list->last->prev = list->first;
  list->first->prev = list->last->next = NULL;
  return list;
}

Node* createNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  Node* node = (Node*)calloc(1, sizeof(Node));
  node->data[0] = fname;
  node->data[1] = lname;
  node->data[2] = bdate;
  node->data[3] = email;
  node->data[4] = address;
  node->data[5] = telephone;
  node->next = node->prev = NULL;
  return node;
}

void addNode(List* list, Node* node){
  node->next = list->last;
  node->prev = list->last->prev;
  list->last->prev->next = node;
  list->last->prev = node;
}

void addContactToAddressBookList(List* list, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
    if(list) addNode(list, createNode(fname, lname, bdate, email, address, telephone));
}

void removeNode(Node* node){
  if(node->next) node->prev->next = node->next;
  if(node->next) node->next->prev = node->prev;
  free(node);
}

List* removeList(List* list){
  if(list != NULL){
    Node* next;
    Node* current = list->first;
    while(current){
      next = current->next;
      free(current);
      current = next;
    }
    free(list);
  }
  return NULL;
}

Node* findContactList(List* list, char* query, int searchby){
  if(searchby == 1 || searchby == 2 || searchby == 3 || searchby == 5){
    Node* node = list->first->next;
    while(node->next){
      if(strcmp(node->data[searchby], query) == 0) return node;
      node = node->next;
    }
    return NULL;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

Node* findMin(Node* node, int searchby){
  Node* result;
  if(node->next){
    result = node;
  }
  else{
    node = NULL;
  }
  while(node->next){
    if(strcmp(node->data[searchby], result->data[searchby]) < 0){
      result = node;
    }
    node = node->next;
  }
  return result;
}

List* sortListBy(List* list, int orderby){
  if(orderby == 1 || orderby == 2 || orderby == 3 || orderby == 5){
    List* sorted = createList();
    while(list->first->next != list->last){
      Node* node = findMin(list->first->next, orderby);
      node->prev->next = node->next;
      node->next->prev = node->prev;
      addNode(sorted, node);
    }
    free(list->first);
    free(list->last);
    free(list);
    return(sorted);
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}


void findAndRemove(List* list, char* query, int searchby){
  Node* node = findContactList(list, query, searchby);
  if(node) removeNode(node);
}

void printNode(Node* node){
  for(int i = 0; i < 6; i++){
    printf("%s ", node->data[i]);
  }
  printf("\n");
}

void printList(Node* node){
  if(node->next){
    printNode(node);
    printList(node->next);
  }
}

void findAndPrint(List* list, char* query, int searchby){
  Node* node = findContactList(list, query, searchby);
  if(node) printNode(node);
}
