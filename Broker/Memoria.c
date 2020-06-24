#include "Memoria.h"
extern int tamanio_memoria;
extern int tamanio_minimo_particion;
extern char* algoritmo_memoria;
extern char* algoritmo_reemplazo;
extern char* algoritmo_particion_libre;
extern char* ip_broker;
extern int frecuencia_compactacion;
extern int puerto_broker;
extern char* log_file;

extern t_log* broker_logger;
extern t_config* config;

//ASCII boquitas en hexa
//control + shift + u 3e y 3c 


//--------------------------- ADMINISTRACION DE MEMORIA-------------------

/*Dado el valor de memoria inicial, asigno un bloque para la memoria inicial y lo retorno*/
void asignar_memoria_inicial(int tamanio_en_bytes){

    //log_info(broker_logger, "Por asignar la memoria inicial");

    /* Asigno la memoria a un puntero auxiliar y la inicializo en cero */
    void* memoria_inicial = malloc(tamanio_en_bytes*sizeof(char));    
    memset(memoria_inicial, 0, tamanio_en_bytes*sizeof(char));
   
    printf("Se asigno la memoria inicial");
    printf("La direccion inicial de memoria es: %p \n", memoria_inicial);

    /* Genero el bloque de memoria inicial*/
    t_bloque_memoria *bloque;
    bloque = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    //bloque->id = 0;
    bloque->tamanio_particion = tamanio_en_bytes;
    bloque->tamanio_mensaje = 0;
    bloque->esta_vacio = true;
    bloque->payload = memoria_inicial;
	bloque->timestamp = 0;
	bloque->last_time = 0;

    printf("Particion de memoria inicial creada con: %i \n", bloque->tamanio_particion );

    /* Agrego el bloque a la lista */
    list_add(lista_memoria,bloque);

    //log_info(broker_logger, "Ya asigne la memoria inicial y cree el primer bloque");

    /* Retorno la variable inicial para guardarla en la global */
    return ;
}




/*Dado un tamaño de particion, devuelvo el bloque asignado y derivo segun el algoritmo de memoria 
	Ya corri todos los algoritmos aca. 
	Esta deberia ser la que tiene que llamar la cola de mensajes*/
t_bloque_memoria* asignar_particion_memoria(int tamanio_msje, void* mensaje){
    
    //log_info(broker_logger, "Por asignar memoria a mi particion");

    //Creo una nueva particion que es la que voy a devolver luego de asignar la particion
    t_bloque_memoria* nuevaParticion;
    //Asigno un bloque segun el algoritmo de memoria que utilicemos
    nuevaParticion = algoritmo_de_memoria(tamanio_msje, mensaje);

    //log_info(broker_logger, "Ya le asigne memoria a mi particion!");

    return nuevaParticion;
}



//----------------------------ALGORITMOS DE MEMORIA-------------------------


/*Segun el algoritmo que me pasa el archivo de configuracion, designo con esta funcion
    que algoritmo se va a usar y le paso la lista y el tamaño de bytes*/
t_bloque_memoria* algoritmo_de_memoria(int tamanio_msje, void* mensaje){

    //Creo una nueva particion, que es donde se van a guardar los datos de la particion alojada
    //y la particion que voy a retornar
    t_bloque_memoria* particionNueva;
    

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (  strcmp( algoritmo_memoria, "BD") == 1){
        particionNueva= buddy_system(tamanio_msje, mensaje);
    }
    else{
        particionNueva= particiones_dinamicas(tamanio_msje, mensaje);
    }

    return particionNueva;
}




