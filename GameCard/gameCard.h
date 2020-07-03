
#ifndef GAMECARD_H_
#define GAMECARD_H_

#include "Contexto_gameCard.h"
#include "Conexion_con_gameboy.h"
#include "Herramientas_gameCard.h"
#include "FileSystem.h"
#include "Conexion_con_broker.h"
#include "Funciones_de_conexion.h"

bool noCumpleConRutasfs();
void crearFileSystemVacio();
void crearMetadataDirectorios();

/***********aux*********/
t_new_pokemon* picachuHardcodeado();
t_new_pokemon* picachuHardcodeadoLineaExisteParaModif();
t_new_pokemon* picachuHardcodeadoLineaExisteParaModif();
t_new_pokemon* charmanderHardcodeado();
t_new_pokemon* picachuHardcodeado(char* pokemon,int posx,int posy,int cantidad);
t_catch_pokemon*  picachuHardcodeadoAtrapar(char* pokemonNombre, int posx, int posy);


#endif /* GAMECARD_H_ */

