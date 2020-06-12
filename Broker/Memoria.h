#ifndef MEMORIA_BROKER_H_
#define MEMORIA_BROKER_H_


#include "Herramientas_broker.h"
#include "Contexto_broker.h"
#include "Broker.h"
#include "utils.h"
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


//--------------- ADMINISTRACION DE MEMORIA-------------------

//Estructura para reservar la memoria inicial
typedef struct{
    int id_particion;
    int tamanio;
    bool esta_vacio;
    float* payload; //puntero a ubicacion en memoria
}t_bloque_memoria;

void  LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria);

void  LiberarBloqueMemoria(t_bloque_memoria *bloque);


//---ALGORITMOS DE MEMORIA----


t_bloque_memoria* algoritmo_de_memoria( int tamanio_en_bytes);

t_bloque_memoria* particiones_dinamicas( int tamanio_en_bytes);
t_bloque_memoria* buddy_system( int tamanio_en_bytes);

t_bloque_memoria* algoritmo_first_fit(int tamanio); //LISTO

//void algoritmo_de_particion_libre();




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
        3- Si no cabe tengo que correr el algoritmo de eliminacion
*/