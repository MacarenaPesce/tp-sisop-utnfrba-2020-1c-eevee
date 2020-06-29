#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include "Herramientas_gameCard.h"
#include <fcntl.h>
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include "formatoFs.h"

void marcarBloqueOcupado(int bloqueLibre);
bool existePokemon(char* pokemon);
int  obtenerPrimerBloqueLibre();
void crearPokemon(t_new_pokemon* pokemon);

//creacion del pokemon

t_list* bloquesMetadataPokemon;
int tamanioPokemon;
char* listaPokemon;
void agregarAstring(void* elem);

char*  cargarPokemon(t_new_pokemon* pokemon);
void marcarBloqueOcupado(int bloqueLibre);
//void agregarBloqueParaMetadataArchivo(char* bloqueLibre);
void crearMetadataArchPoke(char* pokemon, int tamanio);

//manejo del ultimo elemento

int cantElemPokemon;

void modificarPokemon(t_new_pokemon* poke);

//viendo bitmap de nuevo

int 	bitarrayfd;
char* bitmapData;

//****************

int espacioPokemon(char* lineaPoke);

bool noHayEspacioParaPokemon(char* listaPokemon);

bool entraEnBloque(char* lineaPokemon);

int fdBitmap;

int tamanioPokemon;

char* pokeLinea;
//modificar pokemon

//t_list* obtenerBloquesPokemon(char* poke);
void llenarListaBloquesPoke(char* pok);

char* pokemonEnMemoria;

char* pokemonAguardar;

char* nuevaPos;

char* posAcopiar;

int tamanioNuevaPos;

int nuevaCant;

/******/
void agregarCantidadNuevaAposicion(char* posicion);

//para manejar bloques libres del pokemon
int cantBloquesOcupadosPorPoke;

void persistirCambiosEnBloquesPropios(void* bloque);

int copiado, desde, hasta;

void persistirCambiosEnBloquesNuevos(char* bloqueNuevo);

//char* obtenerBloquesNuevos(int cantBloqNecesarios);

bool noHayEspacioParaModificaciones(char* posValidas);

int cantBloquesLibres();

void cantBloquesOcupadosPorPokemon(char* bloque);

int cantBloquesPoke;

void obtenerCantBloques(char* bloque);

char** obtenerbloquesMetadata(char* pokemon);

t_list* obtenerBloquesNuevos(int cantBloqNecesarios);

t_list* bloquesNuevos;

void copiarEnBloques(void* bloque);

int cantBlok;

void sumameBloques(char* bloq);

int ultimoBloque(char* pokeCompleto);

char* pokemon;

int cantElem;

char* copiarSinSaltoUltimo;

int cantBloquesNecesariosPara(char* pokeCompleto);

void quitarSaltoDeLinea(char* texto);

void cantidadElementos(char* text);

void cantidadElementos(char* text);

void copiarSinUltimoSalto(char* text);

void contardorDeBloquesOcupadosPorPokemon(char** bloksOcupadosPorPokemon);

void copiarPokemonAmemoria(t_list* bloksOcupadosPorPokemon);

bool estaPosicionEnMemoria(char* pokemonEnMemoria, char* nuevaPos);

void copiarEnBloque(char* bloqueLibre,char* lineaAcopiar);

void agregarBloqueParaMetadataArchivo(char* bloqueLibre);

void cambiarTamanioMetadata(char* pokemon,int tamanioAgregar);

void modificarBloquesMetadata(char* poke, t_list* bloquesPokemon);

void modificarPosicion(char* nuevaPos,int cantidad,char* pokemonEnMemoria);

void agregarNuevaPosicion(char* nuevaPos,int cantidad,char* pokemonEnMemoria);

void agregarAlista(char* bloque);

char* contenidoBloque;
char* traerAmemoriaUltimoBloque(char* ultBloque);

char* bloqEnMemo;

void agregarPosicionPokemonAbloquesNuevos();

void modificarBloquesMetadata(char* poke,t_list* bloquesPokemon);

void agregarPosicionEnUltimoBLoque();

int espacioLibreEnfs();


#endif /* FILESYSTEM_H_ */
