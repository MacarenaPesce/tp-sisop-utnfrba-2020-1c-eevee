#ifndef BROKER_H_
#define BROKER_H_

#include "includes.h"
//#include "memoria.h"

int espacio_total;
int espacio_vacio;
int q_bloques_ocupados;
int q_bloques_vacios;
float* primer_bloque;
t_list* lista_memoria;

void recibir_mensaje_de_texto(int, int);
void iniciar_servidor(void);
void esperar_cliente(int);

#endif