//---------------------------PARTICIONES DINAMICAS CON COMPACTACION----------------------
t_bloque_memoria* particiones_dinamicas( int tamanio_msje, void* mensaje){

    //log_info(broker_logger, "Ejecutando particiones dinamicas");

    printf("Particiones dinamicas \n");
    printf("Alojar: %i \n", tamanio_msje );

    t_bloque_memoria* particionNueva;
    //particionNueva = (t_bloque_memoria*)malloc(sizeof(t_bloque_memoria));

    //me fijo si el tamaño del mensaje es menor al minimo tamaño de particion
    int tamanio_parti = tamanio_a_alojar(tamanio_msje);
    printf("Por alojar particion de: %i \n", tamanio_parti );


    //me fijo si la particion puede alojarse a la primera
    bool sePuedeAlojar = puede_alojarse(tamanio_parti);

    bool alojado = false;

    //igualo la frecuencia para compactar segun cuantas veces compacte
    int frec_para_compactar = frecuencia_compactacion; 

    
    while(alojado == false){  //mientras no  este alojado hago la secuencia pueda alojarlo tengo que ir vaciando particiones y fijandome si tengo que compactar

        //me fijo si puedo alojarla a la primera
        if(sePuedeAlojar == true){ 
            //si puede alojarse a la primera llamo al algoritmo de particion libre
            particionNueva = algoritmo_de_particion_libre(tamanio_msje, tamanio_parti, mensaje);

            log_info(broker_logger, "Aloje la nueva particion , PD");

            //seteo alojado en true, para salir del while
            alojado = true;
        }
        else{ //si no puede alojarse a la primera ....

            //me fijo si la frecuencia de compactacion esta habilitada para seguir vaciando particiones
            if(frec_para_compactar == -1){ //si la frecuencia esta seteada en -1
                //corro el algoritmo y tengo que compactar si o si 
                algoritmo_de_reemplazo();

                log_info(broker_logger, "Por compactar...");

                compactar();
            }
            else if(frec_para_compactar > 0){ //en caso de estar habilitada la frecuencia
                algoritmo_de_reemplazo();
                frec_para_compactar = frec_para_compactar - 1;
            }
            else{  //en caso de no estar habilitada , porque ya se agoto

                log_info(broker_logger, "Por compactar...");

                compactar();
                //seteo de nuevo la frecuencia para la prox compactacion
                frec_para_compactar = frecuencia_compactacion;
            }

            //me fijo de nuevo si puede alojarse
            sePuedeAlojar = puede_alojarse(tamanio_parti);
        }

    }

    return particionNueva;

}




//------------------------------BUDDY SYSTEM----------------------------------
t_bloque_memoria* buddy_system( int tamanio_en_bytes, void* mensaje){

    log_info(broker_logger, "Ejecutando buddy system");

    //int bytes_reservar= numero_potencia_dos(int tamanio_en_bytes)

    t_bloque_memoria* particionNueva;

    return particionNueva;
    
}


//---------------------------FIN DE ALGORITMOS DE MEMORIA-----------------------




//-----------------------------ELECCION DE PARTICION LIBRE--------------------


t_bloque_memoria* algoritmo_de_particion_libre(int tamanio_msje, int tamanio_parti, void* mensaje){
    
    t_bloque_memoria* bloque;

    log_info(broker_logger, "Por ejecutar algoritmo de particion libre");

    if( strcmp( algoritmo_particion_libre, "FF") == 1){
        bloque = algoritmo_first_fit(tamanio_msje, tamanio_parti, mensaje);
    }
    else{
        bloque = algoritmo_best_fit(tamanio_msje, tamanio_parti, mensaje);
    }

    log_info(broker_logger, "Algoritmo de particion libre ejecutado!");

    return bloque;

}


//------------------------------FIRST FIT------------------------------------
/* Busco el primer hueco donde puedo alojar una nueva particion y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_first_fit(int tamanio_msje, int tamanio_parti, void* mensaje){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice; 

    log_info(broker_logger, "Ejecutando first fit");

    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

        aux = list_get(lista_memoria, i);

        if((aux->tamanio_particion >= tamanio_parti) && (aux->esta_vacio == true)){
            bloque = aux;
            i = list_size(lista_memoria);
            break;
        }

    }

    //obtengo el indice del bloque que voy a particionar
    indice = obtener_indice_particion(bloque);

    log_info(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_parti,indice,tamanio_msje,mensaje);

    log_info(broker_logger, "Ya ejecute el algoritmo fist fit");

    return bloque_final;

}




//------------------------------BEST FIT-------------------------------------
/* Busco el huevo mas pequeño donde quepa la nueva particion a alojar, y la alojo.
    NOTA: para esto ya esta chequeado que puede alojarse
    NOTA2: recibo el tamaño correcto, ya chequeado si es el minimo a alojar o no.*/
