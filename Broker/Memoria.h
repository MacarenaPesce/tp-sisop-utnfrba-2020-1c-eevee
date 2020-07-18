#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_


#include "Herramientas_broker.h"
#include "Contexto_broker.h"
#include "Broker.h"
#include "Buddy.h"
#include "utilsMemoria.h"
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
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>


//--------------- ADMINISTRACION DE MEMORIA-------------------
void asignar_memoria_inicial(int tamanio_en_bytes); 
void asignar_particion_memoria(t_mensaje_cola* estructura_mensaje); 

//---ALGORITMOS DE MEMORIA----
void algoritmo_de_memoria(t_mensaje_cola* estructura_mensaje); 
void particiones_dinamicas( t_mensaje_cola* estructura_mensaje); 
void buddy_system( t_mensaje_cola* estructura_mensaje);

//********Funciones de Algoritmos de memoria************
void algoritmo_de_particion_libre(int tamanio_parti, t_mensaje_cola* estructura_mensaje); 
void algoritmo_first_fit(int tamanio_parti, t_mensaje_cola* estructura_mensaje);
void algoritmo_best_fit(int tamanio_parti, t_mensaje_cola* estructura_mensaje); 
void algoritmo_de_reemplazo(); 
t_bloque_memoria* algoritmo_fifo();
t_bloque_memoria* algoritmo_lru(); 
t_bloque_memoria* obtener_bloque_mas_viejo();
t_bloque_memoria* obtener_bloque_menos_referenciado();

//-----DUMP-----
void dump_memoria();
void escribir_estado_de_memoria(FILE* archivo);
char* crear_string_dump(t_bloque_memoria* bloque,int indice);


//*******************AUXILIARES DE PD*******************

/* Las aclaraciones del uso de cada funcion estan en el archivo .c */
t_bloque_memoria* particionar_bloque(int tamanio_parti, int indice_nodo_particionar, t_mensaje_cola* estructura_mensaje);

/***Para Particiones dinamicas***/
//Consolidacion
void consolidar(t_bloque_memoria* bloque);
void consolidar_dos_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque);

//Compactacion
void compactar();


#endif /* MEMORIA_BROKER_H_ */

/* 
    Paso a Paso:

        1- Broker quiere guardar un mensaje en memoria. Llama a AsignarParticionMemoria(tamañoMensaje)
        2- AsignarParticionMemoria(tamañoMensaje) se encarga de correr llamar al algoritmo de
           particiones y devolver la particion nueva alojada.
        3- El algoritmo de particiones es algoritmo_de_memoria(tamañoMensaje), se encarga de derivar 
           al algoritmo adecuado sea particiones dinamicas o sea Buddy, segun que esta seteado en el
           archivo config
           
        Para alojar particiones:
        Tengo que:
        1- Recorrer la lista de memoria para ver si puedo alojar la particion
        2- Si cabe en una particion libre, tengo que alojar la particion (segun el algoritmo)
            Tener en cuenta si esta vacio, y el tamañominimo de particion seteado por el archivo config
        -------HASTA ACA LISTO-----------
        3- Si no cabe tengo que correr el algoritmo de eliminacion
*/