#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}

void insertTreeMap(TreeMap * tree, void* key, void * value) {
    TreeNode *parent = NULL;
    TreeNode *current = tree->root;

    while (current != NULL) {
        parent = current;

        if (is_equal(tree, key, current->pair->key)) return; // Evitar duplicados

        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }

    TreeNode *newNode = createTreeNode(key, value);
    newNode->parent = parent;

    if (parent == NULL)
        tree->root = newNode;
    else if (tree->lower_than(key, parent->pair->key))
        parent->left = newNode;
    else
        parent->right = newNode;

    tree->current = newNode;
}

TreeNode * minimum(TreeNode * x) {
    if (x == NULL) return NULL;
    while (x->left != NULL)
        x = x->left;
    return x;
}

void removeNode(TreeMap * tree, TreeNode* node) {
    if (node->left == NULL && node->right == NULL) { // Caso 1: Nodo hoja
        if (node == node->parent->left) node->parent->left = NULL;
        else node->parent->right = NULL;
    }
    else if (node->left == NULL || node->right == NULL) { // Caso 2: Un hijo
        TreeNode *child = (node->left != NULL) ? node->left : node->right;
        child->parent = node->parent;

        if (node->parent == NULL)
            tree->root = child;
        else if (node == node->parent->left)
            node->parent->left = child;
        else
            node->parent->right = child;
    }
    else { // Caso 3: Dos hijos
        TreeNode *succ = minimum(node->right);
        node->pair = succ->pair;
        removeNode(tree, succ);
        return;
    }
    free(node);
}

Pair * searchTreeMap(TreeMap * tree, void* key) {
    TreeNode *current = tree->root;

    while (current != NULL) {
        if (is_equal(tree, key, current->pair->key)) {
            tree->current = current;
            return current->pair;
        }
        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }
    return NULL;
}

Pair * upperBound(TreeMap * tree, void* key) {
    TreeNode *current = tree->root;
    TreeNode *ub = NULL;

    while (current != NULL) {
        if (tree->lower_than(current->pair->key, key)) {
            current = current->right;
        } else {
            ub = current;
            current = current->left;
        }
    }

    if (ub != NULL) {
        tree->current = ub;
        return ub->pair;
    }
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode *min = minimum(tree->root);
    tree->current = min;
    return (min != NULL) ? min->pair : NULL;
}

TreeNode * successor(TreeNode * node) {
    if (node->right != NULL)
        return minimum(node->right);

    TreeNode *p = node->parent;
    while (p != NULL && node == p->right) {
        node = p;
        p = p->parent;
    }
    return p;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;

    TreeNode *succ = successor(tree->current);
    tree->current = succ;

    return (succ != NULL) ? succ->pair : NULL;
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);
}