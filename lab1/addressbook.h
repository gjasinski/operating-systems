#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct BstNode{
  struct BstNode *parent;
  struct BstNode *left;
  struct BstNode *right;
  char *data[6];
}BstNode;

typedef struct BstTree{
  struct BstNode *tree;
  int orderedby;
}BstTree;

typedef struct Node{
  struct Node* next;
  struct Node* prev;
  char *data[6];
}Node;

typedef struct List{
  struct Node* first;
  struct Node* last;
}List;

BstNode* createBstNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
BstTree* createTree(int orderby);
void addBstNodeRecusive(BstNode* root, BstNode* node, int orderedby);
void addBstNode(BstTree* tree, BstNode* node);
void addContactToAddressBookTree(BstTree* tree, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void removeBstTree(BstNode* tree);
void removeAddressBookTree(BstTree* tree);
BstNode* findBstNode(BstNode* node, char* query, int orderedby);
BstNode* findBstTree(BstTree* tree, char* query);
BstNode* bstMin(BstNode* proot);
BstNode* bstSuccessor(BstNode* proot);
void bstRemoveNode(BstNode* proot);
void bstFindAndRemove(BstTree* proot, char* query);
void reorderAddresBookRecursive(BstTree* tree, BstNode* node);
BstTree* reorderAddresBook(BstTree* addressBook, int orderby);
void printBstNode(BstNode* node);
void findContactBstTree(BstTree* tree, char* query);
void printTree(BstNode* node);
//--------------------------
List* createList();
Node* createNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void addNode(List* list, Node* node);
void addContactToAddressBookList(List* list, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void removeNode(Node* node);
List* removeList(List* list);
Node* findContactList(List* list, char* query, int searchby);
Node* findMin(Node* node, int searchby);
List* sortListBy(List* list, int orderby);
void findAndRemove(List* list, char* query, int searchby);
void printNode(Node* node);
void printList(Node* node);
void findAndPrint(List* list, char* query, int searchby);
#endif
