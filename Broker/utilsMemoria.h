#include "Memoria.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include<stdint.h>

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>


/*Funciones Auxiliares en comun entre buddy y PD*/
int tamanio_a_alojar(int tamanio);
int obtener_indice_particion(t_bloque_memoria* bloque);
t_bloque_memoria* crear_bloque_vacio(int tamanio_particion, void* particion);
bool puede_alojarse(int tamanio_en_bytes);
void liberar_bloque_memoria(t_bloque_memoria* bloque);

void* calcular_posicion_relativa(t_bloque_memoria* bloque);

//Auxiliar para LRU y FIFO
uint64_t get_timestamp();


//void  LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria);
