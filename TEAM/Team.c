/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

void recibir_appeared_pokemon_desde_gameboy(t_appeared_pokemon * mensaje){
	log_info(team_logger,"Voy a recibir un pokemon y coordenadas");

	log_info(team_logger,"Me llego este pokemon: %s", mensaje->pokemon);
	log_info(team_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
	log_info(team_logger,"La coordenada Y es: %d", mensaje->coordenadas.posy);
	free(mensaje->pokemon);
	free(mensaje);
}

void definir_objetivo_global(){

	/*Leo del archivo de config y guardo los pokemon en la lista pokemones*/

	char **read_array = config_get_array_value(config, "POKEMON_ENTRENADORES");
	lista_config = list_create();
	pokemones_ordenada = list_create();
	string_iterate_lines(read_array, _imprimir);

	/*Creo una nueva lista con los pokemon agrupados por especie*/

	pokemones_ordenada = list_sorted(lista_config, (void*)ordenar);
	//list_iterate(pokemones_ordenada, mostrar);
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	char* unPokemon;
	unPokemon = list_get(pokemones_ordenada, 0);
	uint32_t contador = 0;
	lista_objetivos = malloc(sizeof(t_list));

	/*Empiezo a cargar a lista de objetivo global, con tipo y cantidad de cada uno*/
	log_info(team_logger,"Cargando el objetivo global... \n");
	char* especiePokemon;
	char* otroPokemon;
	int i = 0;
	while(pokemones_ordenada != NULL){
			especiePokemon = unPokemon;
			otroPokemon = list_get(pokemones_ordenada, i);
			if(otroPokemon == NULL){
				objetivo->especie = string_from_format("%s\0",especiePokemon);
				objetivo->cantidad = contador;
				log_info(team_logger,"Un tipo de pokemon es: %s y la cantidad es %i", objetivo->especie, objetivo->cantidad);
				list_add(lista_objetivos, (void*)objetivo);
				break;
			}
			if(string_equals_ignore_case(unPokemon,otroPokemon)){
				contador++;
				i++;
			}else{
				objetivo->especie = string_from_format("%s\0",especiePokemon);
				objetivo->cantidad = contador;
				log_info(team_logger,"Un tipo de pokemon es: %s y la cantidad es %i", objetivo->especie, objetivo->cantidad);
				list_add(lista_objetivos, (void*)objetivo);
				unPokemon = otroPokemon;
				contador = 1;
				i++;
			}
	}
	log_info(team_logger,"Objetivo global cargado\n");
	list_clean(lista_config);
}

void localizar_entrenadores_en_mapa(){
	char **read_array_posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char **read_array_objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	lista_entrenadores = list_create();

	string_iterate_lines(read_array_posiciones, _imprimir);
	//list_iterate(lista_config, mostrar);

	uint32_t i = 0;
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	while(lista_config != NULL){
		char* coordenada_char;
		coordenada_char = list_get(lista_config, i);
		if(coordenada_char != NULL) {
			int coordenada = atoi(coordenada_char);
			if(i == 0 || i % 2 == 0) {
			entrenador->posx = coordenada;
			i++;

		}else{
			entrenador->posy = coordenada;
			entrenador->estado = NUEVO;
			entrenador->objetivo = NULL;
			entrenador->id = i;
			list_add(lista_entrenadores, entrenador);

			/*CREO UN HILO POR ENTRENADOR*/
			pthread_t hilo;
			pthread_create(&hilo,NULL,(void*)jugar_con_el_entrenador,(void*)entrenador);

			printf("Se agrego un entrenador con id %i, pos x = %i, pos y = %i\n",entrenador->id, entrenador->posx, entrenador->posy);
			i++;
		}

		} else{
			break;
		}
	}
	list_clean(lista_config);

}




void * jugar_con_el_entrenador(t_entrenador * entrenador){
	printf("Soy un hilo para el entrenador \n");
	//TODO PLANIFICAR A PARTIR DE ACA..
	/*
	 * Para poder planificar un entrenador, se seleccionará el hilo del entrenador más cercano al Pokémon.
Cada movimiento en el mapa responderá a un ciclo de CPU, y este NO realizará movimientos
diagonales para llegar a la posición deseada. Para simular más a la realidad esta funcionalidad, se
deberá agregar un retardo de X segundos configurado por archivo de configuración.
	 */

	/*
	 * Para planificar a los distintos entrenadores se utilizarán los algoritmos FIFO, Round Robin y Shortest
job first con y sin desalojo. Para este último algoritmo se desconoce la próxima rafaga, por lo que se
deberá utilizar la fórmula de la media exponencial. A su vez, la estimación inicial para todos los
entrenadores será la misma y deberá poder ser modificable por archivo de configuración
	 */


	/*
	 * Cada entrenador al iniciar en el sistema entrará en estado New. A medida que el Team empiece a
recibir distintos Pokémon en el mapa despertará a los distintos entrenadores en estado New o en
Blocked (que estén esperando para procesar) pasandolos a Ready. Siempre se planificará aquel
entrenador que se encuentre sin estar realizando ninguna operación activamente y, en caso de
existir más de uno, sea el que más cerca se encuentre del objetivo.
A medida que cada entrenador se planifique (ya sea para moverse, intercambiar o atrapar un
Pokémon) entrarán en estado exec. En el contexto de nuestro trabajo practico no contemplaremos el
multiprocesamiento, esto implica que solo UN entrenador podrá estar en estado Exec en
determinado tiempo.
Cuando un entrenador en estado Exec finalice su recorrido y su ejecución planificada entrará en un
estado bloqueados. Este estado implica que el entrenador no tiene más tareas para realizar
momentáneamente.
Cuando un entrenador en estado Exec cumpla todos sus objetivos, pasará a estado Exit. Cuando
todos los entrenadores dentro de un Team se encuentren en Exit, se considera que el proceso Team
cumplió el objetivo global.
	 */
}


void iniciar_servidor(void){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	log_info(team_logger,"\nSe aceptó un nuevo cliente");

	t_packed * paquete = recibir_mensaje(socket_cliente);

	switch(paquete->cola_de_mensajes){
		case COLA_APPEARED_POKEMON:
			recibir_appeared_pokemon_desde_gameboy(paquete->mensaje);
			break;
		default:
			printf("Error\n");
	}

}


int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	config = config_create("team.config");
	configurar_signals();
	inicializar_listas();//sacar los leaks
	definir_objetivo_global();//sacar los leaks
	localizar_entrenadores_en_mapa();

	iniciar_servidor();

	//enviar_get();
	//conectarse_a_colas_de_broker();

	//liberar el config de arriba


}
