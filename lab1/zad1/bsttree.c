#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsttree.h"
//#include "addressbook.c"

bst_node_t* create_bst_node(char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  bst_node_t* node = (bst_node_t*)calloc(1, sizeof(bst_node_t));
  node->parent = node->left = node->right = NULL;
  node->data = create_contact(fname, lname, bdate, email, address, telephone);
  return node;
}

bst_tree_t* create_tree(int order_by){
  if(order_by == 1 || order_by == 2 || order_by == 3 || order_by == 5){
    bst_tree_t* tree = (bst_tree_t*)calloc(1, sizeof(bst_tree_t));
    tree->tree = NULL;
    tree->order_by = order_by;
    return tree;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

void add_bst_node_recusive(bst_node_t* root, bst_node_t* node, int order_by){
  if(strcmp(root->data->data[order_by], node->data->data[order_by]) > 0){
    if(root->left == NULL){
      node->parent = root;
      root->left = node;
    }
    else{
      add_bst_node_recusive(root->left, node, order_by);
    }
  }
  else{
    if(root->right == NULL){
      node->parent = root;
      root->right = node;
    }
    else{
      add_bst_node_recusive(root->right, node, order_by);
    }
  }
}

void add_bst_node(bst_tree_t* tree, bst_node_t* node){
  if(tree->tree == NULL){
    tree->tree = node;
  }
  else{
    add_bst_node_recusive(tree->tree, node, tree->order_by);
  }
}

void add_contact_to_address_book_tree(bst_tree_t* tree, char* fname, char* lname, char* bdate, char* email, char* address, char* telephone){
  if(tree) add_bst_node(tree, create_bst_node(fname, lname, bdate, email, address, telephone));
}

void remove_bst_tree(bst_node_t* tree){
  if(tree != NULL){
    remove_bst_tree(tree->left);
    remove_bst_tree(tree->right);
    delete_contact(tree->data);
    free(tree);
  }
}

void remove_address_book_tree(bst_tree_t* tree){
  remove_bst_tree(tree->tree);
  free(tree);
}

bst_node_t* find_bst_node(bst_node_t* node, char* query, int order_by){
  if(node == NULL) return NULL;
  if(strcmp(node->data->data[order_by], query) == 0) return node;
  if(strcmp(node->data->data[order_by], query) > 0){
    return find_bst_node(node->left, query, order_by);
  }
  else{
    return find_bst_node(node->right, query, order_by);
  }
}

bst_node_t* find_bst_tree(bst_tree_t* tree, char* query){
  return find_bst_node(tree->tree, query, tree->order_by);
}

bst_node_t* bst_min(bst_node_t* proot){
    if(!proot) return NULL;
    while(proot->left){
        proot = proot->left;
    }
    return proot;
}

bst_node_t* bst_successor(bst_node_t* proot){
    if(proot->right) return bst_min(proot->right);
    while(proot->parent && proot->parent->right == proot){
        proot = proot->parent;
    }
    return proot->parent;
}

void bst_remove_node(bst_node_t* proot){
    bst_node_t* ptr;
    if(!proot->left && !proot->right) ptr = NULL;
    else{
      if(!proot->left) ptr = proot->right;
      if(!proot->right) ptr = proot->left;
      if(proot->left && proot->right){
        ptr = bst_successor(proot);
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
    delete_contact(proot->data);
    free(proot);
}

void bst_find_and_remove(bst_tree_t* proot, char* query){
    bst_node_t* ptr = find_bst_node(proot->tree, query, proot->order_by);

    if(ptr == NULL) return;
    if(ptr == proot->tree){
        bst_node_t* guard = (bst_node_t*)calloc(1, sizeof(bst_node_t));
        guard->left = proot->tree;
        guard->right = guard->parent = NULL;
        ptr->parent = guard;
        bst_remove_node(ptr);
        proot->tree = guard->left;
        if(proot->tree) proot->tree->parent = NULL;
        free(guard);
    }
    else bst_remove_node(ptr);
}

void reorder_addres_book_recursive(bst_tree_t* tree, bst_node_t* node){
  if(node){
    reorder_addres_book_recursive(tree, node->left);
    reorder_addres_book_recursive(tree, node->right);
    node-> left = node->right = node->parent = NULL;
    add_bst_node(tree, node);
  }
}

bst_tree_t* reorder_addres_book(bst_tree_t* addressBook, int order_by){
  if(order_by == 1 || order_by == 2 || order_by == 3 || order_by == 5){
    bst_tree_t* newAddressBook = create_tree(order_by);
    reorder_addres_book_recursive(newAddressBook, addressBook->tree);
    free(addressBook);
    return newAddressBook;
  }
  else{
    printf("Wrong argument. You can use 1, 2, 3, 5.\n");
    return NULL;
  }
}

void print_bst_node(bst_node_t* node){
  print_contact(node->data);
}

void find_contact_bst_tree(bst_tree_t* tree, char* query){
  if(tree){
    print_bst_node(find_bst_tree(tree, query));
  }
}
void print_tree(bst_node_t* node){
  if(node){
    print_tree(node->left);
    print_bst_node(node);
    print_tree(node->right);
  }
}
