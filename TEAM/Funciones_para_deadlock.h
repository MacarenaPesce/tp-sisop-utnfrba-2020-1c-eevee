/*
 * Funciones_para_deadlock.h
 *
 *  Created on: 14 jun. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_PARA_DEADLOCK_H_
#define FUNCIONES_PARA_DEADLOCK_H_

struct pokemones_bloqueados{
	int pid;
	char * pokemon;
};
typedef struct pokemones_bloqueados t_pokemones_bloqueados;

struct consulta_bloqueo{
	int pid;
	char * pokemon;
	int sentencia;
};
typedef struct consulta_bloqueo t_consulta_bloqueo;

struct deadlock{
	int pid;
	char* pokemon_capturado;
	char* pokemon_pedido;
};
typedef struct deadlock t_deadlock;

#endif /* FUNCIONES_PARA_DEADLOCK_H_ */
