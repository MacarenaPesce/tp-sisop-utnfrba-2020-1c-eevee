/*
 * formatoFs.h
 *
 *  Created on: 28 jun. 2020
 *      Author: utnso
 */

#ifndef FORMATOFS_H_
#define FORMATOFS_H_

#include "Herramientas_gameCard.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

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

char* bitmapData;
struct stat mystat;
char* bmap;
//int tamBmap;
t_bitarray* bitarray;
//int bitmap;

t_dictionary* semaforosPokemon;

void cargarRutasFs();//ok
int abrir_ruta(char *ruta);
void cargarMetadataFs();//ok
void crearFileSystemVacio();//ok
bool noCumpleConRutasfs();//ok
void crearMetadataFs();//ok
void crearBitmap();//ok
bool existeDirectorio(char* ruta);//ok
void crearDirectoriosParaFs();//ok
void InicializarBloquesDeDatosFs();//ok
void crearMetadataDirectorioFiles();//ok
void abrirBitmap();
void inicializarSemaforosBitmapYMetadata();
int destruir_lista(char * bloques);

#endif /* FORMATOFS_H_ */
