#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include <stdio.h>
#include <stdlib.h>

#include "pagina.h"
#include "contador.h"

//Least Recently Used
void lru(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador);
//First in First out
void fifo(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador);
//Segunda Chance
void segunda_chance(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador);
//Algoritmo criado/Aleatório
void aleatorio(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador);

#endif