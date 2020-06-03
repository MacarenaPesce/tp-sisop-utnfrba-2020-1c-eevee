/*
 * FileSystem.c
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#include "FileSystem.h"

bool esta_vacio_fs(){

	cargarRutasFs();
}

void cargarRutasFs() {

	rutas_fs = malloc(sizeof(t_rutas_fs));
	rutas_fs->puntoDeMontaje = string_new();
	rutas_fs->pathDirectorioMetadataFs= string_new();
	rutas_fs->pathArchivoMetadataFs= string_new();
	rutas_fs->pathArchivoBitMap = string_new();
	rutas_fs->pathDirectorioFilesMetadata =string_new();
	rutas_fs->pathDirectorioBloques = string_new();

	//tomar path del punto de montaje (es un atributo extern)
	string_append(&(rutas_fs->puntoDeMontaje),punto_montaje_fs);

	//armar ruta directorio Metadata
	string_append(&(rutas_fs->pathDirectorioMetadataFs),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioMetadataFs),"/Metadata");

	//armar ruta archivo metadata.bin
	string_append(&(rutas_fs->pathArchivoMetadataFs),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoMetadataFs),"/Metadata.bin");

	//armar ruta archivo bitmap.bin
	string_append(&(rutas_fs->pathArchivoBitMap),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoBitMap),"/Bitmap.bin");

	//armar ruta Files Metadata
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),"/Files");

	//armar ruta de bloques
	string_append(&(rutas_fs->pathDirectorioBloques),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioBloques), "/Bloques");

}
