/*
 * GameBoy.h
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Contexto_gameboy.h"
#include "Herramientas_gameboy.h"

//struct addrinfo* crear_addrinfo(char * ip, char * puerto);
//int conectarse_a_team(char * ip, char * puerto);

int conectar_a_server(char* ip, char* puerto);

#endif /* GAMEBOY_H_ */
