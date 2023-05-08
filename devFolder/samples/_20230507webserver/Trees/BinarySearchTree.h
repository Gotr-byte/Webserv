#ifndef BinarySearchTree_h
#define BinarySearchTree_h
#include "../Common/Node.h"
#include <algorithm>



struct BinarySearchTree
{
    struct Node *head;
    int (*compare)(void *data_one, void *data_two);
    void *(*search)(struct BinarySearchTree *tree, void* data);
    void  (*insert)(struct BinarySearchTree *tree, void* data, int size);
};

struct BinarySearchTree binary_search_tree_constructor(int (*compare)(void *data_one, void *data_two));    
#endif