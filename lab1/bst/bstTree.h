#ifndef BSTTREE_H
#define BSTTREE_H

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

BstNode* createBstNode(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
BstTree* createTree(int orderby);
void addBstNode(BstTree* tree, BstNode* node);
void addBstNodeRecusive(BstNode* root, BstNode* node, int orderedby);
void printBstNode(BstNode* node);
void removeBstTree(BstNode* tree);
void removeAddressBookTree(BstTree* tree);
BstNode* findContactBstTree(BstTree* tree, char* query);
BstNode* findBstNode(BstNode* node, char* query, int orderedby);
BstNode* bstSuccessor(BstNode* proot);
BstNode* bstMin(BstNode* proot);
void bstFindAndRemove(BstTree* proot, char* query);
void bstRemoveNode(BstNode* proot);
BstTree* reorderAddresBook(BstTree* addressBook, int orderby);
void printTree(BstNode* node);
void reorderAddresBookRecursive(BstTree* tree, BstNode* node);
#endif
