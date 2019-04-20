
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>

#include "rbtree.h"


int full_test_insert() {
    std::vector<int> test;
    rbtree tree;
    std::clock_t start;
    int retval = 0;

    tree = rb_create();
    srand(time(0));

    // test on large random input
#define ELEMS 10000
    for (size_t i = 0; i < ELEMS; i++) {
        test.push_back(rand() % 3000);
    }

    start = std::clock();
    for (auto it = test.begin(); it != test.end(); ++it) {
        rb_insert(tree, *it);
    }
    printf("runtime on size %u: %f\n", ELEMS, (std::clock() - start) / (float) CLOCKS_PER_SEC);
    rb_validate(tree);

    for (auto it = test.begin(); it != test.end(); ++it) {
        if (!rb_find(tree, *it)) {
            fprintf(stderr, "element %d no longer in tree!\n", *it);
            retval = 1;
            break;
        }
    }

    rb_delete(tree);

    return retval;
}

int full_test_delete() {
    std::vector<int> test;
    rbtree tree;
    std::clock_t start;
    int retval = 0;

    tree = rb_create();

    for (size_t i = 0; i < ELEMS; i++) {
        test.push_back(rand() % 3000);
    }
    
    for (auto it = test.begin(); it != test.end(); ++it) {
        rb_insert(tree, *it);
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(test), std::end(test), rng);

    start = std::clock();
    for (auto it = test.begin(); it != test.end(); ++it) {
        rb_remove(tree, *it);
    }
    printf("runtime on size %u: %f\n", ELEMS, (std::clock() - start) / (float) CLOCKS_PER_SEC);
    rb_validate(tree);

    rb_delete(tree);

    return 0;
}


int full_test() {
    int a;
    a = full_test_insert();
    if (a) {
        return a;
    }
    a = full_test_delete();
    return a;
}


int main() {
    full_test();

    return 0;
}
