#include "algoritmos.h"
#include "pagina.h"
#include "contador.h"

//Least Recently Used
void lru(quadro *quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador){
  if (operacao == 'R' || operacao == 'W') //Se é leitura ou escrita
    contador->acessos_mem++; //Adiciona um acesso à memória
  
  int achou_pagina = 0;
  for(int i = 0; i < n_quadros; i++){ //Checa se página está nos quadros
    if(quadros[i].pagina.id_pagina == pagina) { //Se encontrar
      achou_pagina = 1;
      quadros[i].ultimo_acesso = *tempo; //Atualiza último acesso
      quadros[i].pagina.sujo = quadros[i].pagina.sujo || operacao == 'W'; //Se for escrita, suja a página
      (*tempo)++; //Atualiza o tempo
      break; //Sai do loop
    }
  }

  if(achou_pagina == 0) { //Se não está nos quadros
    contador->page_faults++; //Mais um page fault

    int indice_quadro = -1; //Índice do quadro selecionado
    int quadro_ultimo_acesso = 999999999; //Acesso menos recente
    for(int i = 0; i < n_quadros; i++){ //Procura quadro com acesso menos recente
      if(quadros[i].pagina.id_pagina == -1){ //Se encontrou um quadro vazio
        indice_quadro = i; //Seleciona para colocar nele
        break; //Para a procura
      }

      if(quadros[i].ultimo_acesso < quadro_ultimo_acesso){ //Checa se quadro atual foi acessado menos recentemente que o selecionado
        indice_quadro = i; //Se sim, troca seleção
        quadro_ultimo_acesso = quadros[i].ultimo_acesso;
      }
    }

    if (quadros[indice_quadro].pagina.sujo == 1) { //Se página do quadro escolhido está suja
      contador->paginas_sujas++; //Escreve no disco; mais um no contador de páginas sujas
    }

    //Coloca a página no quadro escolhido
    quadros[indice_quadro].pagina.id_pagina = pagina;
    quadros[indice_quadro].pagina.sujo = operacao == 'W'; //Se operação é escrita, página está suja
    quadros[indice_quadro].ultimo_acesso = *tempo;
    (*tempo)++; //Atualiza o tempo
  }
}

//First in First Out
void fifo(quadro *quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador){
  if (operacao == 'R' || operacao == 'W') //Se é leitura ou escrita
    contador->acessos_mem++; //Adiciona um acesso à memória
  
  int achou_pagina = 0;
  for(int i = 0; i < n_quadros; i++){ //Checa se página está nos quadros
    if(quadros[i].pagina.id_pagina == pagina) { //Se encontrar
      achou_pagina = 1;
      quadros[i].pagina.sujo = quadros[i].pagina.sujo || operacao == 'W'; //Se for escrita, suja a página
      break;
    }
  }

  if(achou_pagina == 0) { //Se não está nos quadros
    contador->page_faults++; //Mais um page fault

    int indice_quadro = -1; //Índice do quadro selecionado
    int pag_tempo_adicionada = 999999999; //Adição menos recente
    for(int i = 0; i < n_quadros; i++){ //Procura quadro com adição menos recente
      if(quadros[i].pagina.id_pagina == -1){ //Se encontrou um quadro vazio
        indice_quadro = i; //Seleciona para colocar nele
        break; //Para a procura
      }

      if(quadros[i].pagina.tempo_adicionada < pag_tempo_adicionada){ //Checa se quadro atual foi adicionado menos recentemento que o selecionado
        indice_quadro = i; //Se sim, troca seleção
        pag_tempo_adicionada = quadros[i].pagina.tempo_adicionada;
      }
    }

    if (quadros[indice_quadro].pagina.sujo == 1) { //Se página do quadro escolhido está suja
      contador->paginas_sujas++; //Escreve no disco; mais um no contador de páginas sujas
    }

    //Coloca a página no quadro escolhido
    quadros[indice_quadro].pagina.id_pagina = pagina;
    quadros[indice_quadro].pagina.sujo = operacao == 'W'; //Se operação é escrita, página está suja
    quadros[indice_quadro].pagina.tempo_adicionada = *tempo;
    (*tempo)++; //Atualiza o tempo
  }
}

