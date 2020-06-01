#ifndef HERRAMIENTAS_BROKER_H_
#define HERRAMIENTAS_BROKER_H_

#include "includes.h"

void inicializar_logger();
void inicializar_archivo_de_configuracion();
void obtener_valor_config(char* key, t_config* file, void(*obtener)(void));


void obtener_tamanio_memoria();
void obtener_tamanio_minimo_particion();
void obtener_algoritmo_memoria();
void obtener_algoritmo_reemplazo();
void obtener_algoritmo_particion_libre();
void obtener_la_ip_broker();
void obtener_el_puerto_broker();
void obtener_frecuencia_compactacion();
void obtener_el_log_file();


void configurar_signals(void);
void capturar_signal(int signo);
void terminar_broker_correctamente();





#endif /* HERRAMIENTAS_BROKER_H_ */



/*
TAMANO_MEMORIA               [Numérico]  Tamaño de la memoria en bytes
TAMANO_MINIMO_PARTICION      [Numérico] Tamaño mínimo de la partición en bytes
ALGORITMO_MEMORIA            [String] El tipo de algoritmo de administración de memoria que se va a utilizar (PARTICIONES/BS)
ALGORITMO_REEMPLAZO          [String] El tipo de algoritmo de reemplazo de memoria que se va a utilizar(FIFO/LRU)
ALGORITMO_PARTICION_LIBRE    [String] El tipo de algoritmo de seleccion de particion libre a utilizar(FF/BF)
IP_BROKER                    [String] El IP del servidor del proceso Broker
PUERTO_BROKER                [Numérico] El puerto del servidor del proceso Broker
FRECUENCIA_COMPACTACION      [Numérico] Cantidad de búsquedas fallidas previa compactación
LOG_FILE                     [String] Path del archivo de log donde se almacenará el log obligatorio
*/
