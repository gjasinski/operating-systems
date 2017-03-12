#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

int main(void) {
  List* ad = createList();
  Node* tmp = createNode("aaa", "ddd", "6", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  tmp = createNode("aaa", "e", "8", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  tmp = createNode("aaa", "da", "10", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  tmp = createNode("aaa", "bb", "5", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  tmp = createNode("aaa", "bc", "3", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  tmp = createNode("aaa", "db", "7", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  printList(ad->first->next);printf("\n");
  ad = insertionSort(ad, 1);
  printList(ad->first->next);
  ad = insertionSort(ad, 2  );printf("\n");
  printList(ad->first->next);

  printf("\n");
  printf("---------------\n");
  findAndRemove(ad, "ddd", 1);
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "10", 2);
  printList(ad->first->next);printf("\n");
  tmp = createNode("aaaeeeeeeeeeeeeeeeee", "db1", "17", "aaa", "aaa", "aaa");
  addNode(ad, tmp);
  ad = insertionSort(ad, 1  );
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "7", 2);
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "8", 2);
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "3", 2);
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "17", 2);
  printList(ad->first->next);printf("\n");
  findAndRemove(ad, "5", 2);
  printList(ad->first->next);printf("\n");
  tmp = createNode("aaaeeeeeeeeeeeeeeeee", "db1", "17", "aaa", "aaa", "aaa");
  printList(ad->first->next);printf("\n");
  return 0;
}

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

List* insertionSort(List* list, int orderby){
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
  removeNode(node);
}
void printBstNode(Node* node){
  for(int i = 0; i < 6; i++){
    printf("%s ", node->data[i]);
  }
  printf("\n");
}

void printList(Node* node){
  if(node->next){
    printBstNode(node);
    printList(node->next);
  }
}
