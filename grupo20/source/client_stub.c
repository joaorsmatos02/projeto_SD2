#include "../include/client_stub.h"
#include "../include/client_stub-private.h"
#include "../include/data.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sdmessage.pb-c.h"
#include "../include/network_client.h"


/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port){
    struct rtree_t* connection = (struct rtree_t*) malloc(sizeof(struct rtree_t));
    
    if(connection == NULL)
        return NULL;

    char* ipPortBuffer = strtok(strdup(address_port), ":");
    char** ipPortTokens = (char**) malloc(sizeof(char*) * 2);
    int tokenQuantity = 0;

    // extrair os tokens o array inputTokens
    for(;ipPortBuffer != NULL; tokenQuantity++) {
        ipPortTokens[tokenQuantity] = ipPortBuffer;
        ipPortBuffer = strtok(NULL, " ");
    }

    connection->ip_addr = ipPortTokens[0];
    connection->port = ipPortTokens[1];

    if(network_connect(connection) == 0)
        return connection;

    return NULL;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){
    return network_close(rtree);
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    struct _MessageT* messageSend = (struct _MessageT*) malloc(sizeof(struct _MessageT));
    message_t__init(messageSend);

    EntryT entry_copy;
    entry_copy.key = entry->key;
    DataT data_copy;
    data_copy.data = entry->value->data;
    data_copy.datasize = entry->value->datasize;
    entry_copy.value = &data_copy;
    messageSend->opcode = MESSAGE_T__OPCODE__OP_PUT;
    messageSend->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    messageSend->entry = &entry_copy;
    
    struct _MessageT* messageReceive = network_send_receive(rtree, messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_PUT+1)
        return 0;
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return -1;

    return -1;
}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_GET;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_KEY;
    messageSend.key = key;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_GET+1) {
        struct data_t* data = data_create(messageReceive->data->datasize);
        data->data = messageReceive->data->data;
        return data;
    }
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return NULL;

    return NULL;
}

/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_DEL;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_KEY;
    messageSend.key = key;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_DEL+1)
        return 0;
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return -1;

    return -1;
}

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_SIZE;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_SIZE+1)
        return messageReceive->size_height;
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return -1;

    return -1;
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_HEIGHT;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_HEIGHT+1)
        return messageReceive->size_height;
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return -1;

    return -1;
}

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_GETKEYS+1)
        return messageReceive->keys;
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return NULL;

    return NULL;
}

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree){
    struct _MessageT messageSend;

    messageSend.opcode = MESSAGE_T__OPCODE__OP_GETVALUES;
    messageSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    struct _MessageT* messageReceive = network_send_receive(rtree, &messageSend);

    if(messageReceive->opcode == MESSAGE_T__OPCODE__OP_GETVALUES+1) {
        void** result = malloc(messageReceive->n_values * sizeof(void*));

        for(int i = 0; i < messageReceive->n_values; i++)
            memcpy(result[i], messageReceive->values[i].data, messageReceive->values->len);
        
        return result;
    }
    else if (messageReceive->opcode == MESSAGE_T__OPCODE__OP_ERROR)
        return NULL;

    return NULL;
}