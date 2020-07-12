#include "Buddy.h"
extern int tamanio_memoria;
extern int tamanio_minimo_particion;
extern char* algoritmo_memoria;
extern char* algoritmo_reemplazo;
extern char* algoritmo_particion_libre;
extern char* ip_broker;
extern int frecuencia_compactacion;
extern char* log_file;

extern t_log* broker_logger;
extern t_config* config;

extern t_cache_colas* cache_mensajes;

	/*
		FUNCIONAMIENTO

		Busco particion libre:
			- Encuentro particion libre -> usa el Best fit, el mejor ajuste,
						necesito la particion mas pequeña donde quepa
			- Miro el tamaño de la particion 
				- si es igual lo alojo
				- si es menor no me sirve
				- si es mayor la particiono y vuelvo a revisar el tamaño
			- Si no encuentro una particion libre donde quepa o si no hay libres:
				- Corro el algoritmo de eliminacion
				- Consolidar buddies
				- Vuelvo a intentar alojas, repito hasta poder alojar

	*/

//TODO: modificar el algoritmo de memoria que devuelve un t_bloque

/*
	Bueno algunas cosas a tener en cuenta para Buddy:
		- Puedo plantear un arbol paralelo apenas arranque el funcionamiento del broker
			- Cuando leo el config si es buddy hago esto
			- En caso de plantear un arbol, me serviria guardar el id padre para consolidar
			dos buddies.
			- Puedo usar XOR para encontrar buddies, pero necesito posiciones relativas 
			y no reales de memoria
			- Usamos best fit -> problemas / soluciones de usar FF?
			- Sin compactacion		
*/


/* Funcion que arranca con el funcionamiento del Buddy en si */
void buddy_funcionamiento(t_mensaje_cola* estructura_mensaje){

	/* Me fijo si cumple con el tamaño minimo de particion */
	int bytes_a_alojar = tamanio_a_alojar(estructura_mensaje->tamanio_mensaje);

	/* Busco el numero potencia de 2 mas cercano */
	int bytes_potencia_dos = tamanio_potencia_dos(bytes_a_alojar);

    /* Me fijo si la particion puede alojarse a la primera */
    bool sePuedeAlojar = puede_alojarse(bytes_potencia_dos);

	/* Seteo un bool para controlar cuando ya aloje la particion */
    bool alojado = false;

	t_bloque_memoria* bloque_borrado;

	while(alojado == false){ /* Mientras no este alojado */

        /* Me fijo si puedo alojarla a la primera */
        if(sePuedeAlojar == true){ 

            /* 	Si puede alojarse a la primera: 
				Alojo la partición según el funcionamiento de buddies  */
			asignar_bloque_BS(estructura_mensaje, bytes_potencia_dos);

            if(debug_broker) log_debug(broker_logger, "Aloje la nueva particion , BS");

            //seteo alojado en true, para salir del while
            alojado = true;

            continue;
        }	

		/* En caso de no alojarse a la primera */ 
		else {

			/* Elimino una particion */
			bloque_borrado = reemplazo_bloque_BS();

			/* Consolido buddies entorno a la particion eliminada*/
			consolidar_buddies(bloque_borrado);
		}

		/* Me fijo de nuevo si puede alojarse */
        sePuedeAlojar = puede_alojarse(bytes_potencia_dos);

	}


    return;
}

/*	Busca una particion libre, la mas chica (best_fit).
  	La particiona si sobra espacio.
	Asigna los datos.  
	*/
void asignar_bloque_BS(t_mensaje_cola* estructura_mensaje, int tamanio_de_particion){

	/* Encontrar particion libre */
	t_bloque_memoria* particion = encontrar_particion_libre(tamanio_de_particion); 

	/* Particionar el bloque */
	particionar_bloque_buddies( particion, estructura_mensaje);

	return;
}

