#include "gameCard.h"
#include <signal.h>

int main() {

	/*se asocia con terminarCorrectamenteGameCard() para poder liberar memoria
	 * correctamente cuando finaliza el procese game card*/

	//interrupcion del programa ctrl+c
	signal (SIGINT, terminarCorrectamenteGameCard);
	//señal por comando kill desde consola: fin de proceso
	signal (SIGTERM, terminarCorrectamenteGameCard);
	//salir del programa con ctrl+barra (que no es el 7)
	signal (SIGQUIT, terminarCorrectamenteGameCard);

	inicializar_logger();
	inicializar_archivo_de_configuracion();

	cargarRutasFs();
	cargarMetadataFs();

	if (noCumpleConRutasfs()) {
		log_info(gameCard_logger, "No existe el FileSytem requerido");
		log_info(gameCard_logger, "Creando estructura del FileSytem");
		crearFileSystemVacio();
	}

	abrirBitmap();
	inicializarSemaforosParaPokemon();

	/*Esto es lo necesario para comunicarse con gameboy*/
	int serv_socket = iniciar_servidor(PUERTO);

	convertirse_en_suscriptor_global_del_broker();

	crear_hilo_de_escucha_para_gameboy(serv_socket);


}


void terminarCorrectamenteGameCard(){

	log_info(gameCard_logger,"se intenta cerrar el proceso game card");

	desconectarFs();
	terminar_game_card();
}
