#include <stdio.h>
#include <stdlib.h> // Para malloc e free
#include <string.h> // Para strcmp e strcpy

// ----- CONSTANTES E STRUCTS -----

#define HASH_TABLE_SIZE 10 // Tamanho da nossa tabela hash

/**
 * @struct Sala
 * @brief (MAPA) Define a estrutura do comodo (arvore binaria simples).
 */
typedef struct Sala {
    char* nome;
    char* pista; // Pista opcional
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

/**
 * @struct PistaNode
 * @brief (PISTAS) Define a estrutura da arvore de pistas (BST).
 */
typedef struct PistaNode {
    char* pista;
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

/**
 * @struct HashEntry
 * @brief (SUSPEITOS) Define uma entrada na Tabela Hash.
 * Usa "encadeamento" para tratar colisoes.
 */
typedef struct HashEntry {
    char* pista;    // Chave
    char* suspeito; // Valor
    struct HashEntry* next; // Proximo item na lista (em caso de colisao)
} HashEntry;

// A Tabela Hash e um array global de ponteiros
HashEntry* TabelaSuspeitos[HASH_TABLE_SIZE];


// ----- FUNCOES DA TABELA HASH -----

/**
 * @brief Funcao Hash (djb2)
 * Converte uma string (pista) em um indice para a tabela.
 */
unsigned int hash(char* str) {
    unsigned long hash_val = 5381;
    int c;
    while ((c = *str++)) {
        hash_val = ((hash_val << 5) + hash_val) + c; // hash * 33 + c
    }
    return hash_val % HASH_TABLE_SIZE;
}

/**
 * @brief Inicializa a tabela hash com valores nulos.
 */
void inicializarHash() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        TabelaSuspeitos[i] = NULL;
    }
}

/**
 * @brief insere associacao pista/suspeito na tabela hash.
 */
void inserirNaHash(char* pista, char* suspeito) {
    unsigned int index = hash(pista); // Calcula o indice
    
    // Cria a nova entrada
    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    newEntry->pista = pista;
    newEntry->suspeito = suspeito;
    
    // Adiciona no inicio da lista naquele indice
    newEntry->next = TabelaSuspeitos[index];
    TabelaSuspeitos[index] = newEntry;
}

/**
 * @brief consulta o suspeito correspondente a uma pista.
 *
 * Busca na tabela hash e retorna o nome do suspeito,
 * ou NULL se a pista nao for encontrada.
 */
char* encontrarSuspeito(char* pista) {
    unsigned int index = hash(pista);
    
    HashEntry* current = TabelaSuspeitos[index];
    
    // Percorre a lista encadeada naquele indice
    while (current != NULL) {
        if (strcmp(current->pista, pista) == 0) {
            return current->suspeito; // Encontrou!
        }
        current = current->next;
    }
    
    return NULL; // Nao encontrou
}


// ----- FUNCOES DAS ARVORES (Mapa e Pistas) -----

/**
 * @brief cria dinamicamente um comodo.
 */
