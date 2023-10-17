// Sistemas Operacionais
// Projeto 2 - Memória Virtual
// Samantha Dantas Medeiros @ 17/10/2023

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct memoria {
  int qtd_quadros;
  int qtd_referencias;
  int *referencias;
};

struct filaMemoria {
  struct memoria **mem;
  int inicio, fim;
  int tamanho;
  int qtd_fila;
};

struct memoria ler_referencias(FILE *arquivo);

void simula_fifo(struct memoria mem);
void simula_otm(struct memoria mem);
void simula_lru(struct memoria mem);

int main() {
  FILE *arquivo;
  arquivo = fopen("entrada.txt", "r"); // Substitua pelo nome do seu arquivo de entrada

  if (arquivo == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo.\n");
    return 1;
  }

  struct memoria mem;
  mem = ler_referencias(arquivo);

  /*printf("Número de quadros: %d\nReferências a memória: %d\n", mem.qtd_quadros, mem.qtd_referencias);
  printf("Referências a memoria:\n");
  for(int i = 0; i < mem.qtd_referencias; i++) {
    printf("%d ", mem.referencias[i]);
  }
  printf("\n");*/

  // Exemplo de uso da estrutura preenchida
  simula_fifo(mem);
  simula_otm(mem);
  simula_lru(mem);

  free(mem.referencias); // Libera a memória alocada para o array de referências
  
  fclose(arquivo); // Fecha o arquivo

  return 0;
}

struct memoria ler_referencias(FILE *arquivo) {
  struct memoria mem;
  char linha[100]; // Tamanho máximo da linha no arquivo
  int linha_atual = 0;

  if (arquivo == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo.\n");
    exit(1);
  }

  // Lê a primeira linha para obter a quantidade de quadros
  if (fgets(linha, sizeof(linha), arquivo) != NULL) {
    mem.qtd_quadros = atoi(linha);
  } else {
    fprintf(stderr, "Erro ao ler a quantidade de quadros.\n");
    exit(1);
  }

  // Conta a quantidade de referências de memória no arquivo
  while (fgets(linha, sizeof(linha), arquivo) != NULL) {
    linha_atual++;
  }

  mem.qtd_referencias = linha_atual;

  // Volta ao início do arquivo para leitura das referências
  fseek(arquivo, 0, SEEK_SET);

  // Pula a primeira linha que já foi lida
  fgets(linha, sizeof(linha), arquivo);

  // Aloca memória para o array de referências com base na quantidade de linhas restantes
  mem.referencias = (int *)malloc(linha_atual * sizeof(int));
  if (mem.referencias == NULL) {
    fprintf(stderr, "Erro ao alocar memória para referências.\n");
    exit(1);
  }

  // Lê as linhas restantes do arquivo e preenche o array de referências
  linha_atual = 0;
  while (fgets(linha, sizeof(linha), arquivo) != NULL) {
    mem.referencias[linha_atual] = atoi(linha);
    linha_atual++;
  }

  return mem;
}

void simula_fifo(struct memoria mem) {
  int faltas = 0;
  int i = 0, j = 0;

  int ponteiro = 0; // Ponteiro para o quadro mais antigo
  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));

  if (quadros == NULL) {
    perror("Erro ao alocar memória para quadros.\nEncerrando...\n");
    exit(1);
  }

  // Inicializando quadros
  for(i = 0; i < mem.qtd_quadros; i++) { quadros[i] = -1; }

  // Percorrendo referências às memórias, adicionando aos quadros e contando faltas de páginas
  for(i = 0; i < mem.qtd_referencias; i++) {
    int pagina_na_memoria = 0; // Flag para verificar se a página existe na memória
    int pagina_referenciada = mem.referencias[i];

    // Percorre os quadros e verifica se a página está na memória
    for(j = 0; j < mem.qtd_quadros; j++) {
      if(quadros[j] == pagina_referenciada) {
        pagina_na_memoria = 1;
        break;
      }
    }

    // Se a página não está na memória, acrescenta aos quadros e contabiliza falta
    if(!pagina_na_memoria) {
      quadros[ponteiro] = pagina_referenciada;
      ponteiro = (ponteiro + 1) % mem.qtd_quadros; // Avança o ponteiro circularmente

      faltas++;
    }
  }

  printf("FIFO %d\n", faltas); 

  free(quadros); // libera a memória alocada
}

