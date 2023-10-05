// Sistemas Operacionais
// Projeto 1 - Escalonador de Processos
// Samantha Dantas Medeiros @ 24/09/2023

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define quantum 2

struct processo { 
  int id; 
  int tempo_chegada, duracao_pico; 
  int pico_restante, tempo_termino; 
  int tempo_retorno, tempo_resposta, tempo_espera; 
};

int contar_processos(FILE*);
int compararProcessosTempo(const void *a, const void *b); // compara pelo tempo
int compararProcessosTempoPico(const void *a, const void *b); // compara pelo tempo e duracao do pico

struct processo* copiarProcessos(int qtd_processos, const struct processo *processos);

void simula_fcfs(int qtd_processos, struct processo *processos);
void simula_sfj(int qtd_processos, struct processo *processos);
void simula_rr(int qtd_processos, struct processo *processos, int qq); // qq = 2

struct filaProcessos { 
  struct processo **processos;
  int inicio, fim;
  int tamanho;
  int qtd_fila;
};

struct filaProcessos *criar_fila_processos(int processos_na_fila);
void exibirfilaProcessos(struct filaProcessos *fila);
void enfileirar(struct filaProcessos *fila, struct processo *processo);
struct processo *desenfileirar(struct filaProcessos *fila);

void main() {
  int qtd_processos = 0, i = 0;
  FILE *arq;
  arq = fopen("professor.txt","r");

  // Verificando se a solicitação de leitura do arquivo pode ser atendida 
  if(arq == NULL) {
    printf("Não foi possível completar a solicitação!\nEncerrando...\n");
    exit(1);
  }

  qtd_processos = contar_processos(arq);

  fseek(arq, 0, SEEK_SET); // Retorna ao início do arquivo para ler os processos

  // Armazena dados em um array struct processo (alocado dinamicamente) os processos encontrados no arquivo
  struct processo *processos = malloc(qtd_processos * sizeof(struct processo));
  while (fscanf(arq, "%d %d", &processos[i].tempo_chegada, &processos[i].duracao_pico) == 2) {
    processos[i].id = i;
    processos[i].pico_restante = processos[i].duracao_pico;

    // inicializando propriedades
    processos[i].tempo_resposta = -1;
    processos[i].tempo_retorno = -1;
    processos[i].tempo_espera = -1;

    i++;
  }

  // Inicializando simulações
  simula_fcfs(qtd_processos, processos);
  simula_sfj(qtd_processos, processos);
  simula_rr(qtd_processos, processos, quantum);

  // Libera memória
  free(processos);
  fclose(arq);
}

/**
 * Conta a quantidade de linhas do arquivo de entrada, correspondendo ao número de processos
 * @param arquivo arquivo de entrada
 * @return a quantidade de processos no arquivo
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

/**
 * Compara processos pelo tempo de chegada e a duração de pico
 * 
 * @param a
 * @param b
 * @return
*/
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

/**
 * Compara processos pelo tempo de chegada
 * 
 * @param a
 * @param b
 * @returns
*/
int compararProcessosTempoChegada(const void *a, const void *b) {
  const struct processo *processoA = (const struct processo *)a;
  const struct processo *processoB = (const struct processo *)b;
  // Primeiro, compara pelo tempo de chegada
  if (processoA->tempo_chegada < processoB->tempo_chegada) return -1;
  if (processoA->tempo_chegada > processoB->tempo_chegada) return 1;

  // Se ambos forem iguais, mantenha a ordem original
  return 0;
}

/**
 * Inicializa uma fila circular vazia que contém processos
 * 
 * @param processos_na_fila
 * @return a fila inicializada
*/
struct filaProcessos *criar_fila_processos(int processos_na_fila) {
  struct filaProcessos *fila = (struct filaProcessos *)malloc(sizeof(struct filaProcessos));

  fila->processos = (struct processo **)malloc(sizeof(struct processo *) * processos_na_fila);
  fila->inicio = 0;
  fila->fim = -1;

  fila->tamanho = processos_na_fila;
  fila->qtd_fila = 0;

  return fila;
}

/**
 * Adiciona um novo processo a fila circular de processos
 * 
 * @param fila a qual deseja adicionar um novo elemento
 * @param processo elemento que deseja inserir na fila
*/
void enfileirar(struct filaProcessos *fila, struct processo *processo) {
  if (fila->qtd_fila == fila->tamanho) {
    // A fila está cheia, não podemos enfileirar mais processos
    printf("Erro: a fila está cheia!\n");
    exit(1);
  }
  fila->fim = (fila->fim + 1) % fila->tamanho; // Avançar para o próximo índice circularmente
  fila->processos[fila->fim] = processo;
  fila->qtd_fila++;
}

