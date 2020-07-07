#include "gameCard.h"

int main() {

	inicializar_logger();
	inicializar_archivo_de_configuracion();

	cargarRutasFs();
	cargarMetadataFs("gamecard.config");

	if (noCumpleConRutasfs()) {
		log_info(gameCard_logger, "No existe el FileSytem requerido");
		log_info(gameCard_logger, "Creando estructura del FileSytem");
		crearFileSystemVacio();
	}

	abrirBitmap();
	inicializarSemaforoPokemon();

	//inicializarSemaforosMetadataYBitmap();

	convertirse_en_suscriptor_global_del_broker();

	/*Esto es lo necesario para comunicarse con gameboy*/
	int serv_socket = iniciar_servidor(PUERTO);

	crear_hilo_de_escucha_para_gameboy(serv_socket);

	desconectarFs();
	terminar_game_card();
}