//Segunda Chance
void segunda_chance(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador){
  if (operacao == 'R' || operacao == 'W') //Se é leitura ou escrita
    contador->acessos_mem++; //Adiciona um acesso à memória
  
  int achou_pagina = 0;
  for(int i = 0; i < n_quadros; i++){ //Checa se página está nos quadros
    if(quadros[i].pagina.id_pagina == pagina) { //Se encontrar
      achou_pagina = 1;
      quadros[i].ultimo_acesso = *tempo; //Atualiza último acesso
      quadros[i].pagina.sujo = quadros[i].pagina.sujo || operacao == 'W'; //Se for escrita, suja a página
      (*tempo)++; //Atualiza o tempo
      break;
    }
  }

  if(achou_pagina == 0) { //Se não está nos quadros
    contador->page_faults++; //Mais um page fault

    int indice_quadro = -1; //Índice do quadro selecionado
    while (1) {
      //Se último acesso é diferente do tempo de adição, bit de referência é 1
      //contador->fila_quadros é o índice inicial atual da lista circular de quadros
      int ref_bit = quadros[contador->fila_quadros].pagina.tempo_adicionada != quadros[contador->fila_quadros].ultimo_acesso;

      if (ref_bit == 0) { //Se o bit de referência é 0
        indice_quadro = contador->fila_quadros; //Coloca a página no quadro
        contador->fila_quadros = (contador->fila_quadros + 1) % n_quadros; //Atualiza o início da lista circular
        break; //Para a procura
      }

      //Se é 1
      quadros[contador->fila_quadros].ultimo_acesso = quadros[contador->fila_quadros].pagina.tempo_adicionada; //Muda o último acesso para que o bit de referência se torne 0
      contador->fila_quadros = (contador->fila_quadros + 1) % n_quadros; //Atualiza o início da lista circular
    }

    if (quadros[indice_quadro].pagina.sujo == 1) { //Se página do quadro escolhido está suja
      contador->paginas_sujas++; //Escreve no disco; mais um no contador de páginas sujas
    }

    //Coloca a página no quadro escolhido
    quadros[indice_quadro].pagina.id_pagina = pagina;
    quadros[indice_quadro].pagina.sujo = operacao == 'W'; //Se operação é escrita, página está suja
    quadros[indice_quadro].pagina.tempo_adicionada = *tempo;
    quadros[indice_quadro].ultimo_acesso = *tempo;
    (*tempo)++; //Atualiza o tempo
  }
}

//Algortimo criado/Aleatório
void aleatorio(quadro* quadros, int n_quadros, int pagina, char operacao, long *tempo, contador_t *contador) {
  if (operacao == 'R' || operacao == 'W') //Se é leitura ou escrita
    contador->acessos_mem++; //Adiciona um acesso à memória
  
  int achou_pagina = 0;
  for(int i = 0; i < n_quadros; i++){ //Checa se página está nos quadros
    if(quadros[i].pagina.id_pagina == pagina) {  //Se encontrar
      achou_pagina = 1;
      quadros[i].pagina.sujo = quadros[i].pagina.sujo || operacao == 'W'; //Se for escrita, suja a página
      break;
    }
  }

  if(achou_pagina == 0) { //Se não está nos quadros
    contador->page_faults++; //Mais um page fault

    int indice_quadro = rand() % n_quadros; //Escolhe um quadro aleatório
    if (quadros[indice_quadro].pagina.sujo == 1) { //Se página do quadro escolhido está suja
      contador->paginas_sujas++; //Escreve no disco; mais um no contador de páginas sujas
    }

    //Coloca a página no quadro escolhido
    quadros[indice_quadro].pagina.id_pagina = pagina;
    quadros[indice_quadro].pagina.sujo = operacao == 'W'; //Se operação é escrita, página está suja
    quadros[indice_quadro].pagina.tempo_adicionada = *tempo;
    (*tempo)++; //Atualiza o tempo
  }
}