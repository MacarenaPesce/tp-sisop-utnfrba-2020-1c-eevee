#include "includes.h"

int calcularBytes (t_packed* paquete){ 

    int bytes = 0;

	//Calculo los bytes segun que operacion tiene mi paquete

	//ME FALTA AGREGAR UN ENUM O ALGO PARA PODER SABER QUE OPERACION HACE, PAQUETE->OPERACION NO ES!!!

	//NO ESTOY SEGURA SI ELIMINAR PAQUETE ESTA BIEN, Y CUANDO HACER EL FREE PKMN PORQUE LE ESTOY RESERVANDO MEMORIA


    switch(paquete-> operacion){
			case NEW_POKEMON:
				puts(" Recibi un NEW POKEMON");
				
                	t_new_pokemon* pkmn;
					pkmn =(t_new_pokemon*)malloc(sizeof(t_new_pokemon));

					/* Apunto a los datos del mensaje */
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete*/
					eliminar_mensaje(paquete);

				//4 bytes por cada uint_32, y tengo 4 uint_32 -> 4 * 4 = 16 
				//mas el largo del nombre del pokemon

                bytes = 16 + (strlen(pkmn->pokemon));

				return bytes;
			case LOCALIZED_POKEMON:
				puts(" Recibi un LOCALIZED POKEMON");

					t_localized_pokemon* pkmn;
					pkmn =(t_localized_pokemon*)malloc(sizeof(t_localized_pokemon));

					/* Apunto a los datos del mensaje */
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete*/
					eliminar_mensaje(paquete);
				
				//tengo 8 uint_32 -> 4 * 8 = 32
				//mas el largo del nombre por pokemon

                bytes = 32 + (strlen(pkmn->pokemon));

				return bytes;
			case GET_POKEMON:
				puts(" Recibi un GET POKEMON");

					t_get_pokemon* pkmn;
					pkmn =(t_get_pokemon*)malloc(sizeof(t_get_pokemon));

					/* Apunto a los datos del mensaje */
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete*/
					eliminar_mensaje(paquete);
                
				//tengo 1 sólo uint_32 -> 4 bytes
				//mas el largo del nombre del pokemon

				bytes = 4 + (strlen(pkmn->pokemon));

				return bytes;
			case APPEARED_POKEMON:
				puts(" Recibi un APPEARED POKEMON");

                	t_appeared_pokemon* pkmn;
					pkmn =(t_appeared_pokemon*)malloc(sizeof(t_appeared_pokemon));

					/* Apunto a los datos del mensaje */
					pkmn = paquete->mensaje;

					/*Libero la memoria del paquete*/
					eliminar_mensaje(paquete);
                
				//tengo 3 uint_32 -> 4 * 3 = 12
				//mas el largo del nombre del pokemon

				bytes = 12 + (strlen(pkmn->pokemon));

				return bytes;
			case CATCH_POKEMON:
				puts(" Recibi un CATCH POKEMON");

                //bytes = ;

				return bytes;
			case CAUGHT_POKEMON:
				puts(" Recibi un CAUGHT POKEMON");

                //bytes = ;

				return bytes;
			default:
				break;
		}


    return bytes;
}