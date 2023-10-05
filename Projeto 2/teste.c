#include <stdio.h>
#include <stdlib.h>

struct memoria {
    int qtd_quadros;
    int *referencias;
};

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

    // Aloca memória para o array de referências com base na quantidade de quadros
    mem.referencias = (int *)malloc(mem.qtd_quadros * sizeof(int));
    if (mem.referencias == NULL) {
        fprintf(stderr, "Erro ao alocar memória para referências.\n");
        exit(1);
    }

    // Lê as linhas restantes do arquivo e preenche o array de referências
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        mem.referencias[linha_atual] = atoi(linha);
        linha_atual++;
    }

    return mem;
}

int main() {
    FILE *arquivo;
    arquivo = fopen("entrada.txt", "r"); // Substitua "arquivo.txt" pelo nome do seu arquivo de entrada

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 1;
    }

    struct memoria mem = ler_referencias(arquivo);

    // Exemplo de uso da estrutura preenchida
    printf("Quantidade de quadros: %d\n", mem.qtd_quadros);
    printf("Referências:\n");
    for (int i = 0; i < mem.qtd_quadros; i++) {
        printf("%d\n", mem.referencias[i]);
    }

    // Libera a memória alocada para o array de referências
    free(mem.referencias);

    // Fecha o arquivo
    fclose(arquivo);

    return 0;
}
