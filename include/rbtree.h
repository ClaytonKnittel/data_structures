#pragma once


#define DOUBLE_BLACK 3
#define BLACK 1
#define RED 0

struct rbTreeNode {
    static rbTreeNode leaf;
    rbTreeNode *left, *right, *parent;
    int val;
    bool color;
};

#define parent(node) ((node)->parent)
#define grandparent(node) ((node)->parent->parent)
#define luncle(node) (grandparent(node)->left)
#define runcle(node) (grandparent(node)->right)


typedef rbTreeNode* rbtree;

#define root(tree) ((tree)->left)


rbtree rb_create();

void rb_delete(rbtree);

bool rb_insert(rbtree, int);

void rb_remove(rbtree, int);

bool rb_find(rbtree, int);

void rb_print(rbtree);

int rb_validate(rbtree);

int rb_size(rbtree);
