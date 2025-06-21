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
    return (tree->lower_than(key1,key2) == 0 && tree->lower_than(key2,key1) == 0);
}

TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (!new) {
        fprintf(stderr, "No pude crear el nodo, memoria insuficiente\n");
        return NULL;
    }
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

// se crea un arbol vacio como el presupuesto del edificio de informatica
TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    if (!new) {
        fprintf(stderr, "Error al crear TreeMap\n");
        return NULL;
    }
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}

// para insertar nuevo par
void insertTreeMap(TreeMap * tree, void* key, void * value) {
    TreeNode *parent = NULL;
    TreeNode *current = tree->root;

    // mini funcion para comparar claves
    while (current != NULL) {
        parent = current;

        if (is_equal(tree, key, current->pair->key)) {
            // clave existente
            return;
        }

        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }

    // nuevo nodo a enlazar
    TreeNode *newNode = createTreeNode(key, value);
    newNode->parent = parent;

    if (parent == NULL)
        tree->root = newNode; // caso arbol vacio 
    else if (tree->lower_than(key, parent->pair->key))
        parent->left = newNode;
    else
        parent->right = newNode;

    tree->current = newNode; // puntero a nuevo nodo
}

// nodo minimo
TreeNode * minimum(TreeNode * x) {
    if (x == NULL) return NULL;
    while (x->left != NULL)
        x = x->left;
    return x;
}

// chao nodo
void removeNode(TreeMap * tree, TreeNode* node) {
    // caso nodo sin hijos
    if (node->left == NULL && node->right == NULL) {
        if (node->parent != NULL) {
            if (node == node->parent->left) node->parent->left = NULL;
            else node->parent->right = NULL;
        } else {
            tree->root = NULL; // el nodo era la raíz
        }
    }
    // caso nodo con un hijo
    else if (node->left == NULL || node->right == NULL) {
        TreeNode *child = (node->left != NULL) ? node->left : node->right;
        child->parent = node->parent;

        if (node->parent == NULL)
            tree->root = child; // El hijo pasa a ser la raíz
        else if (node == node->parent->left)
            node->parent->left = child;
        else
            node->parent->right = child;
    }
    // caso nodo con dos hijos, familia tradicional
    else {
        TreeNode *succ = minimum(node->right);
        node->pair = succ->pair;  // Copiamos datos
        removeNode(tree, succ);   // Eliminamos sucesor
        return;
    }
    free(node);
}

// busca el par con la clave dada, si existe
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
    return NULL; // No encontrado
}

// busca el par con la clave de arriba
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

// busca el par con la clave de abajo
Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode *min = minimum(tree->root);
    tree->current = min;
    return (min != NULL) ? min->pair : NULL;
}

// encuentra el sucesor de un nodo
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

// devuelve el siguiente par en orden
Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;

    TreeNode *succ = successor(tree->current);
    tree->current = succ; // nuevo sucesor

    return (succ != NULL) ? succ->pair : NULL;
}

// chao nodo (por clave)
void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return; // No está la clave

    TreeNode* node = tree->current;
    removeNode(tree, node);
}