/**
 * Remove o processo que está no início da fila circular de processos
 * 
 * @param fila a qual deseja remover o primeiro elemento
*/
struct processo *desenfileirar(struct filaProcessos *fila) {
  if (fila->qtd_fila == 0) {
    // A fila está vazia, não podemos desenfileirar
    return NULL;
  }
  struct processo *processo = fila->processos[fila->inicio];
  fila->inicio = (fila->inicio + 1) % fila->tamanho; // Avançar para o próximo índice circularmente
  fila->qtd_fila--;
  return processo;
}

/**
 * Exibe os elementos contidos na fila
 * 
 * @param fila a qual deseja acessar os elementos
*/
void exibirfilaProcessos(struct filaProcessos *fila) {
  if (fila->inicio == -1) {
    printf("Fila vazia.\n");
    return;
  }

  int i = fila->inicio;

  do {
    printf("ID: %d, Tempo de Chegada: %d, Duração do Pico: %d\n",
           fila->processos[i]->id, fila->processos[i]->tempo_chegada,
           fila->processos[i]->duracao_pico);
    
    i = (i + 1) % fila->tamanho;
  } while (i != (fila->fim + 1) % fila->tamanho);
}

/**
 * Verifica se determinado processo já foi inserido na fila circular
 * 
 * @param fila a qual deseja realizar a busca
 * @param elemento (processo) que deseja encontrar
 * @return se o elemento está presente ou não na fila circular escolhida
*/
bool buscaElementoNaFila(struct filaProcessos *fila, struct processo *elemento) {
  for (int i = fila->inicio; i <= fila->fim; i++) {
    if (fila->processos[i] == elemento) {
      return true; // Elemento encontrado na fila
    }
  }
  return false; // Elemento não encontrado na fila
}

/**
 * Realiza uma cópia dos processos encontrados no arquivo de entrada 
 * 
 * @param qtd_processos que serão copiados
 * @param processos originais que serão copiados
 * @return a copia dos processos no arquivo de entrada
*/
struct processo* copiarProcessos(int qtd_processos, const struct processo *processos) {
  // Criar uma nova estrutura para armazenar a cópia dos processos
  struct processo *copia_processos = (struct processo *)malloc(qtd_processos * sizeof(struct processo));
  
  if (copia_processos == NULL) {
      perror("Erro ao alocar memória para a cópia dos processos");
      exit(1);
  }

  for (int i = 0; i < qtd_processos; i++) {
      copia_processos[i] = processos[i];
  }

  return copia_processos;
}

/**
 * Simula fila de escalonamento FCFS e informa o tempo médio de retorno, resposta e espera
 * 
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
*/
void simula_fcfs(int qtd_processos, struct processo *processos) {
  // Ordenar processos por ordem de chegada na CPU
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoChegada);

  // Tempos e contadores
  float tempo_medio_retorno = 0, tempo_medio_espera = 0, tempo_medio_resposta = 0;
  
  int i = 0;

  for (i = 0; i < qtd_processos; i++) {
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

    tempo_medio_retorno += copia[i].tempo_retorno;
    tempo_medio_espera += copia[i].tempo_espera;
    tempo_medio_resposta += copia[i].tempo_resposta;
  }

  // Calculando os tempos médios de retorno resposta e espera
  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;

  printf("FCFS %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);

  // Libera a memória alocada
  free(copia);
}

/**
 * Simula fila de escalonamento SJF e informa o tempo médio de retorno, resposta e espera
 * 
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
*/
void simula_sfj(int qtd_processos, struct processo *processos) {
  // Filas auxiliares para o processamento dos processos, alocadas dinamicamente
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  struct processo *processo_a_executar = NULL; 
  struct filaProcessos *fila_de_prontos = criar_fila_processos(qtd_processos); // inicialmente vazia
  struct filaProcessos *fila_concluidos = criar_fila_processos(qtd_processos); // inicialmente vazia

  // Tempos e contadores
  float tempo_medio_retorno = 0, tempo_medio_espera = 0, tempo_medio_resposta = 0;

  int tempo_atual = 0;

  int i = 0, j = 0; 

  // Ordena os processos por duração do pico (SJF)
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoPico);

  while (i < qtd_processos) { // Adicione uma condição de término
    struct processo *processo_a_executar = NULL;

    // Define o próximo processo a ser executado
    for (j = 0; j < qtd_processos; j++) {
      struct processo *processo = &copia[j];

      /* Será o processo atual(j) será o próximo processo a ser executado se:
      - seu tempo de chegada <= tempo atual
      - se anda não estiver na fila de concluidos
      */
      if (processo->tempo_chegada <= tempo_atual && !buscaElementoNaFila(fila_concluidos, processo)) {
        /* Com isso, ele é definido caso:
        - ainda não tenha sido definido
        - ou a duração do pico é menor que a do processo atual em relação ao último processo executado
        */
        if (processo_a_executar == NULL || processo->duracao_pico < processo_a_executar->duracao_pico) {
          processo_a_executar = processo;
        }
      }
    }

    // Calcula os tempos de resposta, espera e retorno do processo em execução e o adiciona a fila de concluídos
    if (processo_a_executar != NULL) {      
      // Caso seja a primeira execução, calcula o tempo de resposta
      if(processo_a_executar->tempo_resposta == -1) {
        processo_a_executar->tempo_resposta = tempo_atual - processo_a_executar->tempo_chegada;
      }

      processo_a_executar->tempo_espera = processo_a_executar->tempo_resposta;
      
      tempo_atual += processo_a_executar->duracao_pico; // Atualize o tempo atual após a execução
      processo_a_executar->tempo_retorno = tempo_atual - processo_a_executar->tempo_chegada;

      enfileirar(fila_concluidos, processo_a_executar);
      i++;
    } else {
      tempo_atual++; // Caso não haja um processo em execução, o tempo é incrementado
    }
  }

  // Calculando os tempos médios de retorno resposta e espera
  for (i = 0; i < qtd_processos; i++) {
    tempo_medio_retorno += copia[i].tempo_retorno;
    tempo_medio_espera += copia[i].tempo_espera;
    tempo_medio_resposta += copia[i].tempo_espera;
  }

  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;
  
  printf("SJF %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);

  // Libera a memória alocada
  free(fila_de_prontos->processos);
  free(fila_de_prontos);
  free(fila_concluidos->processos);
  free(fila_concluidos);
  free(copia);
}