t_bloque_memoria* algoritmo_best_fit(int tamanio_msje, int tamanio_parti, void* mensaje){

    t_bloque_memoria* bloque;
    t_bloque_memoria* aux;
    t_bloque_memoria* bloque_final;
    int indice;
    int tam_minimo=0; 

    log_info(broker_logger, "Ejecutando best fit");


    //obtengo el primer bloque donde quepa mi particion nueva
	for(int i=0; i<list_size(lista_memoria); i++){

        //Obtengo el bloque en la posicion de la lista que estamos
        aux = list_get(lista_memoria, i);

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

    log_info(broker_logger, "Por particionar el bloque, ya encontre mi bloque adecuado");

    //particiono el bloque donde voy a alojar mi particion, PERO con el tamaño actualizado
    bloque_final = particionar_bloque(tamanio_parti,indice,tamanio_msje,mensaje);

    log_info(broker_logger, "Ya ejecute best fit");

    return bloque_final;

}

//------------------------------FIN DE ELECCION DE PARTICION------------------




//----------------------------ELIMINAR UNA PARTICION-----------------------


void algoritmo_de_reemplazo(){

    t_bloque_memoria* bloque;

    //segun el algoritmo del archivo de configuracion, utilizo un algoritmo
    if (strcmp( algoritmo_reemplazo, "LRU") == 1){
        bloque = algoritmo_lru();
    }
    else{
        bloque = algoritmo_fifo();
    }

    //realizo la consolidacion siempre que corro el algoritmo de reemplazo
    consolidar(bloque);

    return;

}


//------------------------------FIFO--------------------------------

/* elije como victima a la pagina que esta cargada en memoria hace mas tiempo*/
/* se puede implementar guardando el instante en el que la pagina fue cargada
    o con una cola fifo y se elige a la pagina que primero se agrego*/
t_bloque_memoria* algoritmo_fifo(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();

	for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->timestamp < min_time){
                
                min_time= elemento->timestamp;

                bloque=elemento;

            }
        }

    }
    
    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque);

    return bloque;
}




//------------------------------LRU--------------------------------

/* elije como victima a la pagina menos reciente utilizada*/
/* se puede implementar guardando el instante de ultima referencia a la pagina 
    o con una pila con numeros de paginas, con cada referencia se coloca la pag
    superior, se elige como victima la pag de la parte inferior*/
t_bloque_memoria* algoritmo_lru(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* bloque;
    uint64_t min_time = get_timestamp();

	for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        //me fijo si el elemento actual de la lista vacio y si el timestamp es mayor a cero
        if( (elemento->esta_vacio == false) && (elemento->timestamp > 0) ){
            
            //si el timestamp de la ultima vez usado, es menor al minimo time, lo guardo y me guardo el bloque
            if(elemento->last_time < min_time){
                
                min_time= elemento->last_time;

                bloque=elemento;

            }
        }

    }

    //libero la memoria de un determinado bloque de mi lista , y me lo devuelve
    liberar_bloque_memoria(bloque);
    

    return bloque;
}


//------------------------FIN ELIMINAR UNA PARTICION-----------------------





//-----------------------------AUXILIARES-----------------------------------

/*Dado el tamaño de una particion, me fijo si puede alojarse a la primera 
	o si hay que correr el algoritmo de eliminacion*/ 
bool puede_alojarse(int tamanio_bytes){

	bool puedeEntrar = false;
	t_bloque_memoria* elemento;


	//recorro la lista de memoria, hasta encontrar una particion que este vacia y entre mi tamaño de particion nueva
	for(int i=0; i< list_size(lista_memoria); i++){

		//Obtengo el elemento de la lista en la posicion i
		elemento = list_get(lista_memoria, i);

		//Me fijo si el elemento esta vacio y a su vez entra mi particion
		//Si entra, cambio el valor de puedeEntrar, y corto el for.
		if((elemento->esta_vacio == true) && (elemento->tamanio_particion >= tamanio_bytes)){
			puedeEntrar= true;
			i = list_size(lista_memoria);
			break;
		}

	}


	return puedeEntrar;
}




