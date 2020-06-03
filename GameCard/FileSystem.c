/*
 * FileSystem.c
 *
 *  Created on: 2 jun. 2020
 *      Author: utnso
 */

#include "FileSystem.h"

void cargarRutasFs() {

	log_info(gameCard_logger, "carganado las rutas correspondientes al fs");
	//logger(escribir_loguear,l_info,"Hi, bienvenido a GameCard!");
    //logger(escribir_loguear,l_info,"Por ahora solo valida si existe fs o no");
	//se tiene referncia por fuera para liberar memoria después
	rutas_fs = malloc(sizeof(t_rutas_fs));
	rutas_fs->puntoDeMontaje = string_new();
	rutas_fs->pathDirectorioMetadataFs= string_new();
	rutas_fs->pathArchivoMetadataFs= string_new();
	rutas_fs->pathArchivoBitMap = string_new();
	rutas_fs->pathDirectorioFilesMetadata =string_new();
	rutas_fs->pathDirectorioBloques = string_new();

	//logger(escribir_loguear,l_info,"validando contenido de las rutas propias del fs...");
	//tomar path del punto de montaje (es un atributo extern)
	string_append(&(rutas_fs->puntoDeMontaje),punto_montaje_fs);
	//logger(escribir_loguear,l_info,"El punto de montaje es: %s", rutas_fs->puntoDeMontaje);

	//armar ruta directorio Metadata
	string_append(&(rutas_fs->pathDirectorioMetadataFs),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioMetadataFs),"/Metadata");
	//logger(escribir_loguear,l_info,"Ruta Metadata del fs es: %s", rutas_fs->pathDirectorioMetadataFs);

	//armar ruta archivo metadata.bin
	string_append(&(rutas_fs->pathArchivoMetadataFs),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoMetadataFs),"/Metadata.bin");
	//logger(escribir_loguear,l_info,"Ruta metadata. bin es : %s", rutas_fs->pathArchivoMetadataFs);

	//armar ruta archivo bitmap.bin
	string_append(&(rutas_fs->pathArchivoBitMap),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoBitMap),"/Bitmap.bin");
	//logger(escribir_loguear,l_info,"Ruta metadata.bin es: %s", rutas_fs->pathArchivoBitMap);

	//armar ruta Files Metadata
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),"/Files");
	//logger(escribir_loguear,l_info,"ruta files es: %s", rutas_fs->pathDirectorioFilesMetadata);

	//armar ruta de bloques
	string_append(&(rutas_fs->pathDirectorioBloques),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioBloques),"/Blocks");
	//logger(escribir_loguear,l_info,"ruta bloques es : %s", rutas_fs->pathDirectorioBloques);

	log_info(gameCard_logger, "se han cargado todas las rutas correctamente");
}

int abrir_ruta(char *ruta){
	return open(ruta,O_RDONLY);

}

void cargarMetadataFs() {

	log_info(gameCard_logger, "Cargando Metadata FileSystem");

	metadata_fs = malloc(sizeof(t_metadata_fs));
	metadata_fs->magicNumber = string_new();

	char * tamanioBloque = string_new();
	char * cantidadBloques = string_new();
	char * magicNumber = string_new();

	//el archivo metadata.bin se parece al archivo de configuracion
	//se reutilizan estructuras
	t_config* config = config_create(rutas_fs->pathArchivoMetadataFs);

	string_append(&tamanioBloque,config_get_string_value(config,"TAMANIO_BLOQUES"));
	string_append(&cantidadBloques,config_get_string_value(config,"CANTIDAD_BLOQUES"));
	string_append(&magicNumber,config_get_string_value(config,"MAGIC_NUMBER"));

	// atoi: convierte cadena de caracteres a un int
	metadata_fs->cantidadBloques = atoi(cantidadBloques);
	metadata_fs->tamanioBLoques = atoi(tamanioBloque);
	metadata_fs->magicNumber = magicNumber;

	log_info(gameCard_logger, "se ha cargado correctamente la metadata del FileSystem");

}
