// Sistemas Operacionais
// Projeto 1
// Samantha Dantas Medeiros @ 25/08/23

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// macros
#define quantum 2

// estruturas
struct processo {
  int id; 
  int tempo_chegada;
  int duracao_pico; 
  int pico_restante; 
  int tempo_termino; 
  int tempo_retorno; 
  int tempo_resposta; 
  int tempo_espera; 
};

struct filaProcessos {
  struct processo **processos;
  int inicio;
  int fim;
  int tamanho;
  int qtd_fila;
};

// protótipos
int contar_processos(FILE*);
int compararProcessosTempo(const void *a, const void *b); // compara pelo tempo
int compararProcessosTempoPico(const void *a, const void *b); // compara pelo tempo e duracao do pico

bool contemObjeto(struct processo array[], int tamanho, int id_procura); // verifica se um determinado elemento está contido no array

struct processo* copiarProcessos(int qtd_processos, const struct processo *processos);

// filas de escalonamento
void simula_fcfs(int qtd_processos, struct processo *processos);
void simula_sfj(int qtd_processos, struct processo *processos);
void simula_rr(int qtd_processos, struct processo *processos, int qq); // qq = 2

// funções para operações de filas
struct filaProcessos *criar_fila_processos(int processos_na_fila);
struct processo *obterUltimoConcluido(struct filaProcessos *fila_concluidos);
int fila_vazia(struct filaProcessos *fila);
void enfileirar(struct filaProcessos *fila, struct processo *processo);
void exibirfilaProcessos(struct filaProcessos *fila);
struct processo *desenfileirar(struct filaProcessos *fila);

// main
void main() {
  int qtd_processos = 0, num_processos = 0;
  FILE *arq;
  // arq = fopen("testes/teste1.txt","r"); // teste1 -> exemplo dos slides
  arq = fopen("professor.txt","r"); // teste1 -> exemplo dos slides

  // Verificando leitura do arquivo 
  if(arq == NULL) {
    printf("Não foi possível completar a solicitação!\nEncerrando...\n");
    exit(1);
  }

  qtd_processos = contar_processos(arq);

  // Retorna ao início do arquivo para ler os processos
  fseek(arq, 0, SEEK_SET);

  // Armazena dados em um array struct processo (alocado dinamicamente)
  struct processo *processos = malloc(qtd_processos * sizeof(struct processo));
  while (fscanf(arq, "%d %d", &processos[num_processos].tempo_chegada, &processos[num_processos].duracao_pico) == 2) {
    processos[num_processos].id = num_processos + 1;
    processos[num_processos].pico_restante = processos[num_processos].duracao_pico;

    num_processos++;
  }

  simula_fcfs(qtd_processos, processos);
  simula_sfj(qtd_processos, processos);
  simula_rr(qtd_processos, processos, quantum);

  free(processos);
  fclose(arq);
}

/**
 * Conta a quantidade de linhas do arquivo de entrada, correspondendo ao número de processos
 * @param arquivo arquivo de entrada
 * @returns a quantidade de processos no arquivo
*/
int contar_processos(FILE *arquivo) {
  int contador = 0;
  char caractere;

  while ((caractere = fgetc(arquivo)) != EOF) {
    if (caractere == '\n') {
      contador++;
    }
  }

  // Se o último caractere não foi uma quebra de linha, também conta como uma linha
  if (caractere != '\n' && contador > 0) {
    contador++;
  }

  return contador;
}

// Função de comparação para qsort (ordenação por duração do pico)
int compararProcessosTempoPico(const void *a, const void *b) {
  const struct processo *processoA = (const struct processo *)a;
  const struct processo *processoB = (const struct processo *)b;

  // Primeiro, compara pelo tempo de chegada
  if (processoA->tempo_chegada < processoB->tempo_chegada) return -1;
  if (processoA->tempo_chegada > processoB->tempo_chegada) return 1;

  // Se o tempo de chegada for igual, compara pela duração de pico
  if (processoA->duracao_pico < processoB->duracao_pico) return -1;
  if (processoA->duracao_pico > processoB->duracao_pico) return 1;

  // Se ambos forem iguais, mantenha a ordem original
  return 0;
}
int compararProcessosTempoChegada(const void *a, const void *b) {
  const struct processo *processoA = (const struct processo *)a;
  const struct processo *processoB = (const struct processo *)b;
  // Primeiro, compara pelo tempo de chegada
  if (processoA->tempo_chegada < processoB->tempo_chegada) return -1;
  if (processoA->tempo_chegada > processoB->tempo_chegada) return 1;

  // Se ambos forem iguais, mantenha a ordem original
  return 0;
}

