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
int conectar_broker();


#endif /* FUNCIONES_DE_CONEXION_H_ */
