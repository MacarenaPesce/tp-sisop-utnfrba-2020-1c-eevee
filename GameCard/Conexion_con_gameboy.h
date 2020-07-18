/*
 * Conexion_con_gameboy.h
 *
 *  Created on: 2 jul. 2020
 *      Author: utnso
 */

#ifndef CONEXION_CON_GAMEBOY_H_
#define CONEXION_CON_GAMEBOY_H_

#include "Conexion_con_broker.h"

void escuchar_mensajes_entrantes(int);
void * atender_a_gameboy(void *);
void crear_hilo_de_escucha_para_gameboy(int);


#endif /* CONEXION_CON_GAMEBOY_H_ */
