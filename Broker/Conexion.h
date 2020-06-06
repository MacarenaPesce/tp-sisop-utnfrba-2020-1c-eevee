#ifndef HILOS_BROKER_H_
#define HILOS_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <Pokebola.h>


void recibir_mensaje_de_texto(int, int);
void iniciar_servidor(void);
void* esperar_cliente(void* socket_servidor);
void* ObtenerMensaje(void* cliente);
void* esperar_mensajes(void* cliente);

#endif /* HILOS_BROKER_H_ */
