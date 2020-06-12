#include "utils.h"

/*Dado el valor de memoria inicial, asigno un bloque para la memoria inicial y lo retorno*/
t_bloque_memoria* AsignarMemoriaInicial(int tamanio_en_bytes, t_list* lista_memoria){
    
    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    /* Genero el bloque de memoria inicial*/
    t_bloque_memoria *bloque;
    bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    bloque->id = 0;
    bloque->tamanio = tamanio_en_bytes;
    bloque->esta_vacio = true;
    bloque->payload = memoria_inicial;
  
    /* Agrego el bloque a la lista */
    list_add(lista_memoria,bloque);

    /* Retorno la variable inicial para guardarla en la global */
    return bloque;
}

/*Dado un tamaño de particion, devuelvo el bloque asignado y derivo segun el algoritmo de memoria */
t_bloque_memoria* AsignarParticionMemoria(int tamanio_en_bytes){
    
    //Creo una nueva particion que es la que voy a devolver luego de asignar la particion
    t_bloque_memoria* nuevaParticion;
    //Asigno un bloque segun el algoritmo de memoria que utilicemos
    nuevaParticion = algoritmo_de_memoria(tamanio_en_bytes);

    return nuevaParticion;
}

/*Dado el tamaño de una particion, me fijo si puede alojarse a la primera 
	o si hay que correr el algoritmo de eliminacion*/ 
bool puedeAlojarse(int tamanio_en_bytes){

	int tamanio_particion= tamanio_minimo_particion;
	bool puedeEntrar = FALSE;
	t_bloque_memoria* elemento;

	//Primero me fijo si el tamaño de mi mensaje a guardar, es menor que el minimo tamaño de particion
	
	if(tamanio_en_bytes > tamanio_minimo_particion){ //tamanio_minimo_particion es variable global
		tamanio_particion = tamanio_en_bytes;
	}


	//recorro la lista de memoria, hasta encontrar una particion que este vacia y 
	//entre mi tamaño de particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

		//obtengo el elemento de la lista en la posicion i
		elemento = list_get(lista_memoria, i);

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

/*Dado un indice y un tamaño en byte, alojo la particion en el indice, y creo el nuevo bloque con lo restante en el caso
	que haya algo restante. La idea de esta funcion es que sea llamada por el algoritmo de asignacion.
	Se usaria una vez encontrado el lugar en memoria que ocuparia mi nueva particion */
void particionarBloque(int tamanio, int indice_nodo_particionar){

    t_bloque_memoria *bloque_restante;
	t_bloque_memoria *bloque_inicial;

    /* Obtengo el nodo del bloque a particionar por su indice */
    bloque_inicial = list_get(lista_memoria, indice_nodo_particionar);

    /* Si me sobra espacio lo separo en un nuevo nodo */
    if(bloque_inicial->tamanio - tamanio > 0){ 

        bloque_restante->tamanio = bloque_inicial->tamanio - tamanio;
        bloque_restante->esta_vacio = true;
        bloque_restante->payload = bloque_inicial->payload + tamanio + 1;

        list_add_in_index(lista_memoria, indice_nodo_particionar + 1, bloque_restante);    

    }

    /* Seteo el nodo inicial como ocupado , y actualizo el tamaño */
    bloque_inicial->tamanio = tamanio;
    bloque_inicial->esta_vacio = false;

    return;
}

//el indice lo tengo que buscar por ej con el payload 
int obtenerIndiceParticion(t_bloque_memoria* bloque){

	int indice;
	t_bloque_memoria* elemento ;

	for(int i=0; i< list_size(lista_memoria); i++){

		elemento = list_get(lista_memoria, i);

		if(elemento->payload == bloque->payload){
			indice=i;
			i = list_size(lista_memoria);
			break;
		}
		
	}


	return indice;
}

t_bloque_memoria* alojarBloque(){
	/*tengo que correr el algoritmo 
		encontrar la particion o bloque
		obtener el indice de partcion
		particionar el bloque
		y devuelvo el bloque alojado*/

		t_bloque_memoria* bloque;

		return bloque;
}

//*****************Auxiliares especificas Buddy System******************************

/*Calculo si un numero es potencia de dos para buddy system*/
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


