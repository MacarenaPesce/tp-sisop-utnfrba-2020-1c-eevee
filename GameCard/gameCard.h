
#ifndef GAMECARD_H_
#define GAMECARD_H_

#include "Contexto_gameCard.h"
#include "Herramientas_gameCard.h"


void recibir_get_pokemon_desde_gameboy(t_get_pokemon *);
void recibir_new_pokemon_desde_gameboy(t_new_pokemon *);
void recibir_catch_pokemon_desde_gameboy(t_catch_pokemon *);
void iniciar_servidor(void);
void esperar_cliente(int);
bool noCumpleConRutasfs();
void crearFileSystemVacio();
void crearMetadataDirectorios();

#endif /* GAMECARD_H_ */