Sala* criarSala(char* nome, char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    novaSala->nome = nome;
    novaSala->pista = pista;
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * @brief insere a pista coletada na arvore de pistas (BST).
 * Mantem as pistas em ordem alfabetica.
 */
PistaNode* inserirPista(PistaNode* raiz, char* pista) {
    if (raiz == NULL) { // Arvore vazia
        PistaNode* novoNo = (PistaNode*)malloc(sizeof(PistaNode));
        novoNo->pista = pista;
        novoNo->esquerda = NULL;
        novoNo->direita = NULL;
        printf("  -> Pista coletada: %s\n", pista);
        return novoNo;
    }

    int cmp = strcmp(pista, raiz->pista);
    
    if (cmp < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (cmp > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // se cmp == 0, a pista ja existe, nao faz nada
    
    return raiz;
}

/**
 * @brief Exibe as pistas da BST em ordem alfabetica (In-Order).
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("  - %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// ----- FUNCOES DE LOGICA DO JOGO -----

/**
 * @brief Conta quantas pistas COLETADAS apontam para um suspeito.
 *
 * Esta funcao percorre a BST (arvore de pistas coletadas)
 * e, para cada pista, consulta a Tabela Hash para ver
 * quem e o suspeito.
 */
int contarPistasPorSuspeito(PistaNode* raizPistas, char* suspeito) {
    if (raizPistas == NULL) {
        return 0; // Nenhuma pista aqui
    }

    int count = 0;
    
    // Verifica o suspeito da pista atual
    char* suspeitoDaPista = encontrarSuspeito(raizPistas->pista);
    
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeito) == 0) {
        count = 1; // Esta pista aponta para o suspeito
    }
    
    // Soma com as pistas das sub-arvores (recursao)
    return count + 
           contarPistasPorSuspeito(raizPistas->esquerda, suspeito) + 
           contarPistasPorSuspeito(raizPistas->direita, suspeito);
}

/**
 * @brief navega pela arvore e ativa o sistema de pistas.
 */
void explorarSalas(Sala* salaAtual, PistaNode** raizPistas) {
    char escolha;

    while (1) {
        printf("\n--------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // Coleta a pista se ela existir
        if (salaAtual->pista != NULL) {
            *raizPistas = inserirPista(*raizPistas, salaAtual->pista);
        }

        // Mostra opcoes
        printf("Escolha seu caminho:\n");
        if (salaAtual->esquerda != NULL) printf(" (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL) printf(" (d) Direita: %s\n", salaAtual->direita->nome);
        printf(" (s) Sair e fazer acusacao\n");
        printf("Opcao: ");

        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } 
        else if (escolha == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        }
        else if (escolha == 's') {
            printf("Voce decide que ja viu o suficiente...\n");
            break; 
        }
        else {
            printf("Opcao invalida ou caminho nao existe.\n");
        }
    }
}

/**
 * @brief conduz a fase de julgamento final.
 *
 * Pede ao jogador para acusar um suspeito e verifica
 * se ha pistas suficientes (>= 2) para a acusacao.
 */
void verificarSuspeitoFinal(PistaNode* raizPistas) {
    char acusado[50];

    printf("\n--- HORA DO JULGAMENTO ---\n");
    printf("Com base nas pistas, quem e o culpado?\n");
    printf("(Mordomo, Cozinheira, Jardineiro): ");
    
    // Le o nome do suspeito
    scanf("%49s", acusado); 

    // Conta as pistas contra o acusado
    int numPistas = contarPistasPorSuspeito(raizPistas, acusado);

    printf("\nVerificando as pistas contra %s...\n", acusado);
    printf("Pistas encontradas: %d\n", numPistas);

    // Requisito: Pelo menos 2 pistas
    if (numPistas >= 2) {
        printf("Desfecho: CULPADO! As provas sao contundentes. Bom trabalho, detetive!\n");
    } else {
        printf("Desfecho: INOCENTE! Voce acusou a pessoa errada. O verdadeiro culpado escapou...\n");
    }
}


/**
 * @brief Funcao Principal
 *
 * Vamos tentar usar "int main(int argc, char *argv[])"
 * para ver se ajuda o seu compilador local.
 */
int main(int argc, char *argv[]) {
    
    PistaNode* raizPistas = NULL; // Arvore de pistas comeca vazia
    inicializarHash(); // Limpa a tabela hash

    // 1. Popular a Tabela Hash (Pista -> Suspeito)
    // Estas sao todas as pistas POSSIVEIS e seus donos
    inserirNaHash("Lareira acesa", "Mordomo");
    inserirNaHash("Faca desaparecida", "Cozinheira");
    inserirNaHash("Livro de venenos", "Jardineiro");
    inserirNaHash("Pegadas na lama", "Jardineiro");
    inserirNaHash("Prato quebrado", "Mordomo");

    // 2. Montagem do Mapa (com as pistas)
    Sala* hall = criarSala("Hall de Entrada", NULL);
    
    hall->esquerda = criarSala("Sala de Estar", "Lareira acesa");
    hall->direita = criarSala("Cozinha", "Faca desaparecida");
    
    hall->esquerda->esquerda = criarSala("Biblioteca", "Livro de venenos");
    hall->esquerda->direita = criarSala("Jardim", "Pegadas na lama");
    
    hall->direita->esquerda = criarSala("Despensa", NULL); // Sem pista
    hall->direita->direita = criarSala("Sala de Jantar", "Prato quebrado");

    // 3. Iniciar o Jogo
    printf("--- Detective Quest (Mestre) ---\n");
    
    explorarSalas(hall, &raizPistas);

    // 4. Exibir Pistas Coletadas (em ordem)
    printf("\n--- PISTAS COLETADAS (em ordem alfabetica) ---\n");
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }
    
    // 5. Fase da Acusacao
    verificarSuspeitoFinal(raizPistas);

    // Nota: Em um programa real, precisariamos usar free()
    // para liberar toda a memoria alocada (arvores e hash).

    return 0;
}