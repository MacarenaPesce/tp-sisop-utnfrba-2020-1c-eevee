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



void buddy_funcionamiento(t_mensaje_cola* estructura_mensaje){

	/*Me fijo si cumple con el tamaño minimo de particion*/
	int bytes_a_alojar = tamanio_a_alojar(estructura_mensaje->tamanio_mensaje);

	/*Busco el numero potencia de 2 mas cercano*/
	int bytes_potencia_dos = tamanio_potencia_dos(bytes_a_alojar);

	/*Alojo la partición según el funcionamiento de buddies*/
	asignar_bloque_BS(estructura_mensaje, bytes_potencia_dos);


    return;
}

void asignar_bloque_BS(t_mensaje_cola* estructura_mensaje, int tamanio_particion){

	return;
}

void reemplazar_bloque_BS(t_mensaje_cola* estructura_mensaje){

	return;
}

void consolidar_buddies(){

	return ;
}

void particionar_bloque_buddies(){

	return ;
}

void encontrar_padre(){

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