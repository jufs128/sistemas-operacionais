#ifndef PAGINA_H
#define PAGINA_H

//Página
typedef struct {
  unsigned id_pagina; //Id calculado
  int sujo; //1 se está suja, 0 se não
  long tempo_adicionada; //Tempo de adição nos quadros
} pagina;

//Quadro
typedef struct {
  pagina pagina; //Página guardada no quadro
  long ultimo_acesso; //Tempo do último acesso ao quadro
} quadro;

#endif