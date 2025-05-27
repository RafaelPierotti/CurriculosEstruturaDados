#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

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
            printf("Nome: %s, Area: %s, Qualificações: %s, Experiência: %s, Contato: %s\n",root->keys[i].nome,
                   root->keys[i].area, root->keys[i].qualificacoes, root->keys[i].experiencia, root->keys[i].contato);
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

        while (i >= 0 && stricmp(key.nome, node->keys[i].nome) < 0){
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n++;
    } else {
        while (i >= 0 && stricmp(key.nome, node->keys[i].nome) < 0){
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

            int i = (stricmp(key.nome, newRoot->keys[0].nome) > 0) ? 1 : 0;
            insertNonFull(newRoot->children[i], key);
            root = newRoot;
        } else {
            insertNonFull(root, key);
        }
    }
    return root;
}

int contatoExiste(BTreeNode *node, const char *contato){
    if (node == NULL) {
        return 0;
    }

    for (int i = 0; i < node->n; i++){
        if(strcmp(node->keys[i].contato, contato) == 0) {
            return 1;
        }

        if(!node->leaf){
            if (contatoExiste(node->children[i], contato)){
                return 1;
            }
        }
    }

    if (!node->leaf && node->children[node->n] != NULL){
        if (contatoExiste(node->children[node->n], contato)){
            return 1;
        }
    }

    return 0;
}

void insertCurriculo(BTreeNode **root){
    char nome[100], area[50], qualificacoes[200], experiencia[200], contato [100];

    printf("Insira as informações do currículo\n");

    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Área: ");
    fgets(area, sizeof(area), stdin);
    area[strcspn(area, "\n")] = '\0';

    printf("Qualificações: ");
    fgets(qualificacoes, sizeof(qualificacoes), stdin);
    qualificacoes[strcspn(qualificacoes, "\n")] = '\0';

    printf("Experiência: ");
    fgets(experiencia, sizeof(experiencia), stdin);
    experiencia[strcspn(experiencia, "\n")] = '\0';

    printf("Contato: ");
    fgets(contato, sizeof(contato), stdin);
    contato[strcspn(contato, "\n")] = '\0';

    if(contatoExiste(*root, contato)){
        printf("Contato já existente no sistema!");
        return;
    }

    Curriculo newCurriculo = createCurriculo(nome, area, qualificacoes, experiencia, contato);
    *root = insert(*root, newCurriculo);

    printf("Currículo adicionado");
}

void buscarNome(BTreeNode *root, const char *nome, bool *encontrado){
    if (root == NULL){
        return;
    }

    for (int i = 0; i < root->n; i++){
        if (!root->leaf){
            buscarNome(root->children[i], nome, encontrado);
        }

        if (strnicmp(root->keys[i].nome, nome, strlen(nome)) == 0){
            printf("Nome: %s, Area: %s, Qualificações: %s, Experiência: %s, Contato: %s\n",root->keys[i].nome,
                   root->keys[i].area, root->keys[i].qualificacoes, root->keys[i].experiencia, root->keys[i].contato);
            *encontrado = true;
        }
    }

    if (!root->leaf){
        buscarNome(root->children[root->n], nome, encontrado);
    }
}

void menu_principal(BTreeNode *root){

    int opcao;
    char busca[100];

    do {
        printf("\n--Menu--\n");
        printf("1 - Adicionar currículo\n");
        printf("2 - Exibir currículos\n");
        printf("3 - Buscar por nome\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao){
            case 1:
                insertCurriculo(&root);
                break;
            case 2:
                printf("Árvore organizado por nome\n");
                traverse(root);
                break;
            case 3:
                printf("Digite o nome: ");
                fgets(busca, sizeof(busca), stdin);
                busca[strcspn(busca, "\n")] = '\0';

                bool encontrado = false;
                buscarNome(root, busca, &encontrado);

                if (!encontrado){
                    printf("Nenhum currículo encontrado!\n");
                }
                break;
            case 0:
                printf("Saindo..");
                break;
            default:
                printf("Opção inválida");
        }
    } while (opcao != 0);
}

void adicionaExemplos(BTreeNode **root){
    Curriculo curriculos[] = {
        createCurriculo("Carlos", "Engenharia", "AutoCAD", "5 anos", "carlos@email.com"),
        createCurriculo("Bruno", "Marketing", "SEO, Copy", "2 anos", "bruno@email.com"),
        createCurriculo("Alice", "TI", "C, Java", "3 anos", "alice@email.com"),
        createCurriculo("Diana", "RH", "Comunicação", "4 anos", "diana@email.com")
    };

    int n = sizeof(curriculos) / sizeof(curriculos[0]);

    for (int i = 0; i < n; i++){
        *root = insert(*root, curriculos[i]);
    }
}

int main (){
    setlocale(LC_ALL, "Portuguese");

    BTreeNode *root = NULL;

    adicionaExemplos(&root);

    menu_principal(root);

    return 0;
}