// Função para verificar se um elemento está contido em um array
bool contemObjeto(struct processo array[], int tamanho, int id_procura) {
  for (int i = 0; i < tamanho; i++) {
    if (array[i].id == id_procura) {
      return true; 
    }
  }
  return false; 
}

struct processo *obterUltimoConcluido(struct filaProcessos *fila_concluidos) {
  if (fila_concluidos->inicio <= fila_concluidos->fim) {
      return fila_concluidos->processos[fila_concluidos->fim];
  } else {
      return NULL; // A fila de concluídos está vazia
  }
}

struct filaProcessos *criar_fila_processos(int processos_na_fila) {
  struct filaProcessos *fila = (struct filaProcessos *)malloc(sizeof(struct filaProcessos));
  if (fila == NULL) {
    printf("Não foi possível alocar memória para a fila.\n");
    exit(1);
  }

  fila->processos = (struct processo **)malloc(sizeof(struct processo *) * processos_na_fila);
  if(fila->processos == NULL) {
    printf("Não foi possível alocar memória para os processos na fila.\n");
    exit(1);
  }

  fila->inicio = -1; // 0
  fila->fim = -1;
  fila->tamanho = processos_na_fila;
  fila->qtd_fila = 0;

  return fila;
}

// Função para enfileirar um processo na fila ready
// void enfileirar(struct filaProcessos *fila, struct processo *processo) {
//   if ((fila->fim + 1) % fila->tamanho == fila->inicio) {
//     printf("Erro: a fila está cheia!\n");
//     exit(1);
//   }

//   if(fila->inicio == -1) {
//     fila->inicio = 0;
//   }

//   fila->fim = (fila->fim + 1) % fila->tamanho;
//   fila->processos[fila->fim] = processo;
// }

// Função para desenfileirar um processo da fila ready
// struct processo *desenfileirar(struct filaProcessos *fila) {
//   if (fila->inicio == -1) { 
//     return NULL;
//   }

//   struct processo *processo = fila->processos[fila->inicio];
//   if (fila->inicio == fila->fim) {
//     // Único elemento na fila, resete os índices para vazia
//     fila->inicio = -1;
//     fila->fim = -1;
//   } else {
//     fila->inicio = (fila->inicio + 1) % fila->tamanho;
//   }
//   return processo;
// }

void enfileirar(struct filaProcessos *fila, struct processo *processo) {
  if (fila->fim == fila->tamanho - 1) {
    // A fila está cheia, não podemos enfileirar mais processos
    printf("Erro: a fila está cheia!\n");
    exit(1);
  }
  fila->fim++;
  fila->processos[fila->fim] = processo;
}

struct processo *desenfileirar(struct filaProcessos *fila) {
  if (fila->inicio > fila->fim) {
    // A fila está vazia, não podemos desenfileirar
    return NULL;
  }
  struct processo *processo = fila->processos[fila->inicio];
  fila->inicio++;
  return processo;
}


int fila_vazia(struct filaProcessos *fila) {
  return fila->inicio > fila->fim;
}

// void exibirfilaProcessos(struct filaProcessos *fila) {
//   if (fila->inicio == -1) {
//     printf("Fila vazia.\n");
//     return;
//   }

//   int i = fila->inicio;

//   do {
//     printf("ID: %d, Tempo de Chegada: %d, Duração do Pico: %d\n",
//            fila->processos[i]->id, fila->processos[i]->tempo_chegada,
//            fila->processos[i]->duracao_pico);
    
//     i = (i + 1) % fila->tamanho;
//   } while (i != (fila->fim + 1) % fila->tamanho);
// }

void exibirfilaProcessos(struct filaProcessos *fila) {
  if (fila->inicio > fila->fim) {
    printf("Fila vazia.\n");
  } else {
   
    for (int i = fila->inicio; i <= fila->fim; i++) {
      struct processo *processo = fila->processos[i];
      printf("ID: %d, Tempo de Chegada: %d, Duração do Pico: %d\n", processo->id, processo->tempo_chegada, processo->duracao_pico);
    }
  }
}


