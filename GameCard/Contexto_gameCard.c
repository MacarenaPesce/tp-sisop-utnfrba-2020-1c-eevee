/*adaptación de team a game card
 * nuevamente Tamara Gracias!*/

#include "Contexto_gameCard.h"

/* Global variables*/
// lo numerico int
int tiempo_reintento_conexion;
int tiempo_reintento_operacion;
//lo que es string
char* punto_montaje_fs;
char* ip_broker;
char* puerto_broker;

t_log* gameCard_logger;
t_config* config_game_card;