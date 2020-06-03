/*
 * FileSystem.h
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include "Herramientas_gameCard.h"
#include <fcntl.h>

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

typedef struct {
	bool esDirectorio;
	int32_t tamanioArchivo;
	t_list* bloques;
	bool estaAbiertoArchivo;
} t_metadata_;


void cargarRutasFs();
int abrir_ruta(char *ruta);
int esta_vacio_fs();
#endif /* FILESYSTEM_H_ */
