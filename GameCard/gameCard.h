
#ifndef GAMECARD_H_
#define GAMECARD_H_

#include "Contexto_gameCard.h"
#include "Herramientas_gameCard.h"

void recibir_get_pokemon_desde_gameboy(int, int);
void recibir_new_pokemon_desde_gameboy(int, int);
void recibir_catch_pokemon_desde_gameboy(int, int);
void iniciar_servidor(void);
void esperar_cliente(int);

#endif /* GAMECARD_H_ */
