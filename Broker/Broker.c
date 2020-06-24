#include "Broker.h"

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	iniciar_servidor();


	asignar_memoria_inicial(8000);

}