bool buscaElementoNaFila(struct filaProcessos *fila, struct processo *elemento) {
  for (int i = fila->inicio; i <= fila->fim; i++) {
    if (fila->processos[i] == elemento) {
      return true; // Elemento encontrado na fila
    }
  }
  return false; // Elemento não encontrado na fila
}

// Função para copiar processos
struct processo* copiarProcessos(int qtd_processos, const struct processo *processos) {
  // Criar uma nova estrutura para armazenar a cópia dos processos
  struct processo *copia_processos = (struct processo *)malloc(qtd_processos * sizeof(struct processo));
  
  if (copia_processos == NULL) {
      // Lidar com erros de alocação de memória, se necessário
      perror("Erro ao alocar memória para a cópia dos processos");
      exit(EXIT_FAILURE);
  }

  // Copiar os processos para a nova estrutura
  for (int i = 0; i < qtd_processos; i++) {
      copia_processos[i] = processos[i];
  }

  // Retorna a cópia dos processos
  return copia_processos;
}

//************************************* Implementação de filas de processos 
/**
 * Simula fila de escalonamento FCFS
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
 * Informa o tempo médio de retorno, resposta e espera
*/
void simula_fcfs(int qtd_processos, struct processo *processos) {
  // Ordenar processos por ordem de chegada na CPU
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoChegada);

  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  for (int i = 0; i < qtd_processos; i++) {
    int tempo_espera, tempo_resposta, tempo_retorno;
    
    if (i == 0 || copia[i].tempo_chegada > copia[i-1].tempo_termino) {
      copia[i].tempo_termino = copia[i].tempo_chegada + copia[i].duracao_pico;

    } else {
      copia[i].tempo_termino = copia[i-1].tempo_termino + copia[i].duracao_pico;
    }
  
    copia[i].tempo_retorno = copia[i].tempo_termino - copia[i].tempo_chegada;
    copia[i].tempo_espera = copia[i].tempo_termino - copia[i].duracao_pico - copia[i].tempo_chegada;

    if (i == 0) {
      copia[i].tempo_resposta = 0 - copia[i].tempo_chegada;
    } else {
      copia[i].tempo_resposta = copia[i-1].tempo_termino - copia[i].tempo_chegada;
    }

    tempo_retorno_total += copia[i].tempo_retorno;
    tempo_resposta_total += copia[i].tempo_resposta;
    tempo_espera_total += copia[i].tempo_espera;
  }

  float tempo_medio_retorno = (float)tempo_retorno_total / qtd_processos;
  float tempo_medio_resposta = (float)tempo_resposta_total / qtd_processos;
  float tempo_medio_espera = (float)tempo_espera_total / qtd_processos;

  printf("FCFS %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
  free(copia);
}

/**
 * Simula fila de escalonamento SJF
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
 * Informa o tempo médio de retorno, resposta e espera
*/
void simula_sfj(int qtd_processos, struct processo *processos) {
  // Listas auxiliares para o processamento dos processos
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  struct processo *processo_a_executar = NULL; 
  struct filaProcessos *fila_de_prontos = criar_fila_processos(qtd_processos); // inicialmente vazia
  struct filaProcessos *fila_concluidos = criar_fila_processos(qtd_processos); // inicialmente vazia

  // tempos
  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  // contadores
  int qtd_processos_fila_prontos = 0;
  int qtd_processos_concluidos = 0;
  int qtd_processos_nao_concluidos = 0;

  int i, j, k;
  int tempo_atual = 0;

  // Ordena os processos por duração do pico (SJF)
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoPico);

  while (qtd_processos_concluidos < qtd_processos) { // Adicione uma condição de término
    struct processo *processo_a_executar = NULL;

    for (int i = 0; i < qtd_processos; i++) {
      struct processo *processo = &copia[i];

      if (processo->tempo_chegada <= tempo_atual && !buscaElementoNaFila(fila_concluidos, processo)) {
        if (processo_a_executar == NULL || processo->duracao_pico < processo_a_executar->duracao_pico) {
          processo_a_executar = processo;
        }
      }
    }

    if (processo_a_executar != NULL) {      
      processo_a_executar->tempo_resposta = tempo_atual - processo_a_executar->tempo_chegada;
      processo_a_executar->tempo_espera = processo_a_executar->tempo_resposta;
      
      tempo_atual += processo_a_executar->duracao_pico; // Atualize o tempo atual após a execução
      processo_a_executar->tempo_retorno = tempo_atual - processo_a_executar->tempo_chegada;

      enfileirar(fila_concluidos, processo_a_executar);
      qtd_processos_concluidos++;
    } else {
      printf("Não há processos prontos para execução no momento. Aguarde...\n");
      tempo_atual++; // Avance o tempo se não houver processos prontos
    }
  }

  // Calcule os tempos médios de retorno e espera
  float tempo_medio_retorno = 0, tempo_medio_espera = 0, tempo_medio_resposta = 0;

  for (int i = 0; i < qtd_processos; i++) {
    tempo_medio_retorno += copia[i].tempo_retorno;
    tempo_medio_espera += copia[i].tempo_espera;
    tempo_medio_resposta += copia[i].tempo_espera;

  }

  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;
  
  printf("SJF %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
  free(copia);
}

