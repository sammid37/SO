// Sistemas Operacionais
// Projeto 1
// Samantha Dantas Medeiros @ 25/08/23

#include <stdio.h>
#include <stdlib.h>

// macros

// estruturas
struct processo {
  int id; 
  int duracao_pico;
  int tempo_chegada;
};

// protótipos
void menu();
int contar_processos(FILE*);
void simula_fcfs(int qtd_processos, struct processo *processos);
void simula_sfj(int qtd_processos, struct processo *processos);
void simula_rr(int qtd_processos, struct processo *processos, int qq); // qq = 2
void imprime_resultado(int n_processos, float tempo_medio[]); // array de tempos

// main
void main() {
  int qtd_processos = 0, num_processos = 0;
  FILE *arq;
  // arq = fopen("teste2.txt","r");
  arq = fopen("professor.txt","r");


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

  // Imprimindo as informações dos processos lidos
  // for (int i = 0; i < num_processos; i++) {
  //   printf("Processo %d:\n", processos[i].id);
  //   printf("Tempo de Chegada: %d\n", processos[i].tempo_chegada);
  //   printf("Duração de Pico: %d\n\n", processos[i].duracao_pico);
  // }

  simula_fcfs(qtd_processos, processos);
  simula_sfj(qtd_processos, processos);
  simula_rr(qtd_processos, processos, 2);

  // menu();
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
int compararPorDuracaoPico(const void *a, const void *b) {
  const struct processo *processoA = (const struct processo *)a;
  const struct processo *processoB = (const struct processo *)b;
  return processoA->duracao_pico - processoB->duracao_pico;
}

void simula_fcfs(int qtd_processos, struct processo *processos) {
  int tempo_total[qtd_processos];
  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  for (int i = 0; i < qtd_processos; i++) {
    int tempo_espera, tempo_resposta, tempo_retorno;
    
    if (i == 0 || processos[i].tempo_chegada > tempo_total[i]) {
      tempo_total[i] = processos[i].tempo_chegada + processos[i].duracao_pico;
    } else {
      tempo_total[i] = tempo_total[i-1] + processos[i].duracao_pico;
    }
  
    tempo_retorno = tempo_total[i] - processos[i].tempo_chegada;
    tempo_espera = tempo_total[i] - processos[i].duracao_pico - processos[i].tempo_chegada;

    if (i == 0) {
      tempo_resposta = 0 - processos[i].tempo_chegada;
    } else {
      tempo_resposta = tempo_total[i-1] - processos[i].tempo_chegada;
    }

    tempo_retorno_total += tempo_retorno;
    tempo_resposta_total += tempo_resposta;
    tempo_espera_total += tempo_espera;
  }

  float tempo_medio_retorno = (float)tempo_retorno_total / qtd_processos;
  float tempo_medio_resposta = (float)tempo_resposta_total / qtd_processos;
  float tempo_medio_espera = (float)tempo_espera_total / qtd_processos;

  printf("FCFS %.1f %.1f %.1f \n", tempo_medio_retorno, tempo_medio_resposta, tempo_medio_espera);
}

void simula_sfj(int qtd_processos, struct processo *processos) {
  // Ordena os processos por duração do pico (SJF)
  qsort(processos, qtd_processos, sizeof(struct processo), compararPorDuracaoPico);

  int tempo_total[qtd_processos];
  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  for (int i = 0; i < qtd_processos; i++) {
    int tempo_espera, tempo_resposta, tempo_retorno;
    
    if (i == 0 || processos[i].tempo_chegada > tempo_total[i]) {
      tempo_total[i] = processos[i].tempo_chegada + processos[i].duracao_pico;
    } else {
      tempo_total[i] = tempo_total[i-1] + processos[i].duracao_pico;
    }
  
    tempo_retorno = tempo_total[i] - processos[i].tempo_chegada;
    tempo_espera = tempo_total[i] - processos[i].duracao_pico - processos[i].tempo_chegada;

    if (i == 0) {
      tempo_resposta = 0 - processos[i].tempo_chegada;
    } else {
      tempo_resposta = tempo_total[i-1] - processos[i].tempo_chegada;
    }

    printf("Processo %d:\n", processos[i].id);
    printf("Tempo de Resposta: %d\n", tempo_resposta);
    printf("Tempo de Espera: %d\n", tempo_espera);
    printf("Tempo de Retorno: %d\n\n", tempo_retorno);
    printf("tempo total: %d\n\n", tempo_total[i]);

    tempo_retorno_total += tempo_retorno;
    tempo_resposta_total += tempo_resposta;
    tempo_espera_total += tempo_espera;

    /*if (tempo_total < processos[i].tempo_chegada) {
      tempo_total = processos[i].tempo_chegada;
    }

    int tempo_resposta = tempo_total - processos[i].tempo_chegada;
    int tempo_retorno = tempo_resposta + processos[i].duracao_pico;
    int tempo_espera = tempo_retorno - processos[i].duracao_pico;

    tempo_total += processos[i].duracao_pico;

    tempo_resposta_total += tempo_resposta;
    tempo_retorno_total += tempo_retorno;
    tempo_espera_total += tempo_espera;

    */
  }

  float tempo_medio_resposta = (float)tempo_resposta_total / qtd_processos;
  float tempo_medio_retorno = (float)tempo_retorno_total / qtd_processos;
  float tempo_medio_espera = (float)tempo_espera_total / qtd_processos;

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