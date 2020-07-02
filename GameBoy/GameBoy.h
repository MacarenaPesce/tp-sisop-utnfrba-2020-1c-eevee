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
#include "Comunicacion_con_broker.h"
#include "Comunicacion_con_team.h"
#include "Comunicacion_con_gamecard.h"


void consola_broker(char*, char*, char*, char*, char*);
void consola_team(char*, char*, char*, char*);
void consola_gamecard(char*, char*, char*, char*, char*, char*);
void consola_derivar_comando(char*, char*, char*, char*, char*, char*, char*);
int consola_obtener_key_comando(char*);
void ver_que_me_llego_por_argumento(char*, char*, char*, char*, char*, char*, char*);
void mostrar_mensaje_de_error();
void validar_parametros_dos_argumentos(char*, char*);
void validar_parametros_tres_argumentos(char*, char*, char*);
void validar_parametros_cuatro_argumentos(char*, char*, char*, char*);
void validar_parametros_cinco_argumentos(char*, char*, char*, char*, char*);


#endif /* GAMEBOY_H_ */
