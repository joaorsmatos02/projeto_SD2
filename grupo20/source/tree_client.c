#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/client_stub.h"
#include "../include/tree_client.h"
#include "../include/entry.h"
#include "../include/data.h"

int main(int argc, char* argv[]) {

    //estabelecer conexao com servidor
    struct rtree_t* remoteTree = rtree_connect(argv[1]);
    //verificar se a conexao foi bem sucedida
    if(remoteTree == NULL)
        exit(-1);

    char* bufferStdio = (char*) malloc(sizeof(char)*256);

    printMenu();

    printf("Insira o seu pedido: ");

    char** inputTokens = (char**) malloc(sizeof(char*) * 10);
    int tokenQuantity = 0;

    do {
        // ler do stdin
        fgets(bufferStdio, 256, stdin);
        char* tokenBuffer = strtok(bufferStdio, " ");

        // extrair os tokens o array inputTokens
        for(;tokenBuffer != NULL; tokenQuantity++) {
            inputTokens[tokenQuantity] = tokenBuffer;
            tokenBuffer = strtok(NULL, " ");
        }

        //process the input
        inputHandler(inputTokens, tokenQuantity, remoteTree);

        inputTokens = NULL;
        tokenQuantity = 0;

      // tem de ser mesmo "quit" e nao "quit blablabla"
    } while(strcmp(inputTokens[0], "quit") != 0 && tokenQuantity != 1);  

    return rtree_disconnect(remoteTree);
}

void inputHandler(char** inputTokens, int tokenQuantity, struct rtree_t* rtree) {

    int result = -1;

    if(strcmp(inputTokens[0], "put") == 0) {
        
        if(tokenQuantity == 3) {
            struct data_t* value = data_create(strlen(inputTokens[2]));
            memcpy(value->data, inputTokens[2], value->datasize);

            struct entry_t* entry = entry_create(strdup(inputTokens[1]), value);
            
            result = rtree_put(rtree, entry);
        }
    }
    else if(strcmp(inputTokens[0], "get") == 0) {
        if(tokenQuantity == 2) {
            struct data_t* data = rtree_get(rtree, inputTokens[1]);

            if(data != NULL) {
                result = 0;
                printf("Answer: %s\n", (char *) data->data); //ASSUMINDO QUE É UMA STRING
            }
        }
    }
    else if(strcmp(inputTokens[0], "del") == 0) {
        if(tokenQuantity == 2)
            result = rtree_del(rtree, inputTokens[1]);
    }
    else if(strcmp(inputTokens[0], "size") == 0) {
        if(tokenQuantity == 1) {
            int sizeTree = rtree_size(rtree);
            printf("Answer: %d\n", sizeTree);
            result = 0;
        }
    }
    else if(strcmp(inputTokens[0], "height") == 0) {
        if(tokenQuantity == 1) {
            int heightTree = rtree_height(rtree);
            printf("Answer: %d\n", heightTree);
            result = 0;
        }
    }
    else if(strcmp(inputTokens[0], "getkeys") == 0) {
        if(tokenQuantity == 1) {
            char** keys = rtree_get_keys(rtree);
            result = 0;

            printf("Answer: \n");
            for(int i = 0; keys[i] != NULL; i++)
                printf("%s ", keys[i]);
        }
    }
    else if(strcmp(inputTokens[0], "getvalues") == 0) {
        if(tokenQuantity == 1) {
            char** values = (char**) rtree_get_values(rtree);
            result = 0;

            printf("Answer: \n");
            for(int i = 0; values[i] != NULL; i++)
                printf("%s ", values[i]);
        }
    }

    if (result == 0)
        printf("Operação bem sucedida!");
    else
        printf("Erro na operação...\n\nInsira um pedido válido: ");
}

void printMenu() {
    printf("MENU OPTIONS: \n\n");
    printf("put <key> <data>    - Esta função permite ao utilizador colocar uma entrada na árvore uma chave(key) e um valor(data)\n\n");
    printf("get <key>           - Devolve a entrada da árvore identificada pela chave(key)\n\n");
    printf("del <key>           - Elimina uma entrada da árvore identificada pela chave(key)\n\n");
    printf("size                - Devolve o tamanho da árvore\n\n");
    printf("height              - Devolve a altura da árvore\n\n");
    printf("getkeys             - Devolve todas as chaves(keys) das entradas da árvore\n\n");
    printf("getvalues           - Devolve todos os valores(values) de todas as entradas da árvore\n\n");
    printf("quit                - Termina o programa\n");
}