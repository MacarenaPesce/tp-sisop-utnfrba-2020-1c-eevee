/*
 * Funciones_de_conexion.h
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_DE_CONEXION_H_
#define FUNCIONES_DE_CONEXION_H_

#include "Contexto_team.h"

int crear_listen_socket(char*);
int iniciar_servidor();
void hacer_intento_de_reconexion();
void * intento_de_reconexion();
int conectar_broker();
void * atender_nuevo_gameboy(int);

#endif /* FUNCIONES_DE_CONEXION_H_ */
