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


void buddy_funcionamiento(t_mensaje_cola* estructura_mensaje);
void asignar_bloque_BS(t_mensaje_cola* estructura_mensaje, int tamanio_particion);
t_bloque_memoria* encontrar_particion_libre(int tamanio_de_particion); 
void particionar_bloque_buddies(t_bloque_memoria* particion_inicial,t_mensaje_cola* estructura_mensaje, int tamanio_bytes_pot_dos);
t_bloque_memoria* reemplazar_bloque_BS();
void consolidacion_BS(t_bloque_memoria* bloque_borrado);
bool son_buddies(t_bloque_memoria* bloque_anterior, t_bloque_memoria* bloque_siguiente);
void consolidar_bloques_buddies(t_bloque_memoria* bloque_anterior, t_bloque_memoria* bloque_siguiente);


//*****************Auxiliares especificas Buddy System******************************
bool tamanio_potencia_dos(int tamanio_en_bytes);
int numero_potencia_dos(int tamanio_en_bytes);
