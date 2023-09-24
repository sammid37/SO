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
};

// protótipos
int contar_processos(FILE*);
int compararProcessosTempo(const void *a, const void *b); // compara pelo tempo
int compararProcessosTempoPico(const void *a, const void *b); // compara pelo tempo e duracao do pico

bool contemObjeto(struct processo array[], int tamanho, int id_procura); // verifica se um determinado elemento está contido no array

void simula_fcfs(int qtd_processos, struct processo *processos);
void simula_sfj(int qtd_processos, struct processo *processos);
void simula_rr(int qtd_processos, struct processo *processos, int qq); // qq = 2
void imprime_resultado(int n_processos, float tempo_medio[]); // array de tempos

struct filaProcessos *criar_fila_processos(int processos_na_fila);
struct processo *obterUltimoConcluido(struct filaProcessos *fila_concluidos);


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
  // printf("Foram encontrados %d processos no arquivo.\n", qtd_processos);

  // Retorna ao início do arquivo para ler os processos
  fseek(arq, 0, SEEK_SET);

  // Armazena dados em um array struct processo (alocado dinamicamente)
  struct processo *processos = malloc(qtd_processos * sizeof(struct processo));
  while (fscanf(arq, "%d %d", &processos[num_processos].tempo_chegada, &processos[num_processos].duracao_pico) == 2) {
    processos[num_processos].id = num_processos + 1;
    num_processos++;
  }

  simula_fcfs(qtd_processos, processos);
  simula_sfj(qtd_processos, processos);
  simula_rr(qtd_processos, processos, 2);

  free(processos);
  fclose(arq);
}

// implementação de funções
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

struct filaProcessos *criar_fila_processos(int processos_na_fila) {
  struct filaProcessos *fila = (struct filaProcessos *)malloc(sizeof(struct filaProcessos));
  fila->processos = (struct processo **)malloc(sizeof(struct processo *) * processos_na_fila);
  fila->inicio = 0;
  fila->fim = -1;
  fila->tamanho = processos_na_fila;

  return fila;
}

struct processo *obterUltimoConcluido(struct filaProcessos *fila_concluidos) {
  if (fila_concluidos->inicio <= fila_concluidos->fim) {
      return fila_concluidos->processos[fila_concluidos->fim];
  } else {
      return NULL; // A fila de concluídos está vazia
  }
}

// Função para enfileirar um processo na fila ready
void enfileirar(struct filaProcessos *fila, struct processo *processo) {
  if (fila->fim == fila->tamanho - 1) {
    // A fila está cheia, não podemos enfileirar mais processos
    printf("Erro: a fila está cheia!\n");
    exit(1);
  }
  fila->fim++;
  fila->processos[fila->fim] = processo;
}

int fila_vazia(struct filaProcessos *fila) {
  return fila->inicio > fila->fim;
}

void exibirfilaProcessos(struct filaProcessos *fila) {
  if (fila->inicio > fila->fim) {
    printf("Fila de prontos vazia.\n");
  } else {
    printf("Processos na fila de prontos:\n");
    for (int i = fila->inicio; i <= fila->fim; i++) {
      struct processo *processo = fila->processos[i];
      printf("ID: %d, Tempo de Chegada: %d, Duração do Pico: %d\n", processo->id, processo->tempo_chegada, processo->duracao_pico);
    }
  }
}
// Função para desenfileirar um processo da fila ready
struct processo *desenfileirar(struct filaProcessos *fila) {
  if (fila->inicio > fila->fim) {
    // A fila está vazia, não podemos desenfileirar
    return NULL;
  }
  struct processo *processo = fila->processos[fila->inicio];
  fila->inicio++;
  return processo;
}

bool buscaElementoNaFila(struct filaProcessos *fila, struct processo *elemento) {
  for (int i = fila->inicio; i <= fila->fim; i++) {
    if (fila->processos[i] == elemento) {
      return true; // Elemento encontrado na fila
    }
  }
  return false; // Elemento não encontrado na fila
}


