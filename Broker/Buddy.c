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
	int bytes_potencia_dos = numero_potencia_dos(bytes_a_alojar);

    /* Me fijo si la particion puede alojarse a la primera */
    bool sePuedeAlojar = puede_alojarse(bytes_potencia_dos);

	/* Seteo un bool para controlar cuando ya aloje la particion */
    bool alojado = false;

	t_bloque_memoria* bloque_borrado;

    if(debug_broker) log_debug(broker_logger,"Buddy System", NULL);
    if(debug_broker) log_debug(broker_logger,"Alojar: %i", estructura_mensaje->tamanio_mensaje );
    if(debug_broker) log_debug(broker_logger,"Particion a crear de: %i", bytes_potencia_dos);
	printf("\n");

	while(alojado == false){ /* Mientras no este alojado */

        /* Me fijo si puedo alojarla a la primera */
        if(sePuedeAlojar == true){ 

			if(debug_broker) log_debug(broker_logger,"Ejecutando Algoritmo de Particion Libre %s",algoritmo_particion_libre);
	        printf("\n");

            /* 	Si puede alojarse a la primera: Alojo la partición según el funcionamiento de buddies  */
			asignar_bloque_BS(estructura_mensaje, bytes_potencia_dos);

            if(debug_broker) log_debug(broker_logger, "Aloje la nueva particion , BS");

            //seteo alojado en true, para salir del while
            alojado = true;

            continue;
        }		
		else { /* En caso de no alojarse a la primera */ 

			/* Elimino una particion */
			bloque_borrado = reemplazar_bloque_BS();

			/* Consolido buddies entorno a la particion eliminada*/
			consolidacion_BS(bloque_borrado);
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

	void* posicion_relativa = calcular_posicion_relativa(particion);
	if(debug_broker) log_debug(broker_logger,"Encontre particion libre, posicion relativa %d", posicion_relativa);
	printf("\n");

	/* Particionar el bloque y asignar datos*/
	particionar_bloque_buddies( particion, estructura_mensaje, tamanio_de_particion);

	return;
}


/*	Es el algoritmo best fit , que se encarga de encontrar la particion libre
	pero teniendo en cuenta, que es la que mejor se ajuste */
t_bloque_memoria* encontrar_particion_libre(int tamanio_de_particion){

	t_bloque_memoria* bloque_encontrado;

    t_bloque_memoria* bloque_auxiliar;
    
	t_bloque_memoria* primer_bloque = list_get(cache_mensajes, 0);
    int tam_minimo= primer_bloque->tamanio_particion; 

	if(debug_broker) log_debug(broker_logger,"Buscando particion libre");
    
	/* Recorro la lista para obtener el primer bloque donde quepa mi particion nueva */
	for(int i=0; i<list_size(cache_mensajes->memoria); i++){

        /* Obtengo el bloque en la posicion de la lista que estamos */
        bloque_auxiliar = list_get(cache_mensajes->memoria, i);

        /* Me fijo si el tamaño que quiero alojar cabe o no en el bloque actual y ademas si el bloque en el que estoy esta vacio o no */
        if((bloque_auxiliar->esta_vacio == true) && (bloque_auxiliar->tamanio_particion >= tamanio_de_particion)){

            /* Me fijo si el tamaño mas chico de particion es menor o igual al tamaño del bloque auxiliar actual, 
				en caso de serlo, al bloque le asigno el auxiliar y asi me quedo con el bloque mas chico de toda la lista */
            if(tam_minimo >= bloque_auxiliar->tamanio_particion){
                bloque_encontrado = bloque_auxiliar;
				tam_minimo = bloque_auxiliar->tamanio_particion;
            }
        }
    }

	return bloque_encontrado;
}


/* 	Dado un bloque de memoria, se encarga de particionar el bloque.
	Teniendo en cuenta, que lo tiene que particionar la cantidad de veces necesarias 
	para que sea del menor tamaño posible. 
	Tambien asigna los datos a la particion, agrega los nuevos buddies a la lista
	de bloques. */
void particionar_bloque_buddies(t_bloque_memoria* particion_inicial,t_mensaje_cola* estructura_mensaje, int tamanio_bytes_pot_dos){

	t_bloque_memoria* bloque_restante;

	/* Me fijo si puedo particionar el bloque, si el tamaño de mi particion */
	bool puedo_particionar = (particion_inicial->tamanio_particion > tamanio_bytes_pot_dos);

	/* Obtengo el indice de la particion a particionar*/
	int indice_nodo_particionar = obtener_indice_particion(particion_inicial);

    if(debug_broker) log_debug(broker_logger,"Indice a particionar del nodo: %d",indice_nodo_particionar);

	if(debug_broker) log_debug(broker_logger,"Puedo particionar el bloque y achicarlo: %d", puedo_particionar);

	/* Mientras pueda particionar
		1- Tengo que crear una nueva particion del tamaño divido 2, 
			verificando que sea potencia de 2
		2- Agregar la nueva particion a la lista de particiones
		3- Tengo que volver a mirar si puedo seguir particionando 
			y seguir verificando las potencias de 2 */

	/* En caso de poder particionar, entro en el while y empiezo a crear buddies */
	while(puedo_particionar){

		/* Me fijo cuanto espacio sobra y si es potencia de 2 */
		int tamanio_restante = particion_inicial->tamanio_particion / 2 ;
		bool es_potencia_de_dos = tamanio_potencia_dos(tamanio_restante);
    	
		if(es_potencia_de_dos){ /* Si el tamaño restante es potencia de dos, particiono */

			/* Como me sobra espacio lo separo en un nuevo nodo */
    		void* particion_restante = ((char *)particion_inicial->estructura_mensaje) + tamanio_restante;
			if(debug_broker) log_debug(broker_logger,"particion restante: %p", particion_restante);

			/* Seteo el nuevo tamaño de la particion inicial donde quiero alojar*/
			particion_inicial->tamanio_particion = tamanio_restante;

			/* Seteo el bloque buddie */
        	bloque_restante = crear_bloque_vacio(tamanio_restante, particion_restante);

        	/* Agrego el buddie a la lista de bloques*/
			list_add_in_index(cache_mensajes->memoria, indice_nodo_particionar + 1, bloque_restante);  
		
			void* pos_relativa_buddie = calcular_posicion_relativa(bloque_restante);
			if(debug_broker) log_debug(broker_logger,"Buddies particionados, nuevo buddie en: %d , de tamaño: %d", pos_relativa_buddie, tamanio_restante);
			printf("\n");
		}

		/* Me fijo de nuevo si puedo particionar para ver si sigo en el while o corto*/
		puedo_particionar = (particion_inicial->tamanio_particion > tamanio_bytes_pot_dos);
		if(debug_broker) log_debug(broker_logger,"Puedo particionar el bloque y achicarlo: %d", puedo_particionar);
	}

	/* En caso de no poder particionar mas, porque el bloque es justo del tamaño que necesito */
	if(!puedo_particionar && (tamanio_bytes_pot_dos == particion_inicial->tamanio_particion)){

    	log_info(broker_logger, "Almacenado mensaje en la posicion real %p", particion_inicial->estructura_mensaje);

		/* Seteo el nodo inicial como ocupado , y actualizo el tamaño */
    	particion_inicial->tamanio_particion = tamanio_bytes_pot_dos;
    	particion_inicial->esta_vacio = false;
		particion_inicial->timestamp = get_timestamp();
		particion_inicial->last_time = get_timestamp();


    	/* Copio el mensaje a MP y apunto a la estructura_mensaje */      
    	memcpy((void*)(particion_inicial->estructura_mensaje),estructura_mensaje->mensaje,estructura_mensaje->tamanio_mensaje);

		//free(estructura_mensaje->mensaje);

		//seteo el payload en aux
    	void* aux_mensaje = particion_inicial->estructura_mensaje; 
		//cargo la estructura en bloque
    	particion_inicial->estructura_mensaje = estructura_mensaje;
	    //seteo el payload en mensaje
    	particion_inicial->estructura_mensaje->mensaje = aux_mensaje;  

		/* Calculo la posicion relativa */
    	void* posicion_relativa = calcular_posicion_relativa(particion_inicial);
    	log_info(broker_logger, "Almacenado en la posicion relativa %d",posicion_relativa);

		if(debug_broker) log_debug(broker_logger, "Bloque particionado...");
    	printf("\n");
	}
    //muestro por pantalla antes de compactar como estaba la memoria
    if(debug_broker) list_iterate(cache_mensajes->memoria, print_memoria);

	return ;
}




/* Se encarga de ir borrando una particion, teniendo en cuenta los 
	algoritmos de reemplazo*/
t_bloque_memoria* reemplazar_bloque_BS(){

    if(debug_broker) log_debug(broker_logger,"Por ejecutar algoritmo de reemplazo %s", algoritmo_reemplazo);

	t_bloque_memoria* bloque_eliminado;

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (strcmp( algoritmo_reemplazo, "LRU") == 1){
        bloque_eliminado = algoritmo_lru();
    }
    else{
        bloque_eliminado = algoritmo_fifo();
    }

    if(debug_broker) log_debug(broker_logger,"Termine de ejecutar el reemplazo");
    printf("\n");

	return bloque_eliminado;
}




/* Realiza la consolidacion de buddies, dado un bloque.*/
void consolidacion_BS(t_bloque_memoria* bloque){

	// Obtengo la posicion relativa de mi bloque 
	void* posicion_relativa_bloque = calcular_posicion_relativa(bloque);
	log_error(broker_logger, "posicion relativa del bloque actual %p", posicion_relativa_bloque);

	// Obtengo el indice de un buddy 
	int indice_bloque = obtener_indice_particion(bloque);
	
	// Obtengo los bloques que rodean al que libero 
	t_bloque_memoria* bloque_anterior = list_get(cache_mensajes->memoria, indice_bloque-1);
	
	t_bloque_memoria* bloque_siguiente = list_get(cache_mensajes->memoria, indice_bloque+1);

	/*
		3- Mientras un bloque tenga buddies:
			-consolido el bloque con su buddie
			-seteo nuevamente el bloque siguiente y anterior
			-me fijo de nuevo si son buddies con alguno de los dos
			y asi repito hasta que no pueda encontrar ninguna de estas condiciones
	*/

	if(bloque_siguiente!=NULL) log_warning(broker_logger, " tiene buddie a derecha? %d ", son_buddies(bloque,bloque_siguiente) );
	
	// Me fijo si son buddies los bloques siguiente y anterior, si son buddies consolido 
	if(bloque_siguiente != NULL && bloque_siguiente->esta_vacio == true && son_buddies(bloque,bloque_siguiente)){

		if(debug_broker) log_debug(broker_logger,"Tiene buddie libre, a derecha");
		
		// Si cumple con las condiciones, consolido bloques
		consolidar_bloques_buddies(bloque,bloque_siguiente);

		if(debug_broker) log_debug(broker_logger,"Ya consolide buddies.");
		if(debug_broker) log_debug(broker_logger,"Miro si tengo más buddies libres.");
		printf("\n");

		//if(debug_broker) list_iterate(cache_mensajes->memoria, print_memoria);

		// Como consolido, implemento recursividad para ver si tengo mas buddies para consolidar
		consolidacion_BS(bloque);
	}

	if(bloque_anterior!=NULL) log_warning(broker_logger, " tiene buddie a izquierda? %d ", son_buddies(bloque_anterior,bloque) );
	
	if(bloque_anterior != NULL && bloque_anterior->esta_vacio == true && son_buddies(bloque_anterior,bloque)){

		if(debug_broker) log_debug(broker_logger,"Tiene buddie libre, a izquierda");

		// Si cumple con las condiciones, consolido bloques
		consolidar_bloques_buddies(bloque_anterior,bloque);

		if(debug_broker) log_debug(broker_logger,"Ya consolide buddies.");
		if(debug_broker) log_debug(broker_logger,"Miro si tengo más buddies libres.");
		printf("\n");

		//if(debug_broker) list_iterate(cache_mensajes->memoria, print_memoria);

		// Como consolido, implemento recursividad para ver si tengo mas buddies para consolidar
		consolidacion_BS(bloque_anterior);
	}
    
	if(debug_broker) log_debug(broker_logger,"No tengo más buddies libres.");

	if(debug_broker) log_debug(broker_logger,"Ya consolide luego de vaciar una particion", NULL);
    printf("\n");

	return ;
}


/* Determina si 2 bloques son buddies o no */
bool son_buddies(t_bloque_memoria* bloque_anterior, t_bloque_memoria* bloque_siguiente){

	/* Me fijo si ambos tienen el mismo tamaño para ver si son buddies*/
	if(bloque_anterior->tamanio_particion == bloque_siguiente->tamanio_particion){
		/* ACA VIENE LA PARTE DEL XOR */
		return ((int)bloque_anterior->estructura_mensaje) == ((int)bloque_siguiente->estructura_mensaje ^ bloque_anterior->tamanio_particion);
	}
	else{
		return false;
	}
}

/* Se encarga de realizar la consolidacion en si */
void consolidar_bloques_buddies(t_bloque_memoria* bloque_anterior, t_bloque_memoria* bloque_siguiente){

	consolidar_dos_bloques(bloque_anterior, bloque_siguiente);

	return;
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




/*
void consolidacion_BS(t_bloque_memoria* bloque){

	// Obtengo la posicion relativa de mi bloque 
	void* posicion_relativa_bloque = calcular_posicion_relativa(bloque);
	log_error(broker_logger, "posicion relativa del bloque actual %p", posicion_relativa_bloque);

	// Obtengo el indice de un buddy 
	int indice_bloque = obtener_indice_particion(bloque);
	
	// Obtengo los bloques que rodean al que libero 
	t_bloque_memoria* bloque_anterior = list_get(cache_mensajes->memoria, indice_bloque-1);
	
	t_bloque_memoria* bloque_siguiente = list_get(cache_mensajes->memoria, indice_bloque+1);

	bool tiene_buddies = tiene_buddies_libres(bloque);

	while(tiene_buddies){

		if(bloque_siguiente!=NULL) log_warning(broker_logger, " tiene buddie a derecha? %d ", son_buddies(bloque,bloque_siguiente) );
	
		// Me fijo si son buddies los bloques siguiente y anterior, si son buddies consolido 
		if(bloque_siguiente != NULL && bloque_siguiente->esta_vacio == true && son_buddies(bloque,bloque_siguiente)){

			if(debug_broker) log_debug(broker_logger,"Tiene buddie libre, a derecha");
			
			// Si cumple con las condiciones, consolido bloques
			consolidar_bloques_buddies(bloque,bloque_siguiente);

			if(debug_broker) log_debug(broker_logger,"Ya consolide buddies.");
			if(debug_broker) log_debug(broker_logger,"Miro si tengo más buddies libres.");
			printf("\n");

			//if(debug_broker) list_iterate(cache_mensajes->memoria, print_memoria);

			// Como consolido, implemento recursividad para ver si tengo mas buddies para consolidar
			//consolidacion_BS(bloque);
			tiene_buddies = tiene_buddies_libres(bloque);
		}

		if(bloque_anterior!=NULL) log_warning(broker_logger, " tiene buddie a izquierda? %d ", son_buddies(bloque_anterior,bloque) );
		
		if(bloque_anterior != NULL && bloque_anterior->esta_vacio == true && son_buddies(bloque_anterior,bloque)){

			if(debug_broker) log_debug(broker_logger,"Tiene buddie libre, a izquierda");

			// Si cumple con las condiciones, consolido bloques
			//consolidar_bloques_buddies(bloque_anterior,bloque);

			if(debug_broker) log_debug(broker_logger,"Ya consolide buddies.");
			if(debug_broker) log_debug(broker_logger,"Miro si tengo más buddies libres.");
			printf("\n");

			//if(debug_broker) list_iterate(cache_mensajes->memoria, print_memoria);

			// Como consolido, implemento recursividad para ver si tengo mas buddies para consolidar
			//consolidacion_BS(bloque_anterior);
			tiene_buddies = tiene_buddies_libres(bloque);
		}

	}
	
    
	if(debug_broker) log_debug(broker_logger,"No tengo más buddies libres.");

	if(debug_broker) log_debug(broker_logger,"Ya consolide luego de vaciar una particion", NULL);
    printf("\n");

	return ;
}

bool tiene_buddies_libre(t_bloque_memoria* bloque){


	return;
} */