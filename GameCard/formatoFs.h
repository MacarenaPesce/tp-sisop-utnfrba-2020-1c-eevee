/*
 * formatoFs.h
 *
 *  Created on: 28 jun. 2020
 *      Author: utnso
 */

#ifndef FORMATOFS_H_
#define FORMATOFS_H_

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

//char *bmap;
//int tamBmap;
t_bitarray* bitarray;
//int bitmap;

void cargarRutasFs();//ok
int abrir_ruta(char *ruta);
void cargarMetadataFs(char *ruta);//ok
void crearFileSystemVacio();//ok
bool noCumpleConRutasfs();//ok
void crearMetadataFs();//ok
void crearBitmap();//ok
bool existeDirectorio(char* ruta);//ok
void crearDirectoriosParaFs();//ok
void InicializarBloquesDeDatosFs();//ok
void crearMetadataDirectorioFiles();//ok
void abrirBitmap();

#endif /* FORMATOFS_H_ */
