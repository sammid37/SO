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

bool contemObjeto(struct memoria array[], int tamanho, int ref); // verifica se um determinado elemento está contido no array

struct filaMemoria *criar_fila(int mem_na_fila);
void exibirfila(struct filaMemoria *fila);
void enfileirar(struct filaProcessos *fila, struct memoria *mem);
struct memoria *desenfileirar(struct filaMemoria *fila);

int simula_fifo(struct memoria *mem);
int simula_otm(struct memoria *mem);
int simula_lru(struct memoria *mem);

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
  printf("Quantidade de quadros: %d\n", mem.qtd_quadros);
  printf("Referências:\n");
  for (int i = 0; i < mem.qtd_referencias; i++) {
    printf("%d\n", mem.referencias[i]);
  }

  // Libera a memória alocada para o array de referências
  free(mem.referencias);
  // Fecha o arquivo
  fclose(arquivo);

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

bool contemObjeto(struct processo array[], int tamanho, int id_procura) {
  for (int i = 0; i < tamanho; i++) {
    if (array[i].id == id_procura) {
      return true; 
    }
  }
  return false; 
}

int simula_fifo(struct memoria *mem) {
  int faltas = 0, ref_cont = 0;
  int i = 0, j = 0;
  int **ref_atual = mem->referencias;

  struct filaMemoria fila = criar_fila(mem->qtd_quadros);

  while(ref_cont < mem->qtd_referencias) {
    /**
     * todo:
     * [] verificar se já está na PILHA e a PILHA não está cheia -> empilha (faltas++)
     * [] se a fila estiver cheia e o elemento atual não for igual ao que está na fila -> desempilha o mais antigo
     * []
    */
    if(fila.qtd_fila < fila.tamanho && !contemObjeto(mem, fila.tamanho, ref_atual)) {
      enfileirar(fila, ref_atual);
      faltas++;
    } 

    ref_atual = mem->referencias[i];
    ref_cont++;
  }
  return faltas;
}