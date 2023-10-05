//* USAR ESTRUTURA DE PILHA
// Sistemas Operacionais
// Projeto 2 - Memória Virtual
// Samantha Dantas Medeiros @ 05/10/2023

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

void imprime_result(char fila, int result);

int main() {
  FILE *arquivo;
  arquivo = fopen("entrada.txt", "r"); // Substitua "arquivo.txt" pelo nome do seu arquivo de entrada

  if (arquivo == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo.\n");
    return 1;
  }

  struct memoria mem;
  mem = ler_referencias(arquivo);

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
  int pagina_na_memoria = 0; // Flag para verificar se a página existe na memória
  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));

  
  if (quadros == NULL) {
    perror("Erro ao alocar memória para quadros.\nEncerrando...\n");
    exit(1);
  }

  // Inicializando quadros
  for(i = 0; i < mem.qtd_quadros; i++) { quadros[i] = -1; }

  // Percorrendo referências às memórias, adicionando aos quadros e contando faltas de páginas
  for(i = 0; i < mem.qtd_referencias; i++) {
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
  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));
  int ponteiro = 0; // Ponteiro para o quadro mais antigo
  
  int faltas = 0;
  int i = 0, j = 0;

  printf("OTM %d\n", faltas);

}

void simula_lru(struct memoria mem) {
  int *quadros = (int *)malloc(mem.qtd_quadros * sizeof(int));
  int ponteiro = 0; // Ponteiro para o quadro mais antigo
  
  int faltas = 0;
  int i = 0, j = 0;

  printf("LRU %d\n", faltas);
}
