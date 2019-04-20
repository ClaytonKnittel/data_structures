
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <iomanip>
#include <string>
#include <sstream>

#include "rbtree.h"


#define VERSION 1

#define LEAF (&rbTreeNode::leaf)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

rbTreeNode rbTreeNode::leaf = {NULL, NULL, NULL, 0, BLACK};

rbtree rb_create() {
    // only has a left child, which is the real root of the tree
    rbtree tree = (rbtree) malloc(sizeof(rbTreeNode));
    tree->left = LEAF;
    tree->right = NULL;
    return tree;
}

void rb_delete_(rbTreeNode *node) {
    if (node != LEAF) {
        rb_delete_(node->left);
        rb_delete_(node->right);
        free(node);
    }
}

void rb_delete(rbtree tree) {
    rbTreeNode *root = root(tree);
    rb_delete_(root);
    free(tree);
}

inline void setleft(rbTreeNode *p, rbTreeNode *c) {
    p->left = c;
    if (c != LEAF)
        c->parent = p;
}

inline void setright(rbTreeNode *p, rbTreeNode *c) {
    p->right = c;
    if (c != LEAF)
        c->parent = p;
}


// sets node to be the new child of the parent of par (gp)
#define set_parent_pointer(node, par, gp) \
    (node)->parent = gp;        \
    if ((gp)->left == (par))    \
        (gp)->left = (node);    \
    else                        \
        (gp)->right = (node);

/*
 * -------------------------- helper methods --------------------------
 */


// right rotate about p, with r = p->right
#define rb_rotate_right(p, r) \
    {rbTreeNode *__g_p_ = p->parent;    \
    set_parent_pointer(r, p, __g_p_);   \
    setleft(p, r->right);           \
    r->right = p;                   \
    p->parent = r;}


// left rotate about p, with l = p->left
#define rb_rotate_left(p, l) \
    {rbTreeNode *__g_p_ = p->parent;    \
    set_parent_pointer(l, p, __g_p_);   \
    setright(p, l->left);           \
    l->left = p;                    \
    p->parent = l;}


// left-right rotate about p, with
// l = p->left, lr = l->right
#define rb_rotate_left_right(p, l, lr) \
    {rbTreeNode *__gg_p_ = p->parent;   \
    set_parent_pointer(lr, p, __gg_p_); \
    setright(l, lr->left);              \
    setleft(p, lr->right);              \
    lr->left = l;                       \
    l->parent = lr;                     \
    lr->right = p;                      \
    p->parent = lr;}


// right-left rotate about p, with
// r = p->right, rl = l->left
#define rb_rotate_right_left(p, r, rl) \
    {rbTreeNode *__gg_p_ = p->parent;   \
    set_parent_pointer(rl, p, __gg_p_); \
    setleft(r, rl->right);              \
    setright(p, rl->left);              \
    rl->right = r;                      \
    r->parent = rl;                     \
    rl->left = p;                       \
    p->parent = rl;}


/* 
 * -------------------------- rb_insert --------------------------
 */

inline void rb_insert_fix(rbtree tree, rbTreeNode* node) {
    rbTreeNode* p, *gp, *u;
    while (node != root(tree) and (p = parent(node))->color == RED) {
        // printf("On node: %d%c\n", node->val, node->color == RED ? 'r' : 'b');
        if (p == (gp = p->parent)->left) {
            u = gp->right;
            if (u->color == BLACK) {
                // rotate
                if (p->right == node) {
                    rb_rotate_left_right(gp, p, node);
                    gp->color = RED;
                    node->color = BLACK;
                    node = p; // so the loop terminates
                }
                else {
                    rb_rotate_right(gp, p);
                    gp->color = RED;
                    p->color = BLACK;
                }
            }
            else {
                // recolor
                gp->color = RED;
                p->color = BLACK;
                u->color = BLACK;
                node = gp; // so the loop continues
            }
        }
        else {
            u = gp->left;
            if (u->color == BLACK) {
                // rotate
                if (p->left == node) {
                    rb_rotate_right_left(gp, p, node);
                    gp->color = RED;
                    node->color = BLACK;
                    node = p; // so the loop terminates
                }
                else {
                    rb_rotate_left(gp, p);
                    gp->color = RED;
                    p->color = BLACK;
                }
            }
            else {
                // recolor
                gp->color = RED;
                p->color = BLACK;
                u->color = BLACK;
                node = gp; // so the loop continues
            }
        }
    }
    if (node == root(tree))
        node->color = BLACK;
}

