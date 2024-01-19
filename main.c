#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pagina.h"
#include "algoritmos.h"
#include "contador.h"

//Imprime a tabela dos quadros
void imprimir_quadros(quadro* quadros, int n_quadros) {
  printf("%-11s | %-12s | %-6s | %-10s\n", "endereço", "pagina", "suja", "bit de ref."); 
  for (int i = 0; i < n_quadros; i++) {
    int ref_bit = quadros[i].pagina.tempo_adicionada != quadros[i].ultimo_acesso;
    printf("%-10d | %-12d | %-6d | %-10d\n", i, quadros[i].pagina.id_pagina, quadros[i].pagina.sujo, ref_bit); 
  }
}

//Calcula o id da página
unsigned calcular_pagina(int tamanho_pagina, unsigned addr) {
  unsigned tmp = tamanho_pagina;
  unsigned s = 0;
  while (tmp > 1) {
    tmp = tmp >> 1;
    s++; //Seleciona os s bits menos significativos
  }

  return addr >> s; //Página determinada
}

int main(int argc, char **argv){
  if(argc < 5){ //Se receber menos dos 5 parâmetros obrigatórios
    printf("Parâmetros incorretos. Execute o programa como\n");
    printf("   ./tp2virtual [algoritmo] [arquivo com a sequência de endereços] [tamanho das páginas (em KB)] [tamanho total (em KB)] \n");
    exit(0);
  }

  srand(time(NULL)); //Para obter o tempo de execução 

  long tempo = 0;

  char *algoritmo = argv[1]; //Algoritmo de reposição
  char *nome_arquivo = argv[2]; //Arquivo a ser lido
  int tamanho_pagina = atoi(argv[3]) * 1024; //Tamanho das páginas/dos quadros
  int tamanho_total = atoi(argv[4]) * 1024; //Tamanho da memória
  int n_quadros = tamanho_total / tamanho_pagina; //Número de quadros na memória

  //Contador de estatísticas
  contador_t *contador = (contador_t*) malloc (sizeof(contador_t));
  contador->acessos_mem = 0; //Acessos à memória
  contador->page_faults = 0; //Page faults
  contador->paginas_sujas = 0; //Páginas sujas escritas no disco
  contador->fila_quadros = 0; //Início da fila no Segunda Chance

  //Vetor de quadros
  quadro* quadros = (quadro*) malloc (sizeof(quadro) * n_quadros);
  for(int i = 0; i < n_quadros; i++){
    pagina pagina_vazia = { -1, 0, -1 }; //{id_pagina, sujo, tempo_adicionada}, com valores de quadro vazio
    quadros[i].pagina = pagina_vazia;
    quadros[i].ultimo_acesso = -1;
  }

  unsigned addr; //Endereço da página
  char rw; //Leitura ou escrita
  FILE *arquivo = fopen(nome_arquivo, "r");

  //Impressão do estado inicial dos quadros
  printf("Tabela de quadros antes da execução do algoritmo\n");
  imprimir_quadros(quadros, n_quadros);

  //Inicia contagem do tempo de execução
  clock_t tinicio = clock();

  //Processa todo o arquivo
  while(fscanf(arquivo,"%x %c", &addr, &rw) != EOF){
    unsigned pagina = calcular_pagina(tamanho_pagina, addr); //Calcula o id da página

    if(strcmp(algoritmo, "lru") == 0) //Least Recently Used
      lru(quadros, n_quadros, pagina, rw, &tempo, contador);
    else if(strcmp(algoritmo, "fifo") == 0) //First In First Out
      fifo(quadros, n_quadros, pagina, rw, &tempo, contador);
    else if(strcmp(algoritmo, "2a") == 0) //Segunda Chance
      segunda_chance(quadros, n_quadros, pagina, rw, &tempo, contador);
    else //Aleatório; algoritmo criado
      aleatorio(quadros, n_quadros, pagina, rw, &tempo, contador);
  }

  //Termina contagem de tempo do algoritmo
  clock_t tfim = clock();

  //Impressão do estado final dos quadros
  printf("\nTabela de quadros após a execução do algoritmo\n");
  imprimir_quadros(quadros, n_quadros);

  //Fecha o arquivo e imprime estatísticas obtidas
  fclose(arquivo);
  printf("\nEstatísticas da execução\n");
  printf("Arquivo de entrada: %s\n", nome_arquivo);
  printf("Tamanho da memória: %dKB\n", tamanho_total / 1024);
  printf("Tamanho das páginas: %dKB\n", tamanho_pagina / 1024);
  printf("Técnica de reposição: %s\n", algoritmo);
  printf("Número de acessos na memória: %d\n", contador->acessos_mem);
  printf("Número de page faults: %d\n", contador->page_faults);
  printf("Número de páginas sujas: %d\n", contador->paginas_sujas);
  printf("Tempo de execução: %f\n", (double)(tfim - tinicio) / CLOCKS_PER_SEC);
}