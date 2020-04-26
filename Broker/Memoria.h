/*
 * Herramientas_broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* AsignarMemoriaInicial(int tamaño_en_bytes);
void* AsignarBloqueMemoria(int tamaño_en_bytes);
void LiberarBloqueMemoria(void* bloque_memoria_inicial,void* bloque_a_vaciar,int tamanio_bloqua_a_vaciar);
void* EncontrarBloqueMemoria(void* bloque_memoria_inicial, int tamanio_en_bytes);
void LiberarMemoriaInicial(void* bloque_memoria_inicial)


#endif /* MEMORIA_BROKER_H_ */
