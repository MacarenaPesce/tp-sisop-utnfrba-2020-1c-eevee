/*
 * Team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Team.h"

int main(){

	inicializar_logger();
	inicializar_archivo_de_configuracion();
	configurar_signals();

	logger(escribir_loguear,l_info,"\nTengo que ser siempre el mejor");
	logger(escribir_loguear,l_info,"Mejor que nadie masss");
	logger(escribir_loguear,l_info,"Atraparlos mi lema es");
	logger(escribir_loguear,l_info,"Entrenarlos mi ideal");
	logger(escribir_loguear,l_info,"Yo viajare de aqui alla");
	logger(escribir_loguear,l_info,"Buscando hasta el finnnn");
	logger(escribir_loguear,l_info,"OH POKEMON, yo te entendere");
	logger(escribir_loguear,l_info,"Tu poder interiorrrrrrr");

	logger(escribir_loguear,l_error,"POKEMON! TENGO QUE ATRAPARLOS (SOLOSS TU Y YOOO");
	logger(escribir_loguear,l_error,"NUESTRO DESTINO ASI ESSSS!!!!!");

}