/*
t_bloque_memoria* algoritmo_best_fit(int tamanio_parti, t_mensaje_cola* estructura_mensaje){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice;
    int tam_minimo=0; 

    if(debug_broker) log_debug(broker_logger, "Ejecutando best fit");


    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i<list_size(cache_mensajes->memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(cache_mensajes->memoria, i);

        //me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no 
        if((aux->esta_vacio == true) && (aux->tamanio_particion >= tamanio_parti)){
            
            //me fijo si el tamaño mas chico de particiones sigue siendo 0 
            if(tam_minimo==0){
                tam_minimo = aux->tamanio_particion;
            }

            //me fijo si el tamaño mas chico de particion es menor o igual al tamaño del bloque auxiliar actual, en caso de serlo, al bloque le asigno el aux
            //y asi me quedo con el bloque mas chico de toda la lista
            if(tam_minimo <= aux->tamanio_particion){
                bloque = aux;
            }

        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    if(debug_broker) log_debug(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_parti,indice,estructura_mensaje);

    if(debug_broker) log_debug(broker_logger, "Ya ejecute best fit");

    return bloque_final;

}*/

/*	Es el algoritmo best fit , que se encarga de encontrar la particion libre
	pero teniendo en cuenta, que es la que mejor se ajuste */
t_bloque_memoria* encontrar_particion_libre(int tamanio_de_particion){

	t_bloque_memoria* bloque_encontrado;

	t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    
    int indice;
    int tam_minimo=0; 


    /* Recorro la lista para obtener el primer bloque
		donde quepa mi particion nueva */
	for(int i=0; i<list_size(cache_mensajes->memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(cache_mensajes->memoria, i);

        //me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no 
        if((aux->esta_vacio == true) && (aux->tamanio_particion >= tamanio_parti)){
            
            //me fijo si el tamaño mas chico de particiones sigue siendo 0 
            if(tam_minimo==0){
                tam_minimo = aux->tamanio_particion;
            }

            //me fijo si el tamaño mas chico de particion es menor o igual al tamaño del bloque auxiliar actual, en caso de serlo, al bloque le asigno el aux
            //y asi me quedo con el bloque mas chico de toda la lista
            if(tam_minimo <= aux->tamanio_particion){
                bloque = aux;
            }

        }

    }

	/* Obtengo el indice del bloque que mejor se ajusta a mi particion */
    indice = obtener_indice_particion(bloque);

    /* Obtengo el bloque de la lista */
    bloque_encontrado = list_get(cache_mensajes->memoria, indice);


	return bloque_encontrado;
}

/* 	Dado un bloque de memoria, se encarga de particionar el bloque.
	Teniendo en cuenta, que lo tiene que particionar la cantidad de veces necesarias 
	para que sea del menor tamaño posible. */
void particionar_bloque_buddies(t_bloque_memoria* particion,t_mensaje_cola* estructura_mensaje){



	return ;
}


t_mensaje_cola* reemplazar_bloque_BS(){



	return;
}

/* Realiza la consolidacion de buddies, dado un bloque.*/
void consolidar_buddies(t_mensaje_cola* estructura_mensaje){

	/* Obtengo el indice de un buddy */
	/* Me fijo si la posicion anterior esta libre
	 y Chequeo si tienen el mismo tamaño */
	/* Me fijo si son buddies , si son buddies consolido
		Para saber si son buddies necesito la posicion de memoria relativa*/

	/* Me fijo lo mismo con la posicion siguiente en caso que la primer opcion de negativo*/

	/* Una vez que me fije , vuelvo a repetir con el bloque consolidado */	

	return ;
}

void particionar_bloque_buddies(){

	return ;
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



/*Me fijo si un numero es potencia de dos. Si lo es, devuelvo ese numero, y si no lo es, busco el numero mayor mas cercano potencia de 2*/
int numero_potencia_dos(int tamanio_en_bytes){
	int bytes = tamanio_en_bytes;

	bool num_pot_dos = tamanio_potencia_dos(tamanio_en_bytes);

	while(!num_pot_dos){
		bytes = bytes + 1;
		
		num_pot_dos = tamanio_potencia_dos(bytes);
	}

	return bytes;

}