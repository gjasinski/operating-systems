#ifndef BSTTREE_H
#define BSTTREE_H

#include "addressbook.h"

typedef struct bst_node_t{
  struct bst_node_t *parent;
  struct bst_node_t *left;
  struct bst_node_t *right;
  contact_t *data;
}bst_node_t;

typedef struct bst_tree_t{
  struct bst_node_t *tree;
  int order_by;
}bst_tree_t;

bst_node_t* create_bst_node(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
bst_tree_t* create_tree(int order_by);
void add_bst_node_recusive(bst_node_t* root, bst_node_t* node, int order_by);
void add_bst_node(bst_tree_t* tree, bst_node_t* node);
void add_contact_to_address_book_tree(bst_tree_t* tree, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone);
void remove_bst_tree(bst_node_t* tree);
void remove_address_book_tree(bst_tree_t* tree);
bst_node_t* find_bst_node(bst_node_t* node, char* query, int order_by);
bst_node_t* find_bst_tree(bst_tree_t* tree, char* query);
bst_node_t* bst_min(bst_node_t* proot);
bst_node_t* bst_successor(bst_node_t* proot);
void bst_remove_node(bst_node_t* proot);
void bst_find_and_remove(bst_tree_t* proot, char* query);
void reorder_addres_book_recursive(bst_tree_t* tree, bst_node_t* node);
bst_tree_t* reorder_addres_book(bst_tree_t* addressBook, int order_by);
void print_bst_node(bst_node_t* node);
void find_contact_bst_tree(bst_tree_t* tree, char* query);
void print_tree(bst_node_t* node);
#endif
