#include "utils.h"

void* AsignarParticionMemoria(int tamanio_en_bytes){
    
    //Creo una nueva particion que es la que voy a devolver luego de asignar la particion
    t_particion* nuevaParticion;
    //Asigno un bloque segun el algoritmo de memoria que utilicemos
    nuevaParticion = algoritmo_de_memoria(tamanio_en_bytes);

    return nuevaParticion;
}

bool puedeAlojarse(int tamanio_en_bytes){

	int tamanio_particion= tamanio_minimo_particion;
	bool puedeEntrar = FALSE;

	//Primero me fijo si el tamaño de mi mensaje a guardar, es menor que el
	//minimo tamaño de particion
	
	if(tamanio_en_bytes > tamanio_minimo_particion){ //tamanio_minimo_particion es variable global
		tamanio_particion = tamanio_en_bytes;
	}


	//recorro la lista de memoria, hasta encontrar una particion que este vacia y 
	//entre mi tamaño de particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

		//obtengo el elemento de la lista en la posicion i
		t_particion* elemento = list_get(lista_memoria, i);

		//me fijo si el elemento esta vacio y a su vez entra mi particion
		//Si entra, cambio el valor de puedeEntrar, y corto el for.
		if((elemento->esta_vacio == TRUE) && (elemento->tamanio >= tamanio_particion)){
			puedeEntrar= TRUE;
			i = list_size(lista_memoria);
			break;
		}

	}


	return puedeEntrar;
}


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

					/* Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete
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

					/* Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete
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

					/* Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete
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

					/* Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete
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

					/* Apunto a los datos del mensaje 
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete
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


//Calculo si un numero es potencia de dos para buddy system
bool tamanio_potencia_dos(int tamanio_en_bytes){
	
	if((tamanio_en_bytes == 0) && (tamanio_en_bytes < 0)){

		return false;

	}
	else{

		return ((tamanio_en_bytes & (tamanio_en_bytes - 1)) == 0);

	}

}

//Me fijo si un numero es potencia de dos.
//Si lo es, devuelvo ese numero, 
//y si no lo es, busco el numero mayor mas cercano potencia de 2
int numero_potencia_dos(int tamanio_en_bytes){
	int bytes = tamanio_en_bytes;

	bool num_pot_dos = tamanio_potencia_dos(tamanio_en_bytes);

	while(!num_pot_dos){
		bytes = bytes + 1;
		
		num_pot_dos = tamanio_potencia_dos(bytes);
	}

	return bytes;

}