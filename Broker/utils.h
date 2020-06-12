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
t_bloque_memoria* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria);
t_bloque_memoria* AsignarParticionMemoria( int tamanio_en_bytes);
bool puedeAlojarse(int tamanio_en_bytes);
t_bloque_memoria* particionarBloque(int tamanio, int indice_nodo_particionar);
int obtenerIndiceParticion(t_bloque_memoria* bloque);


//*****************Auxiliares especificas Buddy System******************************
bool tamanio_potencia_dos(int tamanio_en_bytes);
int numero_potencia_dos(int tamanio_en_bytes);

//int calcularBytes (t_packed* paquete);