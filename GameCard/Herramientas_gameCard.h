/* adaptando game card segun lo que Tamara hizo en  a game team
 * 22 de abril del 2020*/

#ifndef HERRAMIENTAS_GAMECARD_H_
#define HERRAMIENTAS_GAMECARD_H_

#include "Contexto_gameCard.h"

void inicializar_logger();
void inicializar_archivo_de_configuracion();
void terminar_game_card();
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));
void obtener_tiempo_reintento_conexion();
void obtener_tiempo_reintento_operacion();
void obtener_punto_montaje_tallgrass();
void obtener_ip_broker();
void obtener_puerto_broker();
void capturar_signal(int);
void configurar_signal_gc();

#endif /* HERRAMIENTAS_GAMECARD_H_ */
