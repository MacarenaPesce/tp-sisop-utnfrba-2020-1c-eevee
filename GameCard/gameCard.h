
#ifndef GAMECARD_H_
#define GAMECARD_H_

#include "Contexto_gameCard.h"
#include "Herramientas_gameCard.h"


void recibir_get_pokemon_desde_gameboy(t_get_pokemon *);
void recibir_new_pokemon_desde_gameboy(t_new_pokmemon);
void recibir_catch_pokemon_desde_gameboy(t_catch_pokemon *);
void iniciar_servidor(void);
void esperar_cliente(int);

bool esta_vacio_fs(char* path_fs);

#endif /* GAMECARD_H_ */

