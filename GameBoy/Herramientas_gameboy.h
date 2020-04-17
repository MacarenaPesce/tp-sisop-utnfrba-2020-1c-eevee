/*
 * Herramientas_gameboy.h
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#ifndef HERRAMIENTAS_GAMEBOY_H_
#define HERRAMIENTAS_GAMEBOY_H_

#include "Contexto_gameboy.h"

void inicializar_logger();
void inicializar_archivo_de_configuracion();
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));
void obtener_la_ip_del_broker();
void obtener_la_ip_de_team();
void obtener_la_ip_de_gamecard();
void obtener_el_puerto_del_broker();
void obtener_el_puerto_de_team();
void obtener_el_puerto_de_gamecard();
void configurar_signals(void);
void capturar_signal(int signo);
void terminar_gameboy_correctamente();

#endif /* HERRAMIENTAS_GAMEBOY_H_ */
