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


//Estructura para reservar la memoria inicial



void asignar_memoria_inicial(int tamanio_en_bytes); //LISTO
t_bloque_memoria* asignar_particion_memoria(t_mensaje_cola* estructura_mensaje); //LISTO

//---ALGORITMOS DE MEMORIA----
t_bloque_memoria* algoritmo_de_memoria(t_mensaje_cola* estructura_mensaje); //LISTO
t_bloque_memoria* particiones_dinamicas( int tamanio_en_bytes, void* mensaje);  //LISTO
t_bloque_memoria* buddy_system( int tamanio_en_bytes, void* mensaje); //EN PROCESO


//********Funciones de Algoritmos de memoria************
t_bloque_memoria* algoritmo_de_particion_libre(int tamanio_msje, int tamanio_parti, void* mensaje); //LISTO
t_bloque_memoria* algoritmo_first_fit(int tamanio_msje, int tamanio_parti, void* mensaje); //LISTO
t_bloque_memoria* algoritmo_best_fit(int tamanio_bytes, int tamanio_parti, void* mensaje); //LISTO
void algoritmo_de_reemplazo(); //LISTO --- falta validar en este y en los otros que son con opciones, si mandan una opcion por el archivo config que no es valida
t_bloque_memoria* algoritmo_fifo(); //LISTO
t_bloque_memoria* algoritmo_lru(); //LISTO 


//-----DUMP-----



//*******************AUXILIARES*******************

/* Las aclaraciones del uso de cada funcion estan en el archivo .c */

bool puede_alojarse(int tamanio_en_bytes);
t_bloque_memoria* particionar_bloque(int tamanio_parti, int indice_nodo_particionar, int tamanio_msje, void* mensaje);
int obtener_indice_particion(t_bloque_memoria* bloque);
int tamanio_a_alojar(int tamanio);

void liberar_bloque_memoria(t_bloque_memoria* bloque); //LISTO

/***Para Particiones dinamicas***/
//Consolidacion
void consolidar(t_bloque_memoria* bloque);
void consolidar_dos_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque);
void consolidar_tres_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque, t_bloque_memoria* tercerBloque);
//Compactacion
void compactar();
//void compactar_payload();

//Auxiliar para LRU y FIFO
uint64_t get_timestamp();


//*****************Auxiliares especificas Buddy System******************************
bool tamanio_potencia_dos(int tamanio_en_bytes);
int numero_potencia_dos(int tamanio_en_bytes);



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

/*
void execute_replacement(uint16_t key, char* value, segment* segment_to_use, int isModified){
	log_info(logger,"Ejecutando algoritmo de reemplazo LRU");
	uint64_t min_time = get_timestamp();
	page* min_page = NULL;
	segment* min_segment = NULL;
	void re_segment(void* aSegment){
		segment* s = (segment*) aSegment;
		void searching_page(void* aPage){
			page*p = (page*) aPage;
			if(p->last_time_used < min_time && !p->isModified){
				min_time = p->last_time_used;
				min_page = p;
				min_segment = s;
			}
		}
		list_iterate(s->page_list,searching_page);
	}
	list_iterate(segmentList,re_segment);

	if(min_segment != NULL && min_page != NULL) {
		log_info(logger,"Se remueve la key %d del segmento %s \n", (int)get_key_from_memory(min_page->frame_num), min_segment->segment_id);
		log_info(output,"[LRU]: Se remueve la key %d del segmento %s", (int)get_key_from_memory(min_page->frame_num), min_segment->segment_id);
		free_frame(min_page->frame_num);
		remove_page_from_segment(min_page,min_segment);
		load_page_to_segment(key, segment_to_use, value, isModified);
	} else {
		log_error(logger, "No se encontro una pagina para reemplazar.");
	}

}*/