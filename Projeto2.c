#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define M 4

typedef struct curriculo {
    char nome [100];
    char area [50];
    char qualificacoes [200];
    char experiencia [200];
    char contato [100];
}Curriculo;

typedef struct BTreeNode {
    Curriculo keys [M - 1];
    struct BTreeNode *children[M];
    int n;
    int leaf;
}BTreeNode;

BTreeNode *createNode(int leaf){
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));

    node->n = 0;
    node->leaf = leaf;
    for(int i = 0; i < M; i++){
        node->children[i] = NULL;
    }

    return node;
}

Curriculo createCurriculo(const char *nome, const char *area, const char *qualificacoes, const char *experiencia, const char *contato){
    Curriculo curriculo;
    strcpy(curriculo.nome, nome);
    strcpy(curriculo.area, area);
    strcpy(curriculo.qualificacoes, qualificacoes);
    strcpy(curriculo.experiencia, experiencia);
    strcpy(curriculo.contato, contato);
    return curriculo;
}

void traverse(BTreeNode *root){
    if (root != NULL){
        int i;
        for(i = 0; i < root->n; i++){
            if (!root->leaf){
                traverse(root->children[i]);
            }
            printf("Nome: %s, Area: %s, Qualificações: %s, Experiência: %s, Contato: %s",root->keys[i].nome,
                   root->keys[i].qualificacoes, root->keys[i].experiencia, root->keys[i].contato);
            fflush(stdout);

        }
        if (!root->leaf){
            traverse(root->children[root->n]);
        }
    }
}

void splitChild(BTreeNode *parent, int i, BTreeNode *child){
    BTreeNode *newNode = createNode(child->leaf);
    newNode->n = M / 2 - 1;

    for (int j = 0; j < M / 2 - 1; j++){
        newNode->keys[j] = child->keys[j + M / 2];
    }

    if (!child->leaf){
        for(int j = 0; j < M / 2; j++){
            newNode->children[j] = child->children[j + M /2];
        }
    }
    child->n = M / 2 - 1;

    for (int j = parent->n; j >= i + 1; j--){
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newNode;

    for (int j = parent->n - 1; j >= i; j--){
        parent->keys[j + 1] = parent->keys[j];
    }

    parent->keys[i] = child->keys[M / 2 - 1];
    parent->n++;
}

void insertNonFull(BTreeNode *node, Curriculo key){
    int i = node->n - 1;

    if(node->leaf){
        while (i >= 0 && strcmp(key.nome, node->keys[i].nome) < 0){
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n++;
    } else {
        while (i >= 0 && strcmp(key.nome, node->keys[i].nome) < 0){
            i--;
        }
        i++;
        if (node->children[i]->n == M - 1){
            splitChild(node, i, node->children[i]);
            if (strcmp(key.nome, node->keys[i].nome) > 0) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

BTreeNode *insert(BTreeNode *root, Curriculo key){
    if (root == NULL){
        root = createNode(1);
        root->keys[0] = key;
        root->n = 1;
    } else {
        if (root->n == M - 1){
            BTreeNode *newRoot = createNode(0);
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);

            int i = (strcmp(key.nome, newRoot->keys[0].nome) > 0) ? 1 : 0;
            insertNonFull(newRoot->children[i], key);
            root = newRoot;
        } else {
            insertNonFull(root, key);
        }
    }
    return root;
}

int main (){
    setlocale(LC_ALL, "Portuguese");

    BTreeNode *root = NULL;

    Curriculo curriculos[] = {
        createCurriculo("Carlos", "Engenharia", "AutoCAD", "5 anos", "carlos@email.com"),
        createCurriculo("Bruno", "Marketing", "SEO, Copy", "2 anos", "bruno@email.com"),
        createCurriculo("Alice", "TI", "C, Java", "3 anos", "alice@email.com"),
        createCurriculo("Diana", "RH", "Comunicação", "4 anos", "diana@email.com")
    };

    int n = sizeof(curriculos) / sizeof(curriculos[0]);

    for (int i = 0; i < n; i++){
        root = insert(root, curriculos[i]);
    }

    printf("Árvore B organizando por nome: \n");
    fflush(stdout);
    traverse(root);
    printf("\n");
    fflush(stdout);


    return 0;
}
