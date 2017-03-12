#ifndef LIST_H
#define LIST_H

typedef struct Node{
  struct Node* next;
  struct Node* prev;
  char *data[6];
}Node;

typedef struct List{
  struct Node* first;
  struct Node* last;
}List;

List* createList();
Node* createNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void addNode(List* list, Node* node);
void removeNode(Node* node);
List* removeList(List* list);
Node* findContactList(List* list, char* query, int searchby);
Node* findMin(Node* node, int searchby);
List* insertionSort(List* list, int orderby);
void printBstNode(Node* node);
void printList(Node* node);
void findAndRemove(List* list, char* query, int searchby);
#endif