/*Dado un indice y un tamaño en byte, alojo la particion en el indice, y creo el nuevo bloque con lo restante en el caso
	que haya algo restante. La idea de esta funcion es que sea llamada por el algoritmo de asignacion.
	Se usaria una vez encontrado el lugar en memoria que ocuparia mi nueva particion */
t_bloque_memoria* particionar_bloque(int tamanio_parti, int indice_nodo_particionar, int tamanio_msje, void* mensaje){

    t_bloque_memoria *bloque_restante;
	t_bloque_memoria *bloque_inicial;

    log_info(broker_logger, "Por particionar...");

    /* Obtengo el nodo del bloque a particionar por su indice */
    bloque_inicial = list_get(lista_memoria, indice_nodo_particionar);

    /* Si me sobra espacio lo separo en un nuevo nodo */
    if(bloque_inicial->tamanio_particion - tamanio_parti > 0){ 

        bloque_restante->tamanio_particion = bloque_inicial->tamanio_particion - tamanio_parti;
        bloque_restante->tamanio_mensaje = 0;
		bloque_restante->esta_vacio = true;
        bloque_restante->payload = bloque_inicial->payload + tamanio_parti + 1;
		bloque_restante->timestamp = 0;
		bloque_restante->last_time = 0;


        list_add_in_index(lista_memoria, indice_nodo_particionar + 1, bloque_restante);    

    }

    /* Seteo el nodo inicial como ocupado , y actualizo el tamaño */
    bloque_inicial->tamanio_particion = tamanio_parti;
    bloque_inicial->tamanio_mensaje = tamanio_msje;
    memcpy(bloque_inicial->payload,mensaje,tamanio_msje);
    bloque_inicial->esta_vacio = false;
	bloque_inicial->timestamp = get_timestamp();
	bloque_inicial->last_time = get_timestamp();

    log_info(broker_logger, "Bloque particionado...");

    return bloque_inicial;
}




/*Obtengo el indice de un determinado, recorriendo toda la lista y comparando los payload*/
int obtener_indice_particion(t_bloque_memoria* bloque){

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




/*Me fijo si el tamaño de mi mensaje a guardar, es menor que el minimo tamaño de particion*/
int tamanio_a_alojar(int tamanio){

	int tamanio_final = tamanio_minimo_particion; //tamanio_minimo_particion es variable global

	if(tamanio > tamanio_minimo_particion){ 
		tamanio_final = tamanio;
	}   

	return tamanio_final;

}




/*Libero la memoria de un determinado bloque y lo retorno */
void liberar_bloque_memoria(t_bloque_memoria* bloque){

    /* Marco el bloque como vacio */
    bloque->esta_vacio = true;

	/* Vacio el timestamp del bloque*/
	bloque->timestamp = 0;

	/* Vacio el last_time del bloque*/
	bloque->last_time = 0;

	/* Vacio el tamaño del mensaje del bloque */
	bloque->tamanio_mensaje = 0;   

    return ;

}





/* Se encarga de realizar la compactacion en particiones dinamicas*/
void compactar(){

    t_bloque_memoria* elemento;
    t_bloque_memoria* nuevoBloque;
    t_bloque_memoria* primerBloque = list_get(lista_memoria, 0);//obtengo el primer bloque para sacar la direccion inicial del payload
    int acumulador_libre = 0; //sumo la cant de espacio libre para la nueva particion
    int acumulador_ocupado = 0; //sumo la cant de espacio ocupado para desp validar
    float* payload_acu = primerBloque->payload;


    //recorro la lista para ver que particiones estan libres y voy borrandolas de aca
    for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        //me fijo si el bloque actual en el que estoy, esta vacio
        if(elemento->esta_vacio == true){//si el bloque esta vacio:

            //sumo el tamaño de particion al acumulador libre
            acumulador_libre += elemento->tamanio_particion;

            //borro la particion de la lista
            list_remove_and_destroy_element(lista_memoria, i, (void*)free );
        }
        else{ //si el bloque esta ocupado

            //sumo el tamaño de particion al acumulador ocupado
            acumulador_ocupado += elemento->tamanio_particion;

        }
    }

    //ahora cambio el payload de las que quedaron ocupadas y hay que moverlas
    //compactar_payload();
    for(int i=0; i< list_size(lista_memoria); i++){

        elemento = list_get(lista_memoria, i);

        /*if((payload_acu == elemento->payload) && (elemento->esta_vacio == false)){
            payload_acu += elemento->tamanio_particion
        }*/

        elemento->payload = payload_acu;

        payload_acu = payload_acu + elemento->tamanio_particion + 1;

    }

    if( list_size(lista_memoria)==acumulador_ocupado){
        printf("se realizo bien la compactación ");
    }

    //Creo el nuevo bloque con el tamaño de todas las particiones libres
    nuevoBloque->tamanio_particion = acumulador_libre;
    nuevoBloque->tamanio_mensaje = 0;
    nuevoBloque->esta_vacio = true;
    nuevoBloque->payload = payload_acu + 1;
	nuevoBloque->timestamp = 0;
	nuevoBloque->last_time = 0;
    
    //Agrego el nuevo bloque a la lista de memoria
    list_add(lista_memoria,nuevoBloque);

	return ;
}




