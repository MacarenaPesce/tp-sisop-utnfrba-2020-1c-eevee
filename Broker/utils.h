#include "Memoria.h"
#include "Herramientas_broker.h"
#include "Contexto_broker.h"
#include "Broker.h"
//#include "utils.h"
#include <Pokebola.h>

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

#include <netinet/in.h>
#include <arpa/inet.h>

/* Las aclaraciones del uso de cada funcion estan en el archivo .c */
t_bloque_memoria* asignar_memoria_inicial(int tamanio_en_bytes, t_list* lista_memoria);
t_bloque_memoria* asignar_particion_memoria( int tamanio_en_bytes);

bool puede_alojarse(int tamanio_en_bytes);
t_bloque_memoria* particionar_bloque(int tamanio_parti, int indice_nodo_particionar, int tamanio_msje);
int obtener_indice_particion(t_bloque_memoria* bloque);
int tamanio_a_alojar(int tamanio);

void liberar_memoria_bloque(t_bloque_memoria* bloque, int indice); //FALTA SETEAR ESE BLOQUE VACIO EN EL LUGAR DEL ANTERIOR
void settear_bloque_liberado_memoria(t_bloque_memoria* bloque, int indice); //EN PROCESO

uint64_t get_timestamp();



//*****************Auxiliares especificas Buddy System******************************
bool tamanio_potencia_dos(int tamanio_en_bytes);
int numero_potencia_dos(int tamanio_en_bytes);

//int calcularBytes (t_packed* paquete);

void  LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria);
