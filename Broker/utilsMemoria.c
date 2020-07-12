#include "utilsMemoria.h"

/*
int calcularBytes (t_packed* paquete){ 

    int bytes = 0;

	//Calculo los bytes segun que operacion tiene mi paquete

	//NO ESTOY SEGURA SI ELIMINAR PAQUETE ESTA BIEN

    switch(paquete->cola_de_mensajes){
			case COLA_NEW_POKEMON:
				puts(" Recibi un NEW POKEMON");
				
                	t_new_pokemon* pkmn;
					pkmn =(t_new_pokemon*)malloc(sizeof(t_new_pokemon));

					// Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);

				//4 bytes por cada uint_32, y tengo 4 uint_32 -> 4 * 4 = 16 
				//mas el largo del nombre del pokemon

                bytes = 16 + (strlen(pkmn->pokemon));

				free(pkmn);

				return bytes;
			case COLA_LOCALIZED_POKEMON:
				puts(" Recibi un LOCALIZED POKEMON");

					t_localized_pokemon* pkmn;
					pkmn =(t_localized_pokemon*)malloc(sizeof(t_localized_pokemon));

					// Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);
				
				//tengo 8 uint_32 -> 4 * 8 = 32
				//mas el largo del nombre por pokemon

                bytes = 32 + (strlen(pkmn->pokemon));

				free(pkmn);

				return bytes;
			case COLA_GET_POKEMON:
				puts(" Recibi un GET POKEMON");

					t_get_pokemon* pkmn;
					pkmn =(t_get_pokemon*)malloc(sizeof(t_get_pokemon));

					// Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);
                
				//tengo 1 sólo uint_32 -> 4 bytes
				//mas el largo del nombre del pokemon

				bytes = 4 + (strlen(pkmn->pokemon));

				free(pkmn);

				return bytes;
			case COLA_APPEARED_POKEMON:
				puts(" Recibi un APPEARED POKEMON");

                	t_appeared_pokemon* pkmn;
					pkmn =(t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));

					// Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);
                
				//tengo 3 uint_32 -> 4 * 3 = 12
				//mas el largo del nombre del pokemon

				bytes = 12 + (strlen(pkmn->pokemon));

				free(pkmn);

				return bytes;
			case COLA_CATCH_POKEMON:
				puts(" Recibi un CATCH POKEMON");

                	t_catch_pokemon* pkmn;
					pkmn =(t_catch_pokemon*)malloc(sizeof(t_catch_pokemon));

					// Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);
                
				//tengo 3 uint_32 -> 4 * 3 = 12
				//mas el largo del nombre del pokemon

				bytes = 12 + (strlen(pkmn->pokemon));

				free(pkmn);

				return bytes;
			case COLA_CAUGHT_POKEMON:
				puts(" Recibi un CAUGHT POKEMON");

					/*
                	t_caught_pokemon* pkmn;
					pkmn =(t_caught_pokemon*)malloc(sizeof(t_caught_pokemon));

					//Apunto a los datos del mensaje
					pkmn = paquete->mensaje;

					//Libero la memoria del paquete
					eliminar_mensaje(paquete);
					
                
				//tengo 1 solo uint_32 -> 4 bytes
                
				bytes = 4;

				free(pkmn);

				return bytes;
			default:
				break;
		}


    return bytes;
}*/



//void LiberarMemoriaInicial(void* bloque_memoria_inicial,t_list* lista_memoria){
    
    /* Libero la memoria inicial */
    //free(bloque_memoria_inicial);

    /* Destruyo la lista y su contenido */
    //list_destroy_and_destroy_elements(lista_memoria);
    //return;
//}

/*
    - Hay que modificar la compactacion, si esta seteada en -1 , no se compacta nunca y se sigue buscando, 
    se compacta unicamente cuando no haya particiones ocupadas, pero con la consolidacion no hace falta compactar

    - Hacer dump 
*/

/*Me fijo si el tamaño de mi mensaje a guardar, es menor que el minimo tamaño de particion*/
int tamanio_a_alojar(int tamanio){

	int tamanio_final = tamanio_minimo_particion; //tamanio_minimo_particion es variable global

	if(tamanio > tamanio_minimo_particion){ 
		tamanio_final = tamanio;
	}   

	return tamanio_final;

}



/*Obtengo el indice de un determinado, recorriendo toda la lista y comparando los payload*/
int obtener_indice_particion(t_bloque_memoria* bloque){

	int indice=0;

    bool buscar_bloque(void* _bloque){

        t_bloque_memoria* bloque_memoria = (t_bloque_memoria*) _bloque;        

        if(bloque_memoria->esta_vacio) {
            indice++;
            return false;
        }

        if(bloque_memoria->estructura_mensaje != bloque->estructura_mensaje){
            indice++;
        }

        return bloque_memoria->estructura_mensaje == bloque->estructura_mensaje;

    }

    list_find(cache_mensajes->memoria, buscar_bloque);
    if(debug_broker) log_debug(broker_logger,"El id malo es: %d",indice);

	return indice-1;
}



/* Seteo un bloque vacio*/
t_bloque_memoria* crear_bloque_vacio(int tamanio_particion, void* particion){

    t_bloque_memoria* bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    bloque->tamanio_particion = tamanio_particion;
    bloque->esta_vacio = true;
    bloque->estructura_mensaje = particion;
    bloque->timestamp = 0;
    bloque->last_time = 0;

    return bloque;

}




/*Libero la memoria de un determinado bloque y lo retorno */
void liberar_bloque_memoria(t_bloque_memoria* bloque){

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

	/* Vacio el timestamp del bloque*/
	bloque->timestamp = 0;

	/* Vacio el last_time del bloque*/
	bloque->last_time = 0;

    return ;

}



/*Dado el tamaño de una particion, me fijo si puede alojarse a la primera 
	o si hay que correr el algoritmo de eliminacion*/ 
bool puede_alojarse(int tamanio_bytes){

    bool tiene_espacio_suficiente(void* _bloque){

        t_bloque_memoria* bloque = (t_bloque_memoria*) _bloque;

        return bloque->tamanio_particion >= tamanio_bytes && bloque->esta_vacio;

    }

    t_bloque_memoria *bloque_posible = list_find(cache_mensajes->memoria,tiene_espacio_suficiente);

    return bloque_posible != NULL;

}




/*Obtengo el tiempo actual en segundos*/ 
uint64_t get_timestamp(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t  x = (uint64_t)( (tv.tv_sec)*1000 + (tv.tv_usec)/1000 );
	return x;
}