bool rb_insert(rbtree tree, int el) {
    rbTreeNode *newNode;
    rbTreeNode *node = root(tree);
    rbTreeNode *prevNode = tree;

    while (node != LEAF) {
        prevNode = node;
        node = node->val <= el ? node->right : node->left;
    }
    if (node != root(tree)) {
        newNode = (rbTreeNode*) malloc(sizeof(rbTreeNode));
        newNode->parent = prevNode;
        if (prevNode->val <= el)
            prevNode->right = newNode;
        else
            prevNode->left = newNode;
        newNode->color = RED;
        newNode->left = newNode->right = LEAF;
        rb_insert_fix(tree, newNode);
    }
    else {
        newNode = root(tree) = (rbTreeNode*) malloc(sizeof(rbTreeNode));
        newNode->color = BLACK;
        newNode->left = newNode->right = LEAF;
        newNode->parent = tree;
    }
    newNode->val = el;

    return 1;
}


/* 
 * -------------------------- rb_remove --------------------------
 */


inline void rb_remove_fix(rbtree tree, rbTreeNode *node) {
    rbTreeNode *p, *s, *gp, *slr;

    while (node->color == BLACK && node != root(tree)) {
        p = node->parent;
        if (node == p->left) {
            s = p->right;
            if (s->color == RED) {
                rb_rotate_left(p, s);
                p->color = RED;
                s->color = BLACK;

                s = p->right;
            }
            // now s is guarenteed to be black
            if (s->left->color == BLACK and s->right->color == BLACK) {
                s->color = RED;
                // if p is already red, loop terminates and p is set
                // to black. Else, p was black and is now double black,
                // so we need to propogate the fix up through with p
                node = p;
            } else {
                if (s->left->color == RED) {
                    // right-left rotate about p, making
                    // s->left (slr) the new root
                    slr = s->left;
                    rb_rotate_right_left(p, s, slr);

                    slr->color = p->color;
                    p->color = BLACK;
                    s->color = BLACK;

                    node = root(tree); // so the loop terminates
                } else {
                    // left rotate about p
                    rb_rotate_left(p, s);

                    s->color = p->color;
                    p->color = BLACK;
                    s->right->color = BLACK;

                    node = root(tree);
                }
            }
        } else {
            s = p->left;
            if (s->color == RED) {
                // right rotate about p
                rb_rotate_right(p, s);
                p->color = RED;
                s->color = BLACK;

                s = p->left;
            }
            // now s is guarenteed to be black
            if (s->right->color == BLACK and s->left->color == BLACK) {
                s->color = RED;
                // if p is already red, loop terminates and p is set
                // to black. Else, p was black and is now double black,
                // so we need to propogate the fix up through with p
                node = p;
            } else {
                if (s->right->color == RED) {
                    // left-right rotate about p, making
                    // s->right (slr) the new root
                    slr = s->right;
                    rb_rotate_left_right(p, s, slr);

                    slr->color = p->color;
                    p->color = BLACK;
                    s->color = BLACK;

                    node = root(tree); // so the loop terminates
                } else {
                    // right rotate about p
                    rb_rotate_right(p, s);

                    s->color = p->color;
                    p->color = BLACK;
                    s->left->color = BLACK;
                    
                    node = root(tree);
                }
            }
        }
    }
    node->color = BLACK;
}


void rb_remove(rbtree tree, int el) {
    rbTreeNode *node;
    rbTreeNode *replacer;
    rbTreeNode *child;

    node = root(tree);
    while (node != LEAF and node->val != el) {
        node = node->val <= el ? node->right : node->left;
    }

    // element was not found
    if (node == LEAF)
        return;
    
    if (node->left == LEAF || node->right == LEAF)
        replacer = node;
    else {
        // find successor of node
        replacer = node->right;
        if (replacer != LEAF) {
            while (replacer->left != LEAF)
                replacer = replacer->left;
        }
    }

    child = (replacer->left == LEAF ? replacer->right : replacer->left);
    if (replacer->parent->left == replacer)
        replacer->parent->left = child;
    else
        replacer->parent->right = child;
    child->parent = replacer->parent;
    
    // proceed to remove replacer from the graph, which
    // is guarenteed to have at most 1 non-leaf child
    node->val = replacer->val; // "swap"
    if (replacer->color != RED) {
        rb_remove_fix(tree, child);
    }

    free(replacer);
}


/* 
 * -------------------------- rb_find --------------------------
 */


bool rb_find(rbtree tree, int el) {
    rbTreeNode* node;

    node = root(tree);
    while (node != LEAF and node->val != el) {
        node = node->val <= el ? node->right : node->left;
    }
    return node != LEAF;
}






/* 
 * -------------------------- rb_print --------------------------
 */

