/*
 * FileSystem.c
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#include "FileSystem.h"

void cargarRutasFs() {

	logger(escribir_loguear,l_info,"Hi, bienvenido a GameCard!");
    logger(escribir_loguear,l_info,"Por ahora solo valida si existe fs o no");
	//se tiene referncia por fuera para liberar memoria después
	rutas_fs = malloc(sizeof(t_rutas_fs));
	rutas_fs->puntoDeMontaje = string_new();
	rutas_fs->pathDirectorioMetadataFs= string_new();
	rutas_fs->pathArchivoMetadataFs= string_new();
	rutas_fs->pathArchivoBitMap = string_new();
	rutas_fs->pathDirectorioFilesMetadata =string_new();
	rutas_fs->pathDirectorioBloques = string_new();

	logger(escribir_loguear,l_info,"validando contenido de las rutas propias del fs...");
	//tomar path del punto de montaje (es un atributo extern)
	string_append(&(rutas_fs->puntoDeMontaje),punto_montaje_fs);
	logger(escribir_loguear,l_info,"El punto de montaje es: %s", rutas_fs->puntoDeMontaje);

	//armar ruta directorio Metadata
	string_append(&(rutas_fs->pathDirectorioMetadataFs),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioMetadataFs),"/Metadata");
	logger(escribir_loguear,l_info,"Ruta Metadata del fs es: %s", rutas_fs->pathDirectorioMetadataFs);

	//armar ruta archivo metadata.bin
	string_append(&(rutas_fs->pathArchivoMetadataFs),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoMetadataFs),"/Metadata.bin");
	logger(escribir_loguear,l_info,"Ruta metadata. bin es : %s", rutas_fs->pathArchivoMetadataFs);

	//armar ruta archivo bitmap.bin
	string_append(&(rutas_fs->pathArchivoBitMap),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoBitMap),"/Bitmap.bin");
	logger(escribir_loguear,l_info,"Ruta metadata.bin es: %s", rutas_fs->pathArchivoBitMap);

	//armar ruta Files Metadata
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),"/Files");
	logger(escribir_loguear,l_info,"ruta files es: %s", rutas_fs->pathDirectorioFilesMetadata);

	//armar ruta de bloques
	string_append(&(rutas_fs->pathDirectorioBloques),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioBloques),"/Blocks");
	logger(escribir_loguear,l_info,"ruta bloques es : %s", rutas_fs->pathDirectorioBloques);

}

int abrir_ruta(char *ruta){
	return open(ruta,O_RDONLY);

}


int esta_vacio_fs(){


//	carga rutas propias del fs en un struct
	cargarRutasFs();

 //valida si existen las rutas propias del fs

	if (
			(abrir_ruta(rutas_fs->pathDirectorioMetadataFs) < 0 ) |
			(abrir_ruta(rutas_fs->pathArchivoMetadataFs)<0 )|
			  (abrir_ruta(rutas_fs->pathArchivoBitMap)<0) |
			     ( abrir_ruta(rutas_fs->pathDirectorioBloques)<0 )){
		perror("no existe ruta, no existe el fs");
		return -1;
	}

	return 0;
}