/**
 * Simula fila de escalonamento RR e informa o tempo médio de retorno, resposta e espera
 * @param qtd_processos o número de processos lidos do arquivo de entrada
 * @param processos os processos obtidos do arquivo de entrada
 * @param qq quantum
*/
void simula_rr(int qtd_processos, struct processo *processos, int qq) {
  // Filas auxiliares para o processamento dos processos, alocadas dinamicamente
  struct processo *copia = copiarProcessos(qtd_processos, processos);
  struct filaProcessos *fila_de_prontos = criar_fila_processos(qtd_processos);

  // Tempos e contadores
  float tempo_medio_retorno = 0, tempo_medio_resposta = 0, tempo_medio_espera = 0;

  int tempo_atual = 0;

  int i = 0, j = 0;

  // Ordena os processos por tempo de chegada e duração do pico (Round Robin)
  qsort(copia, qtd_processos, sizeof(struct processo), compararProcessosTempoChegada);

  // Enquanto a lista circular não estiver vazia
  while (i < qtd_processos || !(fila_de_prontos->qtd_fila == 0)) {
    /* Adiciona o processo a lista de prontos se:
    - não for o último 
    - e o tempo de chegada <= tempo atual
    */
    if (i < qtd_processos && copia[i].tempo_chegada <= tempo_atual) {
      enfileirar(fila_de_prontos, &copia[i]);
      // Interrompe a execução da iteração atual e passa para a próxima iteração imediatamente
      i++; 
      continue;
    }

    // Se a fila de prontos não estiver vazia
    if(!(fila_de_prontos->qtd_fila == 0)) {
      // Encontra o próximo processo a executar (início da fila circular)
      struct processo *processo_atual = desenfileirar(fila_de_prontos); // return processo ou null
      j = processo_atual->id;
    
      // Caso seja a primeira execução, calcula o tempo de resposta
      if(copia[j].tempo_resposta == -1) {
        copia[j].tempo_resposta = tempo_atual - copia[j].tempo_chegada;
      }

      // Verifica se o tempo restante é maior que o quantum (ou seja, se não terminou de executar)
      if(copia[j].pico_restante > qq) {
        // Atualiza o tempo atual e decrementa o tempo restante
        tempo_atual += qq; 
        copia[j].pico_restante -= qq;

        // Enfileira um novo processo na fila de prontos
        while (i < qtd_processos && copia[i].tempo_chegada <= tempo_atual) {
          enfileirar(fila_de_prontos, &copia[i]);
          i++;
        }

        enfileirar(fila_de_prontos, &copia[j]);
      
      } else {
        // Caso o processo já tenha sido concluído, atualiza os tempos
        tempo_atual += copia[j].pico_restante;
        copia[j].pico_restante = 0; 
        copia[j].tempo_termino = tempo_atual; 
        copia[j].tempo_retorno = copia[j].tempo_termino - copia[j].tempo_chegada; 
        copia[j].tempo_espera = copia[j].tempo_retorno - copia[j].duracao_pico;
      }
    } else {
      tempo_atual++; // Caso não haja um processo em execução, o tempo é incrementado
    }
  }

  // Calculando os tempos médios de retorno resposta e espera
  for (int i = 0; i < qtd_processos; i++) {
    tempo_medio_retorno += copia[i].tempo_retorno;
    tempo_medio_espera += copia[i].tempo_espera;
    tempo_medio_resposta += copia[i].tempo_resposta;
  }

  tempo_medio_retorno /= qtd_processos;
  tempo_medio_espera /= qtd_processos;
  tempo_medio_resposta /= qtd_processos;

  printf("RR %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);

  // Libera a memória alocada
  free(fila_de_prontos->processos);
  free(fila_de_prontos);
  free(copia);
}
