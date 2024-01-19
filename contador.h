#ifndef CONTADOR_H
#define CONTADOR_H

//Estrutura que guarda as estatísticas e o início da lista circular (para Segunda Chance)
typedef struct {
  int acessos_mem;
  int page_faults;
  int paginas_sujas;
  int fila_quadros;
} contador_t;

#endif