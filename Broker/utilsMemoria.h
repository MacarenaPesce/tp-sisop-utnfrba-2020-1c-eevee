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

int calcular_posicion_relativa(t_bloque_memoria* bloque);
void print_memoria(void* _bloque);
bool ordenar_bloques_memoria(void* puntero1, void* puntero2);
void actualizar_lru_de_mensaje(t_mensaje_cola* mensaje);
t_bloque_memoria* buscar_bloque_de_mensaje(t_mensaje_cola* mensaje);

//Auxiliar para LRU y FIFO
uint64_t get_timestamp();