/**
 * Simula fila de escalonamento RR
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
 * @param qq quantum
 * Informa o tempo médio de retorno, resposta e espera
*/

void simula_rr(int qtd_processos, struct processo *processos, int qq) {
  // Listas auxiliares para o processamento dos processos
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  struct filaProcessos *fila_de_prontos = criar_fila_processos(qtd_processos);
  // Inicialização de variáveis de controle
  int tempo_atual = 0;
  int qtd_processos_concluidos = 0;
  int i = 0;
  int j = 0;
  // Ordena os processos por tempo de chegada e duração do pico (Round Robin)
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoChegada);

  while (i < qtd_processos || !(fila_de_prontos->inicio > fila_de_prontos->fim)) {
    if (i < qtd_processos && copia[i].tempo_chegada <= tempo_atual) {
      enfileirar(fila_de_prontos, &copia[i]);
      printf("Adicionado a fila de prontos: processo %d\n", copia[i].id);
      i++;
      continue;
    }

    exibirfilaProcessos(fila_de_prontos);

    if(!(fila_de_prontos->inicio > fila_de_prontos->fim)) {
      struct processo *processo_atual = desenfileirar(fila_de_prontos); // return processo ou null
      j = processo_atual->id-1;
      printf("Processo atual: prc %d\n", processo_atual[i].id);
      printf("j: %d\n", j);

      if(copia[j-1].tempo_resposta == -1) {
        copia[j-1].tempo_resposta = tempo_atual - copia[j-1].tempo_chegada;
      }

      if(copia[j-1].pico_restante > qq) {
        tempo_atual += qq;
        copia[j-1].pico_restante -= qq;

        while (i < qtd_processos && copia[j-1].tempo_chegada <= tempo_atual) {
          enfileirar(fila_de_prontos, &copia[i]);
          i++;
        }

        enfileirar(fila_de_prontos, &copia[j-1]);

      } else {
        tempo_atual += copia[j-1].pico_restante;
        copia[j-1].pico_restante = 0; 
        copia[j-1].tempo_termino = tempo_atual; 
        copia[j-1].tempo_retorno = copia[j-1].tempo_termino - copia[j-1].tempo_chegada; 
        copia[j-1].tempo_espera = copia[j-1].tempo_resposta - copia[j-1].duracao_pico;
        copia[j-1].tempo_resposta = copia[j-1].tempo_termino - copia[j-1].tempo_chegada; 
      }
    } else {
      tempo_atual++;
    }
  }

  // Calcule os tempos médios de retorno e espera
  float tempo_medio_retorno = 0;
  float tempo_medio_espera = 0;
  float tempo_medio_resposta = 0;

  for (int i = 0; i < qtd_processos; i++) {
    tempo_medio_retorno += copia[i].tempo_retorno;
    tempo_medio_espera += copia[i].tempo_espera;
    tempo_medio_resposta += copia[i].tempo_resposta;
  }

  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;

  printf("RR %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);

  // Libere a memória alocada para as filas
  free(fila_de_prontos->processos);
  free(fila_de_prontos);
  free(copia);
}
