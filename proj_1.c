// Sistemas Operacionais
// Projeto 1
// Samantha Dantas Medeiros @ 25/08/23

#include <stdio.h>
#include <stdlib.h>

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
int compararProcessos(const void *a, const void *b) {
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

void simula_fcfs(int qtd_processos, struct processo *processos) {
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
  // Ordena os processos por duração do pico (SJF)
  qsort(processos, qtd_processos, sizeof(struct processo), compararProcessos);
 
  // tempos
  int tempo_termino[qtd_processos];
  int tempo_total[qtd_processos];
  int tempo_espera_total = 0;
  int tempo_retorno_total = 0;
  int tempo_resposta_total = 0;

  // Listas auxiliares para o processamento dos processos
  struct processo copia_processos[qtd_processos];
  struct processo concluidos[qtd_processos];
  struct processo fila_de_prontos[qtd_processos];


  for (int i = 0; i < qtd_processos; i++) {
    printf("Processo %d chegada: %d pico: %d\n", processos[i].id, processos[i].tempo_chegada, processos[i].duracao_pico);
    if(i == 0) { 
      fila_de_prontos[i] = processos[0];
      tempo_termino[i] =  processos[0].tempo_chegada + processos[0].duracao_pico;
      // calculando tempo de termino, espera, retorno e resposta
      copia_processos[i] = processos[0];
      copia_processos[i].tempo_retorno = processos[i].tempo_termino - processos[i].tempo_chegada;
      copia_processos[i].tempo_termino =  processos[i].tempo_chegada + processos[i].duracao_pico;
      copia_processos[i].tempo_espera = processos[i].tempo_termino - processos[i].duracao_pico - processos[i].tempo_chegada;
      copia_processos[i].tempo_resposta = 0; // primeiro processo
     
      // Adicionando e verificando processos na fila de prontos
      for(int j = 0; j < qtd_processos; j++) {
        if(processos[j].tempo_chegada < processos[i].tempo_termino) {
          int existe_em_prontos = 0;
          for(int k = 0; k < qtd_processos; k++) {
            if(fila_de_prontos[k].id == processos[j].id) {
              existe_em_prontos = 1;
              break;
            }
          }
          // adiciona a fila de prontos
          if (!existe_em_prontos) {
            fila_de_prontos[i] = processos[j];
          }
        }
      }

      // Adicionando e verificando processos na fila de concluídos
      for(int j = 0; j < qtd_processos; j++) {
        int existe_em_concluidos = 0;
        for(int k = 0; k < qtd_processos; k++) {
          if(concluidos[k].id == processos[j].id) {
            existe_em_concluidos = 1;
            break;
          }
        }
        // adiciona a fila de concluídos
        if(!existe_em_concluidos) {
          concluidos[i] = processos[i];
        }
      }
    } else {
      if(i < qtd_processos && fila_de_prontos[i].id == 0 && i >= 0) {
        for(int j = 0; j < qtd_processos; j++) {
          int existe_em_concluidos = 0;
          for(int k = 0; k < qtd_processos; k++) {
            if(concluidos[k].id == processos[j].id) {
              existe_em_concluidos = 1;
              break;
            }
          }
          // adiciona a fila de prontos
          if(!existe_em_concluidos) {
            fila_de_prontos[i] = processos[j];
          }
        }
      }

      struct processo sort_pico[qtd_processos];
      for(int j = 0; j < qtd_processos; j++) {
        sort_pico[j] = fila_de_prontos[j];
      } 

      for(int j = 0; j < qtd_processos; j++) {
        for(int k = 0; k < qtd_processos; k++) {
          if(sort_pico[k].duracao_pico > sort_pico[k+1].duracao_pico) {
            // troca entre processos
            struct processo temp = sort_pico[k];
            sort_pico[k] = sort_pico[k + 1];
            sort_pico[k + 1] = temp;
          }
        }
      }

      struct processo a_executar = sort_pico[0];
      int tempo_termino_anterior = tempo_termino[i - 1];

      if(a_executar.tempo_chegada > tempo_termino_anterior) {
        tempo_termino[i] = a_executar.tempo_chegada + a_executar.duracao_pico;
      } else {
        tempo_termino[i] = tempo_termino_anterior + a_executar.duracao_pico;
      }

      // processos resolvidos
      copia_processos[i] = a_executar;
      copia_processos[i].tempo_termino = tempo_termino[i];
      copia_processos[i].tempo_retorno = tempo_termino[i] - a_executar.tempo_chegada - a_executar.duracao_pico;
      copia_processos[i].tempo_espera = tempo_termino[i] - a_executar.tempo_chegada;
      copia_processos[i].tempo_resposta = tempo_termino_anterior - a_executar.tempo_chegada;

      for(int j = 0; j < qtd_processos; j++) {
        if(processos[j].tempo_chegada <= tempo_termino[i]) {
          int existe_na_fila = 0;
          for(int k = 0; k  <  qtd_processos; k++) {
            if(fila_de_prontos[k].id == processos[j].id) {
              existe_na_fila = 1;
              break;
            }
          }

          int existe_em_concluidos = 0;
          for(int k = 0; k  <  qtd_processos; k++) {
            if(concluidos[k].id == processos[j].id) {
              existe_em_concluidos = 1;
              break;
            }
          }

          if(!existe_na_fila && !existe_em_concluidos) {
            fila_de_prontos[i] = processos[i];
          }

        }
      }

      // remove da fila de prontos
      for(int j = 0; j < qtd_processos; j++) {
        if(fila_de_prontos[j].id == a_executar.id) {
          fila_de_prontos[j].id = 0; 
        }
      }
    }
  }
  float tempo_medio_resposta = 0;
  float tempo_medio_retorno = 0;
  float tempo_medio_espera = 0;
  tempo_medio_resposta = tempo_resposta_total / qtd_processos;
  tempo_medio_retorno = tempo_retorno_total / qtd_processos;
  tempo_medio_espera = tempo_espera_total / qtd_processos;

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