/*
 * Broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "Contexto_broker.h"
#include "Herramientas_broker.h"

void recibir_mensaje_de_texto(int, int);
void iniciar_servidor(void);
void esperar_cliente(int);

#endif
