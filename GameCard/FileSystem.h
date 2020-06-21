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

typedef struct {
	char* puntoDeMontaje;
	char* pathDirectorioMetadataFs;
	char* pathArchivoMetadataFs;
	char* pathArchivoBitMap;
	char* pathDirectorioFilesMetadata;
	char* pathDirectorioBloques;
}  t_rutas_fs ;


t_rutas_fs * rutas_fs;

typedef struct {
	int32_t tamanioBLoques;
	int32_t cantidadBloques;
	char* magicNumber;
} t_metadata_fs;

t_metadata_fs * metadata_fs;

typedef struct {
	bool esDirectorio;
	int32_t tamanioArchivo;
	t_list* bloques;
	bool estaAbiertoArchivo;
} t_metadata_;

int32_t tamanioBitmap;

char *bmap;
int tamBmap;
t_bitarray* bitarray;
int bitmap;

void marcarBloqueOcupado(int bloqueLibre);
void cargarRutasFs();
int abrir_ruta(char *ruta);
void cargarMetadataFs(char *ruta);
void crearFileSystemVacio();
bool noCumpleConRutasfs();
void crearMetadataFs();
void inicializarBitmap();
void liberarMemoriaFs();
void crearBitmap();
bool existePokemon(char* pokemon);
bool existeDirectorio(char* ruta);
void crearDirectoriosParaFs();
void InicializarBloquesDeDatosFs();
void enteroACadena(uint32_t numero, char *bufer);
void mostrarContenidoFs();

//nuevas bitmap
void abrirBitmap();


//esto es auxiliar para ver contenido del bitmap

char * bitmapAux;

int  obtenerPrimerBloqueLibre();

void agregarAparicionPokemonABloque(int bytesAcopiar, char* linea);

void crearPokemon(t_new_pokemon* pokemon);

void crearMetadataDirectorioFiles();

//creacion del pokemon

t_list* bloquesMetadataPokemon;
int tamanioPokemon;
char* listaPokemon;
char* listAux;

char* stringAux;
void agregarAstring(void* elem);


void medirTamanioLineaPokemon(void* pokemon);
char*  cargarPokemon(t_new_pokemon* pokemon);
void copiarEnBloqueLibre(int bloqueLibre,char* lineaAcopiar);
void marcarBloqueOcupado(int bloqueLibre);
void agregarBloqueParaMetadataArchivo(int bloqueLibre);
void copiarEnBloqueLibre(int bloqueLibre,char* lineaAcopiar);

//manejo del ultimo elemento

int cantElemPokemon;

void crearMetadataArchPoke(char* pokemon, char* lineaPoke);

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

char* pokemon;
//modificar pokemon

t_list* obtenerBloquesPokemon(char* poke);

#endif /* FILESYSTEM_H_ */