/* Se encarga de realizar la consolidacion en Particiones dinamicas*/
void consolidar(t_bloque_memoria* bloque){
    
    int indice_bloque = obtener_indice_particion(bloque);

    t_bloque_memoria* bloque_anterior = list_get(lista_memoria,(indice_bloque-1));

    t_bloque_memoria* bloque_siguiente = list_get(lista_memoria,(indice_bloque+1));

    //Me fijo si el bloque es el ultimo y si el anterior esta vacio
    if(bloque_siguiente == NULL && bloque_anterior->esta_vacio == true){
        consolidar_dos_bloques(bloque_anterior,bloque);
    }
    //Me fijo si el bloque es el primero y el siguiente esta vacio
    if(bloque_anterior == NULL && bloque_siguiente->esta_vacio == true){
        consolidar_dos_bloques(bloque,bloque_siguiente);
    }
    //Me fijo si los dos bloques son distintos de NULL
    if(bloque_anterior != NULL && bloque_siguiente != NULL){

        //Me fijo si los dos bloques estan vacios
        if(bloque_anterior->esta_vacio == true && bloque_siguiente->esta_vacio){
            consolidar_tres_bloques(bloque_anterior,bloque,bloque_siguiente);
        }
        else if(bloque_anterior->esta_vacio == true){
            consolidar_dos_bloques(bloque_anterior,bloque);
        }
        else{
            consolidar_dos_bloques(bloque, bloque_siguiente);
        }

    }    
    //Me fijo si es el unico bloque ???

    return ;
}

void consolidar_dos_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque){

    primerBloque->tamanio_particion = primerBloque->tamanio_particion + segundoBloque->tamanio_particion;

    int indice = obtener_indice_particion(segundoBloque);

    //eliminar segundo bloque de la lista
	//@NAME: list_remove_and_destroy_element @DESC: Remueve un elemento de la lista de una determinada posicion y libera la memoria.
	list_remove_and_destroy_element(lista_memoria, indice, (void*)free);


    return ;
}

void consolidar_tres_bloques(t_bloque_memoria* primerBloque, t_bloque_memoria* segundoBloque, t_bloque_memoria* tercerBloque){

    primerBloque->tamanio_particion = primerBloque->tamanio_particion + segundoBloque->tamanio_particion + tercerBloque->tamanio_particion;

    int indiceUno = obtener_indice_particion(segundoBloque);
    int indiceDos = obtener_indice_particion(tercerBloque);

    //eliminar segundo bloque y tercer bloque de la lista
	list_remove_and_destroy_element(lista_memoria, indiceUno, (void*)free);
	list_remove_and_destroy_element(lista_memoria, indiceDos, (void*)free);

    return ;
}




/*Obtengo el tiempo actual en segundos*/ 
uint64_t get_timestamp(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t  x = (uint64_t)( (tv.tv_sec)*1000 + (tv.tv_usec)/1000 );
	return x;
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


//----------------------------DUMP DE LA MEMORIA--------------------------



//-----------------------------FIN DEL DUMP--------------------------------