void simula_otm(struct memoria mem) {
  int faltas = 0;
  int i = 0, j = 0, k = 0;

  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));

  int ponteiro = 0, aux = 0; // Ponteiro para o quadro mais antigo
  
  if (quadros == NULL) {
    perror("Erro ao alocar memória para quadros e chamadas futuras para o quadro.\nEncerrando...\n");
    exit(1);
  }

  // Inicializando quadros
  for(i = 0; i < mem.qtd_quadros; i++) { quadros[i] = -1; }

  // Percorrendo referências às memórias, adicionando aos quadros e contando faltas de páginas
  for (i = 0; i < mem.qtd_referencias; i++) {
    int pagina_na_memoria = 0;
    int pagina_referenciada = mem.referencias[i];

    // Verifica se a página atual sendo referenciada existe na memória (quadros)
    for (j = 0; j < mem.qtd_quadros; j++) {
      if (quadros[j] == pagina_referenciada) {
        pagina_na_memoria = 1;
        break;
      }
    }

    // Caso a página não esteja na memória...
    if (!pagina_na_memoria) {
      int max_distancia = -1;
      int pagina_a_remover = -1;

      // Encontra a página mais distante no futuro
      for (j = 0; j < mem.qtd_quadros; j++) {
        int encontrou_no_futuro = 0;
        for (k = i + 1; k < mem.qtd_referencias; k++) {
          if (quadros[j] == mem.referencias[k]) {
            encontrou_no_futuro = 1;
            if (k > max_distancia) {
              max_distancia = k;
              pagina_a_remover = j;
            }
            break;
          }
        }
        if (!encontrou_no_futuro) {
          pagina_a_remover = j; // Se não for encontrado no futuro, remove-o imediatamente.
          break;
        }
      }

      quadros[pagina_a_remover] = pagina_referenciada;
      faltas++;
    }
  }

  printf("OTM %d\n", faltas);

  free(quadros); // libera a memória alocada
}

void simula_lru(struct memoria mem) {
  int faltas = 0;
  int i = 0, j = 0, k = 0, l = 0;

  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));
  int *aux = (int *)malloc(mem.qtd_quadros * sizeof(int)); 

  if (quadros == NULL || aux == NULL) {
    perror("Erro ao alocar memória para quadros.\nEncerrando...\n");
    exit(1);
  }

  // Inicializando quadros
  for(i = 0; i < mem.qtd_quadros; i++) { quadros[i] = -1; }

  // Percorrendo referências às memórias, adicionando aos quadros e contando faltas de páginas
  for(i = 0; i < mem.qtd_referencias; i++) {
    int pagina_na_memoria = 0;
    int fluxo_paginas = 0; // página atual foi inserida nos quadros ou uma página foi substituída durante este ciclo

    // Verifica se a página atual sendo referenciada existe na memória (quadros)
    for (j = 0; j < mem.qtd_quadros; j++) {
      if (quadros[j] == mem.referencias[i]) {
        pagina_na_memoria = 1;
        fluxo_paginas = 1;
        break;
      }
    }

    // Caso a página não esteja na memória...
    if (pagina_na_memoria == 0) {
      for (j = 0; j < mem.qtd_quadros; j++) {
        if(quadros[j] == -1) {
          quadros[j] = mem.referencias[i];
          fluxo_paginas = 1;
          faltas++;
          break;
        }
      }
    }

    // Caso a página atual tenha sido inserida ou uma página foi substituída no fluxo
    if(fluxo_paginas == 0) {
      for(j = 0; j < mem.qtd_quadros; j++) {
        aux[j] = 0;
      }

      // Preenchendo aux com 1 para quando a referencia passada estiver no quadro
      for(k = i - 1, l = 1; l <= mem.qtd_quadros - 1; l++, k--) {
        for(j = 0; j < mem.qtd_quadros; j++) {
          if(quadros[j] == mem.referencias[k]) {
            aux[j] = 1;
          }
        }
      }

      // Caso algum índice de aux tenha ficado como zero, essa posição é salva
      int posicao = 0;
      for(j = 0; j < mem.qtd_quadros; j++) {
        if(aux[j] == 0) {
          posicao = j;
          break;
        }
      }
      // substituindo coonteúdo do quadro e contabilizando falta de página
      quadros[posicao] = mem.referencias[i];
      faltas++;
    }
  }

  printf("LRU %d\n", faltas); 

  // libera a memória alocada
  free(quadros); 
  free(aux);
}