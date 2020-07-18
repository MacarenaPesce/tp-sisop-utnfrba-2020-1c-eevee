#include "Herramientas_gameCard.h"

void inicializar_logger(){
	//si pongo 1 muestra por consola si muestra 0 no.
	gameCard_logger = log_create("gamecard.log", "gameCard", 1, LOG_LEVEL_INFO);
	log_info(gameCard_logger, "Bienvenido al proceso Game Card");
}

void inicializar_archivo_de_configuracion(){
	config_game_card = config_create("../gamecard.config");
	if(config_game_card == NULL){
		terminar_game_card();

	}else{
		log_info(gameCard_logger, "Cargando los datos del archivo de configuración");
		obtener_valor_config(KEY_IP_BROKER, config_game_card, obtener_ip_broker);
		obtener_valor_config(KEY_PUERTO_BROKER, config_game_card,obtener_puerto_broker);
		obtener_valor_config(KEY_PUNTO_MONTAJE_TALLGRASS, config_game_card, obtener_punto_montaje_tallgrass);
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_CONEXION,config_game_card,obtener_tiempo_reintento_conexion);
		obtener_valor_config(KEY_TIEMPO_DE_REINTENTO_OPERACION , config_game_card, obtener_tiempo_reintento_operacion);
		obtener_valor_config(KEY_TIEMPO_RETARDO_OPERACION,config_game_card,obtener_tiempo_retardo_operacion);
		obtener_valor_config(KEY_CONFIG_ID, config_game_card, obtener_el_id);
		obtener_valor_config(KEY_CANT_BLOQUES , config_game_card,obtener_cantidad_bloques);
		obtener_valor_config(KEY_TAM_BLOQUE,config_game_card,obtener_tamanio_bloques);
		obtener_valor_config(KEY_NOMBRE_FS, config_game_card, obtener_nombre_fs);

	    config_destroy(config_game_card);
	    log_info(gameCard_logger,"Los datos del archivo de configuracion se han cargado correctamente");

	}

}

// a partir de la clave del archivo de configuracion
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void)){
	//si la clave se encuentra en el archivo de configuracion
	if (config_has_property(file, key)){
		//aplicar la funcion que se pasa por el tercer parametro
		obtener();
	}
}

void obtener_el_id(){
	id = config_get_int_value(config_game_card, KEY_CONFIG_ID);
	log_debug(gameCard_logger,"Mi ID es: %d",id);
}

void obtener_tiempo_reintento_conexion(){
	tiempo_reintento_conexion= config_get_int_value(config_game_card,KEY_TIEMPO_DE_REINTENTO_CONEXION);
}


void obtener_tiempo_reintento_operacion(){
	tiempo_reintento_operacion=config_get_int_value(config_game_card,KEY_TIEMPO_DE_REINTENTO_OPERACION);
}


void obtener_punto_montaje_tallgrass(){
	punto_montaje_fs =
			strdup(config_get_string_value(config_game_card, KEY_PUNTO_MONTAJE_TALLGRASS));
}

void obtener_ip_broker(){
	ip_broker =
			strdup(config_get_string_value(config_game_card, KEY_IP_BROKER));
}

void obtener_tiempo_retardo_operacion(){
	tiempo_retardo_operacion =
			config_get_int_value(config_game_card,KEY_TIEMPO_RETARDO_OPERACION);
}


void obtener_puerto_broker(){
	puerto_broker =
			strdup(config_get_string_value(config_game_card, KEY_PUERTO_BROKER));
}

void obtener_cantidad_bloques(){

	cantidadBloques=config_get_int_value(config_game_card,KEY_CANT_BLOQUES);

}

void obtener_tamanio_bloques(){

	tamanioDeBloque=config_get_int_value(config_game_card,KEY_TAM_BLOQUE);
}

void obtener_nombre_fs(){

	magicNumber=strdup(config_get_string_value(config_game_card, KEY_NOMBRE_FS));
}


void terminar_game_card(){
	/*Aca deberias liberar todas las estructuras que usaste!!*/

	log_info(gameCard_logger,"llegando al final");
	log_info(gameCard_logger,"liberando recursos");
	log_info(gameCard_logger,"finalizando GameCard");
	log_destroy(gameCard_logger);
	exit(EXIT_SUCCESS);
}
