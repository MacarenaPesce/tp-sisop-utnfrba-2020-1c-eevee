#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "formatoFs.h"
#include <sys/stat.h>
#include <sys/types.h>

// semaforos

pthread_mutex_t mutexSemPokemones;
t_dictionary* semaforosPokemones;
pthread_mutex_t semMutexBitmap;

char* lineaBloques;

void inicializarSemaforosParaPokemon();
void agregarSemaforoPokemon(char* poke);
void eliminarSemaforoPokemon(char* poke);

//fin semaforos

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
void crearMetadataArchPoke(char* pokemon, int tamanio, t_list* bloques);

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

extern char* pokemonEnMemoria;

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

void agregarPosicionPokemonAbloquesNuevos(char* ultBloque, char* stringAcopiar,
		int espacioOcupadoBloque, int espacioEnBloque, int espacioNuevaLinea, int nuevoSizeMetadata);

void modificarBloquesMetadata(char* poke,t_list* bloquesPokemon);

void agregarPosicionEnUltimoBLoque(char* ultBloq,char* stringAcopiar,int espacioNuevaLinea);

int espacioLibreEnfs();

bool estaAbiertoArchivo(char* pokemon);

void capturarPokeEnPos(char* posicion);

void vaciarBloques(t_list* bloquesMetadataPokemon);

void marcarBloquesLibres(t_list* bloquesMetadataPokemon);

void vaciarBloques(t_list* bloquesMetadataPokemon);

void marcarBloqueLibreBitmap(int numBloque);

void limpiarBloque(char* bloq);

void eliminarMetadataPokemon(char* pokemon);

t_list* obtenerPosicionesPokemon(char* pokemon);

void abrirArchivo(char* poke);
void cerrarArchivo(char* poke);
uint32_t capturarPokemon(t_catch_pokemon* pokeAatrapar);

void agregarPosicionSinSaltoDeLinea(int espacioEnBloque, int espacioNuevaLinea,
		char* ultBloque, int nuevoSizeMetadata);

//semaforos

//localized

t_list* pokemonesParaLocalized;

void agregarPosicionAlistaParaLocalized(char* posicion);

void crearDirectorioPokemon(char* pokemon);

int obtenerEspacioMetadata(char* pokemon);

void copiarPokemonEnMemoria(void* unBloque);

void liberarMemoria();
#endif /* FILESYSTEM_H_ */
