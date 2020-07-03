#include "gameCard.h"

int main() {

	inicializar_logger();
	inicializar_archivo_de_configuracion();

	/*cargarRutasFs();
	cargarMetadataFs("gamecard.config");

	if (noCumpleConRutasfs()) {
		log_info(gameCard_logger, "No existe el FileSytem requerido");
		log_info(gameCard_logger, "Creando estructura del FileSytem");
		crearFileSystemVacio();
	}

	abrirBitmap();*/

	/*En el enunciado se indica que la suscripcion se debe hacer al iniciar el proceso,
	 * supongo que es luego de hacer los seteos iniciales */
	convertirse_en_suscriptor_global_del_broker();


	/*Esto es lo necesario para comunicarse con gameboy*/
	int serv_socket = iniciar_servidor(PUERTO);

	crear_hilo_de_escucha_para_gameboy(serv_socket);

	terminar_game_card();

}

t_catch_pokemon*  picachuHardcodeadoAtrapar(char* pokemonNombre, int posx, int posy) {
	t_catch_pokemon* poke = (t_catch_pokemon*) malloc(sizeof(t_catch_pokemon));

	poke->pokemon = pokemonNombre;
	poke->coordenadas.posx = posx;
	poke->coordenadas.posy = posy;

	return poke;
}


//esto es solo para probar

t_new_pokemon* picachuHardcodeado(char* pokemonNombre, int posx, int posy,
		int cantidad) {
	t_new_pokemon* poke = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));

	poke->pokemon = pokemonNombre;
	poke->coordenadas.posx = posx;
	poke->coordenadas.posy = posy;
	poke->cantidad = cantidad;

	return poke;
}

t_new_pokemon* charmanderHardcodeado() {
	t_new_pokemon* poke = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));

	poke->pokemon = "Charmander";
	poke->coordenadas.posx = 987;
	poke->coordenadas.posy = 55;
	poke->cantidad = 107;

	return poke;
}

t_new_pokemon* picachuHardcodeadoLineaExisteParaModif() {
	t_new_pokemon* poke = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));

	poke->pokemon = "Picachu";
	poke->coordenadas.posx = 120;
	poke->coordenadas.posy = 7000;
	poke->cantidad = 9;

	return poke;
}

t_new_pokemon* picachuHardcodeadoLineaNoExisteParaModif() {
	t_new_pokemon* poke = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));

	poke->pokemon = "Picachu";
	poke->coordenadas.posx = 1;
	poke->coordenadas.posy = 100;
	poke->cantidad = 9;

	return poke;
}
