#include <stdlib.h>
#include <stdio.h>
#include "FileSystem.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

bool noCumpleConRutasfs() {
	return (abrir_ruta(rutas_fs->pathDirectorioMetadataFs) < 0)
			| (abrir_ruta(rutas_fs->pathArchivoMetadataFs) < 0)
			| (abrir_ruta(rutas_fs->pathArchivoBitMap) < 0);
}
void crearFileSystemVacio() {

	crearMetadataFs();
	crearBitmap();
	crearDirectoriosParaFs();
	InicializarBloquesDeDatosFs();
	abrirBitmap();

}


void crearMetadataFs() {

	cargarMetadataFs("gamecard.config");
//se va a crear el directorio metadata
//analizar permisos para ese directorio por ahora todos pueden hacer cualquier cosa
	int status = mkdir(rutas_fs->pathDirectorioMetadataFs, 0777);
	//se pudo crear directorio metadata
	if (status == 0) {
		log_info(gameCard_logger, "se ha creado el directorio Matadata");
	}
// se van a crear el archivo metadata.bin
// w= crea un fichero en modo binario para escribir
	int archivoMetadata = open(rutas_fs->pathArchivoMetadataFs, "wb");
	write(archivoMetadata, metadata_fs->magicNumber, sizeof(metadata_fs->magicNumber));
	write(archivoMetadata,metadata_fs->tamanioBLoques, sizeof(metadata_fs->tamanioBLoques));
	write(archivoMetadata, &metadata_fs->cantidadBloques, sizeof(metadata_fs->cantidadBloques));
	close(archivoMetadata);
	log_info(gameCard_logger, " Se ha creado el archivo metadata.bin");
}

void cargarRutasFs() {

	log_info(gameCard_logger, "carganado las rutas correspondientes al fs");

	rutas_fs = malloc(sizeof(t_rutas_fs));
	rutas_fs->puntoDeMontaje = string_new();
	rutas_fs->pathDirectorioMetadataFs = string_new();
	rutas_fs->pathArchivoMetadataFs = string_new();
	rutas_fs->pathArchivoBitMap = string_new();
	rutas_fs->pathDirectorioFilesMetadata = string_new();
	rutas_fs->pathDirectorioBloques = string_new();

	string_append(&(rutas_fs->puntoDeMontaje), punto_montaje_fs);
	string_append(&(rutas_fs->pathDirectorioMetadataFs), rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioMetadataFs), "/Metadata");

	string_append(&(rutas_fs->pathArchivoMetadataFs),rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoMetadataFs), "/Metadata.bin");

	string_append(&(rutas_fs->pathArchivoBitMap), rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoBitMap), "/Bitmap.bin");

	string_append(&(rutas_fs->pathDirectorioFilesMetadata),rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioFilesMetadata), "/Files");

	string_append(&(rutas_fs->pathDirectorioBloques), rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioBloques), "/Blocks");

	log_info(gameCard_logger, "se han cargado todas las rutas correctamente");
}

int abrir_ruta(char *ruta) {
	return open(ruta, O_RDONLY);

}

void cargarMetadataFs(char *ruta) {

	log_info(gameCard_logger, "Cargando Metadata FileSystem");

	metadata_fs = malloc(sizeof(t_metadata_fs));
	//metadata_fs->magicNumber = string_new();

	char* tamanioBloque = string_new();
	char* cantidadBloques = string_new();
	char* magicNumber = string_new();

	//el archivo metadata.bin se parece al archivo de configuracion
	//se reutilizan estructuras
	t_config* configMetataNueva = config_create(ruta);

	string_append(&tamanioBloque,
			config_get_string_value(configMetataNueva, "BLOCK_SIZE"));
	string_append(&cantidadBloques,
			config_get_string_value(configMetataNueva, "BLOCKS"));
	string_append(&magicNumber,
			config_get_string_value(configMetataNueva, "MAGIC_NUMBER"));

	// atoi: convierte cadena de caracteres a un int
	metadata_fs->cantidadBloques = atoi(cantidadBloques);
	metadata_fs->tamanioBLoques = atoi(tamanioBloque);
	metadata_fs->magicNumber = magicNumber;

	log_info(gameCard_logger,"se ha cargado correctamente la metadata del FileSystem");

	config_destroy(configMetataNueva);
	free(tamanioBloque);
	free(cantidadBloques);
	free(magicNumber);

}


