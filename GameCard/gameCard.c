#include "gameCard.h"
#include <stdbool.h>
#include "FileSystem.h"
#include "Pokebola.h"

void recibir_get_pokemon_desde_gameboy(t_get_pokemon * mensaje){
	log_info(gameCard_logger,"Recibo el pokemon: %s de Game Boy",mensaje->pokemon);
	free(mensaje->pokemon);
	free(mensaje);
}

void recibir_catch_pokemon_desde_gameboy(t_catch_pokemon *mensaje){
	log_info(gameCard_logger,"Voy a recibir un pokemon y coordenadas");
	log_info(gameCard_logger,"Me llego este pokemon: %s", mensaje->pokemon);
	log_info(gameCard_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
	log_info(gameCard_logger,"La coordenada Y es: %d", mensaje->coordenadas.posy);
	free(mensaje->pokemon);
	free(mensaje);
}

void recibir_new_pokemon_desde_gameboy(t_new_pokemon *mensaje){
	    log_info(gameCard_logger,"Voy a recibir un pokemon,su corrdenada y cantidad");
		log_info(gameCard_logger,"Me llego este pokemon: %s", mensaje->pokemon);
		log_info(gameCard_logger,"La coordenada X es: %d", mensaje->coordenadas.posx);
		log_info(gameCard_logger,"La coordenada Y es: %d", mensaje->coordenadas.posy);
		log_info(gameCard_logger,"La cantidad del pokemon en esa coordenada es:%d",mensaje->cantidad);
		free(mensaje->pokemon);
		free(mensaje);
}

void iniciar_servidor(void){
	//se inicia gamecard como proceso servidor
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

	//me conecto con el cliente
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	log_info(gameCard_logger,"se aceptó un nuevo cliente");

	t_packed * paquete = recibir_mensaje(socket_cliente);

		switch(paquete->cola_de_mensajes){
			case  COLA_GET_POKEMON :
				recibir_get_pokemon_desde_gameboy(paquete->mensaje);
				break;
			case  COLA_CATCH_POKEMON :
				recibir_catch_pokemon_desde_gameboy(paquete->mensaje);
				break;
			case COLA_NEW_POKEMON :
				recibir_new_pokemon_desde_gameboy(paquete->mensaje);
				break;

			default:
				printf("Error, cola de mensajes desconocida: %d\n",paquete->cola_de_mensajes);
						break;
				}

		}

int  main () {

	//analizar fugas de memoria

	inicializar_logger ();
	inicializar_archivo_de_configuracion ();

	//valida si existen las rutas propias del fs
	cargarRutasFs();
	if ( noCumpleConRutasfs()){
		log_info(gameCard_logger,"no existe el FileSytem requerido");
		log_info(gameCard_logger,"Inicializando FileSystem requerido");
		crearFileSystemVacio();

	    //free(metadata_fs->magicNumber);
		//free(metadata_fs);
		//free(gameCard_logger);
		//free(config_game_card);
		//free(rutas_fs);

		}

	//free(metadata_fs->magicNumber);


	return 0;

	/*
	config_destroy(config_game_card);
	free(rutas_fs);
	free(metadata_fs);
	free(gameCard_logger);*/



	//ANALIZAR ADECUADAMENTE LA CONEXION

	//aca intentará suscribirse globalmente al Broker
	//a las colas de mensaje NEW_POKEMON, CATCH_POKEMON,GET_POKEMON

	//DEBERÍA RECIBIR ACK DEL BROKER

	//DEBE ESPERAR RECIBIR UN MENSAJE DEL BROKER

	//AL RECIBIR UN MENSAJE DE CUALQUIER HILO DEL BROKER
	//ENVIAR ACK AL BROKER
	//CREAR HILO QUE ATIENDA DICHA SOLICITUD
    //DEBE EVALUARSE QUE TIPO DE SOLICITUD ES
	//VOLVER A ESTAR A LA ESCUCHA DE NUEVOS MENSAJES DE LA COLA DE MENSAJES

	//Y TAMMBIÉN TENDRA UN SOCKET ESCUCHA PARA EL GAMECARD


	//carga la estructura metadata_fs con el metadata.bin
	// todo cargarMetadatoFs(rutas_fs->pathArchivoMetadataFs);

	  //  free(metadata_fs->magicNumber);



	//liberarMemoriaFs();

}