//************************************* Implementação de filas de processos 
void simula_fcfs(int qtd_processos, struct processo *processos) {
  // Ordenar processos por ordem de chegada na CPU
  qsort(processos, qtd_processos, sizeof(struct processo), compararProcessosTempoChegada);

  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  for (int i = 0; i < qtd_processos; i++) {
    int tempo_espera, tempo_resposta, tempo_retorno;
    
    if (i == 0 || processos[i].tempo_chegada > processos[i-1].tempo_termino) {
      //tempo_total[i] = processos[i].tempo_chegada + processos[i].duracao_pico;
      processos[i].tempo_termino = processos[i].tempo_chegada + processos[i].duracao_pico;

    } else {
      processos[i].tempo_termino = processos[i-1].tempo_termino + processos[i].duracao_pico;
    }
  
    processos[i].tempo_retorno = processos[i].tempo_termino - processos[i].tempo_chegada;
    processos[i].tempo_espera = processos[i].tempo_termino - processos[i].duracao_pico - processos[i].tempo_chegada;

    if (i == 0) {
      processos[i].tempo_resposta = 0 - processos[i].tempo_chegada;
    } else {
      processos[i].tempo_resposta = processos[i-1].tempo_termino - processos[i].tempo_chegada;
    }

    tempo_retorno_total += processos[i].tempo_retorno;
    tempo_resposta_total += processos[i].tempo_resposta;
    tempo_espera_total += processos[i].tempo_espera;
  }

  float tempo_medio_retorno = (float)tempo_retorno_total / qtd_processos;
  float tempo_medio_resposta = (float)tempo_resposta_total / qtd_processos;
  float tempo_medio_espera = (float)tempo_espera_total / qtd_processos;

  printf("FCFS %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
}

void simula_sfj(int qtd_processos, struct processo *processos) {
  // Listas auxiliares para o processamento dos processos
  struct processo copia_processos[qtd_processos];
  struct processo *processo_a_executar = NULL; 
  struct filaProcessos *fila_de_prontos = criar_fila_processos(qtd_processos); // inicialmente vazia
  struct filaProcessos *fila_concluidos = criar_fila_processos(qtd_processos); // inicialmente vazia

  // tempos
  int tempo_termino[qtd_processos];
  int tempo_total[qtd_processos];
  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  // contadores
  int qtd_processos_fila_prontos = 0;
  int qtd_processos_concluidos = 0;
  int qtd_processos_nao_concluidos = 0;
  int qtd_copia_processos = 0;

  int i, j, k;
  int tempo_atual = 0;

  // Ordena os processos por duração do pico (SJF)
  qsort(processos, qtd_processos, sizeof(struct processo), compararProcessosTempoPico);

  while (qtd_processos_concluidos < qtd_processos) { // Adicione uma condição de término
    struct processo *processo_a_executar = NULL;

    for (int i = 0; i < qtd_processos; i++) {
      struct processo *processo = &processos[i];

      if (processo->tempo_chegada <= tempo_atual && !buscaElementoNaFila(fila_concluidos, processo)) {
        if (processo_a_executar == NULL || processo->duracao_pico < processo_a_executar->duracao_pico) {
          processo_a_executar = processo;
        }
      }
    }

    if (processo_a_executar != NULL) {
      printf("Próximo processo a executar: %d\n", processo_a_executar->id);
      
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
    tempo_medio_retorno += processos[i].tempo_retorno;
    tempo_medio_espera += processos[i].tempo_espera;
    tempo_medio_resposta += processos[i].tempo_espera;

  }

  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;
  
  printf("SJF %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
}


void simula_rr(int qtd_processos, struct processo *processos, int qq) {
    int tempo_total = 0;
    int tempo_espera_total = 0;
    int tempo_retorno_total = 0;
    int tempo_resposta_total = 0;
    int *tempo_restante = (int *)malloc(sizeof(int) * qtd_processos);

    // Inicializa o tempo restante para cada processo
    for (int i = 0; i < qtd_processos; i++) {
      tempo_restante[i] = processos[i].duracao_pico;
    }

    int concluidos = 0; // Número de processos concluídos

    while (concluidos < qtd_processos) {
      for (int i = 0; i < qtd_processos; i++) {
        if (tempo_restante[i] > 0) {
          // Processa o processo atual com o quantum
          if (tempo_restante[i] <= qq) {
            // O processo é concluído
            tempo_total += tempo_restante[i];
            tempo_resposta_total += tempo_total - processos[i].tempo_chegada;
            tempo_retorno_total += tempo_total - processos[i].tempo_chegada;
            tempo_espera_total += tempo_total - processos[i].tempo_chegada - processos[i].duracao_pico;
            tempo_restante[i] = 0;
            concluidos++;
          } else {
            // O quantum expira, mas o processo ainda não está concluído
            tempo_total += qq;
            tempo_restante[i] -= qq;
          }
        }
      }
    }

    free(tempo_restante);

    float tempo_medio_resposta = (float)tempo_resposta_total / qtd_processos;
    float tempo_medio_retorno = (float)tempo_retorno_total / qtd_processos;
    float tempo_medio_espera = (float)tempo_espera_total / qtd_processos;

    printf("RR %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
}