size_t height(rbTreeNode *node) {
    size_t l, r;
    if (node == LEAF)
        return 0;
    l = height(node->left);
    r = height(node->right);
    return MAX(l, r) + 1;
}

#define WID 6

#define DISPLAY_LEAVES 0

void rb_print(rbtree tree) {
    size_t h = height(root(tree));
    const size_t hwid = WID / 2;
    bool drawable_els = 1;
    rbtree node;
    std::queue<rbtree> *q = new std::queue<rbtree>();
    std::queue<rbtree> *qp = new std::queue<rbtree>(); // for swapping

    q->push(root(tree));


#if DISPLAY_LEAVES
    size_t sep = WID << h;
#else
    size_t sep = WID << (h - 1);
#endif

    while (drawable_els) {
        drawable_els = 0;
        std::cout << std::setw((sep + 1) / 2 - hwid) << "";
        do {
            node = q->front();
            q->pop();
            if (node != NULL) {
                qp->push(node->left);
                qp->push(node->right);
                if (node != LEAF and (node->left != LEAF or node->right != LEAF))
                    drawable_els = 1;
                if (node != LEAF) {
                    std::ostringstream os;
                    os << node->val;
                    os << (node->color == RED ? "r" : "b");
                    std::cout << os.str().c_str();
                }
                else {
#if DISPLAY_LEAVES
                    std::cout << "l";
                    drawable_els = 1;
#else
                    std::cout << " ";
#endif
                }
            } else {
                std::cout << " ";
                qp->push(NULL);
                qp->push(NULL);
            }
            std::cout << std::setw(sep);
        } while (!q->empty());
        std::cout << "\n";
        sep >>= 1;
        std::swap(q, qp);
    }
    std::cout << std::flush;
    
    // 7/2 WID 
    // 3/2 WID
    // WID 
    // WID/2 spaces + setw(WID * 2)
    // setw(WID)
}



/* 
 * -------------------------- rb_validate --------------------------
 */


bool bst_valid(rbTreeNode *node) {
    if (node == LEAF)
        return 1;
    if (!bst_valid(node->left) or !bst_valid(node->left))
        return false;
    bool valid = 1;
    if (node->left != LEAF)
        valid = valid && (node->left->val <= node->val);
    if (node->right != LEAF)
        valid = valid && (node->val <= node->right->val);
    return valid;
}


int black_depth(rbTreeNode *node) {
    if (node == LEAF)
        return 0;
    int l, r;
    l = black_depth(node->left);
    r = black_depth(node->right);
    if (l == -1 || r == -1 || l != r)
        return -1;
    return l + node->color; // cheaty because black is 1
}


bool no_red_red(rbTreeNode *node) {
    if (node == LEAF)
        return 1;
    if (node->color == RED)
        if ((node->left->color == RED) or (node->right->color == RED))
            return 0;
    return no_red_red(node->left) and no_red_red(node->right);
}

bool parents_valid(rbTreeNode *node) {
    if (node == LEAF)
        return 1;
    if (node->left != LEAF) {
        if (node->left->parent != node)
            return 0;
        if (!parents_valid(node->left))
            return 0;
    }
    if (node->right != LEAF) {
        if (node->right->parent != node)
            return 0;
        if (!parents_valid(node->right))
            return 0;
    }
    return 1;
}


int rb_validate(rbtree tree) {
    if (root(tree)->color != BLACK) {
        fprintf(stderr, "Root of tree is not black\n");
        return 1;
    }

    // validate bst property
    if (!bst_valid(root(tree))) {
        fprintf(stderr, "Tree does not satisfy the bst property\n");
        return 2;
    }

    // validate black depth property
    if (black_depth(root(tree)) == -1) {
        fprintf(stderr, "Black depth of tree is not consistent\n");
        return 3;
    }

    // validate rule that no red node can have a red child
    if (!no_red_red(root(tree))) {
        fprintf(stderr, "A red node has a red child\n");
        return 4;
    }

    // validate that parent pointers are correct
    if (!parents_valid(root(tree))) {
        fprintf(stderr, "Parent pointers are wrong\n");
        return 5;
    }
    if (root(tree) != LEAF && root(tree)->parent != tree) {
        fprintf(stderr, "Root of tree not pointing back to top\n");
        return 5;
    }
    return 0;
}




/* 
 * -------------------------- rb_size --------------------------
 */


int rb_size_(rbTreeNode* node) {
    if (node == LEAF)
        return 0;
    return rb_size_(node->left) + rb_size_(node->right) + 1;
}

int rb_size(rbtree tree) {
    return rb_size_(root(tree));
}