void crearBitmap() {

	int bytesBitmap= metadata_fs->cantidadBloques/8;

	if (bytesBitmap % 8 != 0 ){ bytesBitmap++;}

	char* bitarrayInicializador = malloc(bytesBitmap);

	bzero( bitarrayInicializador, bytesBitmap);

	int archBitmap = open(rutas_fs->pathArchivoBitMap,"wb");

	int i = write(archBitmap,bitarrayInicializador,bytesBitmap);

	if(i<bytesBitmap){

		log_info(gameCard_logger,"no se pudo copiar el bitamp");
		break;
	}

		close(archBitmap);
		free(bitarrayInicializador);


	log_info(gameCard_logger," Se ha creado el archivo bitmap.bin");

}

void crearDirectoriosParaFs() {

	//ver los permisos S_IRWXU
	int fueCreado = mkdir(rutas_fs->pathDirectorioFilesMetadata, 0777);
	if (fueCreado == -1) {
		perror("no se pudo crear el directorio /Files");
		log_info(gameCard_logger,
				"se ha producido un error no se pudo crear el directorio %s",
				rutas_fs->pathDirectorioFilesMetadata);
	}

	//de nuevo ver permisos
	int creado = mkdir(rutas_fs->pathDirectorioBloques, 0777);
	if (creado == -1) {
		perror("no se pudo crear el directorio /Blocks");
		log_info(gameCard_logger,
				"se ha producido un error no se pudo crear el directorio %s",
				rutas_fs->pathDirectorioBloques);
	}

log_info(gameCard_logger, "se han creado los directorios /Files y /Blocks");


}


bool existeDirectorio(char* ruta) {

	return abrir_ruta(ruta) > -1;
}



void InicializarBloquesDeDatosFs() {

	int numBloque=1;

	for (numBloque = 1; numBloque<= metadata_fs->cantidadBloques; numBloque++) {

		char* pathBloque = string_new();
		char* nombreBloque= string_new();


		string_append(&pathBloque, rutas_fs->pathDirectorioBloques);
		string_append(&pathBloque,"/");
		string_append(&nombreBloque,(string_itoa(numBloque)));
		string_append(&pathBloque,nombreBloque);
		string_append(&pathBloque, ".bin");

		FILE * nuevoBloque = fopen(pathBloque, "wb");
		fclose(nuevoBloque);

	}

}

/*********************************ACA OPERACIONES************************/

bool existePokemon(char* nombrePokemon) {

	char* ruta_pokemon= string_new();
	string_append(&ruta_pokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&ruta_pokemon,"/Pokemon/");
	string_append(&ruta_pokemon,nombrePokemon);
	return existeDirectorio(ruta_pokemon);

	}


void crearPokemon(t_new_pokemon* pokemon){

	int tamanioPokemon;


	int bloqueLibre= obtenerPrimerBloqueLibre();

	char* rutaBloqueLibre=string_new();
	char* nombreBloque=string_new();

   string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
   string_append(&rutaBloqueLibre,"/");
   string_append(&nombreBloque,(string_itoa(bloqueLibre)));
   string_append(&rutaBloqueLibre,nombreBloque);
   string_append(&rutaBloqueLibre, ".bin");
   int bloque = open(rutaBloqueLibre, "wb+");

	//lo que quiero copiar es menor al tamaño del bloque

   //copiar

   //si no es menor buscar otro bloque libre



}


void copiarEnArchivo(int fd,char* dato,int tamanioDato){

	write(fd,dato,tamanioDato);

}




void abrirBitmap() {

	int bitmap = open(rutas_fs->pathArchivoBitMap, O_RDWR);
	struct stat mystat;

	fstat(bitmap, &mystat);


	char *bmap;
	bmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
			bitmap, 0);

	int bytesBitmap = metadata_fs->cantidadBloques / 8;

	if (metadata_fs->cantidadBloques % 8 != 0){ bytesBitmap++;}

	bitarray = bitarray_create_with_mode(bmap, bytesBitmap,MSB_FIRST);


	log_info(gameCard_logger,"abriendo bitmap");
}


int  obtenerPrimerBloqueLibre(){

	for(int i; bitarray_get_max_bit(bitarray);i++){

		if (bitarray_test_bit(bitarray,i)==0){

			return i;

			}

	}


}


