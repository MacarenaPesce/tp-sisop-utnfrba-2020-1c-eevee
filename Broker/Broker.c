#include "Broker.h"

int main(){

	cola_mensajes = list_create();
	printf("\n\n cantidad de elementos en lista %d",cola_mensajes->elements_count);

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	iniciar_servidor();

	/* TODO: Obtener este valor desde la config */
	int tamanio_memoria_inicial = 8000;

	//primer_bloque = AsignarMemoriaInicial(tamanio_memoria_inicial,lista_memoria);
	while(true){
	
	}
}
