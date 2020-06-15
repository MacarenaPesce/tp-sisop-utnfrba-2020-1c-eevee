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

	int status = mkdir(rutas_fs->pathDirectorioMetadataFs, 0777); //analizar permisos

	if (status == 0) {
		log_info(gameCard_logger, "no se ha creado el directorio Matadata");
	}

	FILE * archivoMetadata = fopen(rutas_fs->pathArchivoMetadataFs, "wb");

	char* lineaMetadataSize=string_new();
	string_append(&lineaMetadataSize,"BLOCK_SIZE=");
	string_append(&lineaMetadataSize,string_itoa(metadata_fs->tamanioBLoques));
	string_append(&lineaMetadataSize,"\n");

	fwrite(lineaMetadataSize, string_length(lineaMetadataSize),1, archivoMetadata);

	char* lineaMetadataBlock=string_new();
	string_append(&lineaMetadataBlock,"BLOCKS=");
	string_append(&lineaMetadataBlock,(string_itoa( metadata_fs->cantidadBloques)));
	string_append(&lineaMetadataBlock,"\n");

	fwrite(lineaMetadataBlock,string_length(lineaMetadataBlock), 1, archivoMetadata);

	char* lineaMetadataMagicNumber=string_new();
	string_append(&lineaMetadataMagicNumber,"MAGIC_NUMBER=");
	string_append(&lineaMetadataMagicNumber,metadata_fs->magicNumber);

	fwrite(lineaMetadataMagicNumber, string_length(lineaMetadataMagicNumber), 1,archivoMetadata);

	fclose(archivoMetadata);

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
	log_info(gameCard_logger,"quiero ver la ruta: %s",ruta);

	metadata_fs = malloc(sizeof(t_metadata_fs));

	char* tamanioBloque = string_new();
	char* cantidadBloques = string_new();
	char* magicNumber = string_new();

	//el archivo metadata.bin se parece al archivo de configuracion
	//se reutilizan estructuras
	t_config* configMetataNueva = config_create(ruta);

	string_append(&tamanioBloque,
			(string_itoa(config_get_int_value(configMetataNueva, "BLOCK_SIZE"))));

	string_append(&cantidadBloques,
			(string_itoa(config_get_int_value(configMetataNueva, "BLOCKS"))));

	string_append(&magicNumber,
			config_get_string_value(configMetataNueva, "MAGIC_NUMBER"));

	// atoi: convierte cadena de caracteres a un int
	metadata_fs->cantidadBloques = atoi(cantidadBloques);
	metadata_fs->tamanioBLoques = atoi(tamanioBloque);
	metadata_fs->magicNumber = magicNumber;

	log_info(gameCard_logger,"cant bloques: %d",metadata_fs->cantidadBloques);
	log_info(gameCard_logger,"tamanio bloque: %d",metadata_fs->tamanioBLoques);
	log_info(gameCard_logger,"nombre fs: %s",metadata_fs->magicNumber);

	log_info(gameCard_logger,"se ha cargado correctamente la metadata del FileSystem");

	config_destroy(configMetataNueva);
	free(tamanioBloque);
	free(cantidadBloques);
	free(magicNumber);

}


void crearBitmap() {

	log_info(gameCard_logger,"crear bitmap");

	FILE *bitmapArch = fopen(rutas_fs->pathArchivoBitMap,"w");

		int blocksChar = metadata_fs->cantidadBloques/8;

		if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}

		log_info(gameCard_logger,"tamanio blocksChar: %d",blocksChar);

		char* bitmapData = string_new();

		bitmapData=string_repeat('0',blocksChar);


		log_info(gameCard_logger,"aca veo bitmapData: %s",bitmapData);

		//log_info(gameCard_logger,"quiero ver el cero %s",string_itoa(cero));
		//string_append(&bitmapData,(string_repeat(string_itoa(0),blocksChar)));

		//log_info(gameCard_logger,"el cero como texto  %s",bitmapData);


		fwrite(&bitmapData,sizeof(char),string_length(bitmapData),bitmapArch);


		free(bitmapData);
		fclose(bitmapArch);


		log_info(gameCard_logger," Se ha creado el archivo bitmap.bin");

}

void crearDirectoriosParaFs() {


	int fueCreado = mkdir(rutas_fs->pathDirectorioFilesMetadata, 0777); //analizar permisos
	if (fueCreado == -1) {
		perror("no se pudo crear el directorio /Files");
		log_info(gameCard_logger,
				"se ha producido un error no se pudo crear el directorio %s",
				rutas_fs->pathDirectorioFilesMetadata);
	}


	int creado = mkdir(rutas_fs->pathDirectorioBloques, 0777); //analizar permisos
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


void agregarAparicionPokemonABloque(int bytesAcopiar, char* linea){

	if (bytesAcopiar<=0){return;}

	else{

	int bloqueLibre= obtenerPrimerBloqueLibre();

		char* rutaBloqueLibre=string_new();
		char* nombreBloque=string_new();

	   string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
	   string_append(&rutaBloqueLibre,"/");
	   string_append(&nombreBloque,(string_itoa(bloqueLibre)));
	   string_append(&rutaBloqueLibre,nombreBloque);
	   string_append(&rutaBloqueLibre, ".bin");
	   FILE *bloque = fopen(rutaBloqueLibre, "wb");

		 if (metadata_fs->tamanioBLoques>=bytesAcopiar){
		fwrite(linea,string_length(linea),1,bloque);
		 fclose(bloque);
		}
		 else{

			 fwrite(linea,metadata_fs->tamanioBLoques,1,bloque);
			 fclose(bloque);
			 int  bytesAcopiar= string_length(linea)- metadata_fs->tamanioBLoques;

			 agregarAparicionPokemonABloque(bytesAcopiar,linea);

		 }
	}
}




void crearPokemon(t_new_pokemon* pokemon){

	char* lineaPokemon= string_new();

	string_append(&lineaPokemon,(string_itoa(pokemon->coordenadas.posx)));
	string_append(&lineaPokemon,"-");
	string_append(&lineaPokemon,(string_itoa(pokemon->coordenadas.posy)));
	string_append(&lineaPokemon,"=");
	string_append(&lineaPokemon,(string_itoa(pokemon->cantidad)));

	int tamanioLinea=string_length(lineaPokemon);

	log_info(gameCard_logger, "aca quiero linea a copiar en el bloque: %s", lineaPokemon);


	if (tamanioLinea<=0){return;}

		else{

		int bloqueLibre= obtenerPrimerBloqueLibre();

		log_info(gameCard_logger, "primer bloque libre : %d",bloqueLibre);

			char* rutaBloqueLibre=string_new();
			char* nombreBloque=string_new();

		   string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
		   string_append(&rutaBloqueLibre,"/");
		   string_append(&nombreBloque,(string_itoa(bloqueLibre)));
		   string_append(&rutaBloqueLibre,nombreBloque);
		   string_append(&rutaBloqueLibre, ".bin");
		   FILE *bloque = fopen(rutaBloqueLibre, "wb");

		   log_info (gameCard_logger,"el bloque que abrimos es %s",rutaBloqueLibre);

		   log_info(gameCard_logger,"el tamanio de bloque es: %d",metadata_fs->tamanioBLoques);

		   log_info(gameCard_logger,"el tamanio de la linea es ", tamanioLinea);

			 if (metadata_fs->tamanioBLoques>tamanioLinea){
			fwrite(lineaPokemon,string_length(lineaPokemon),1,bloque);
			 fclose(bloque);

			 log_info(gameCard_logger, "se escribió el pokemon correctamente");
			}
			 else{

				 fwrite(lineaPokemon,metadata_fs->tamanioBLoques,1,bloque);
				 fclose(bloque);
				 int  bytesAcopiar= string_length(lineaPokemon)- metadata_fs->tamanioBLoques;

				 agregarAparicionPokemonABloque(bytesAcopiar,lineaPokemon);

			log_info(gameCard_logger,"se estan llenando bloques");

			 }
		}



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

	for (int i=1; i<=bitarray_get_max_bit(bitarray);i++){

		 bitarray_clean_bit(bitarray,i);}

	log_info(gameCard_logger,"abriendo bitmap");
}


int obtenerPrimerBloqueLibre(){

	log_info(gameCard_logger,"entrando a primer bloque libre");

	log_info(gameCard_logger,"dame tamanio de bitarray: %d",bitarray_get_max_bit(bitarray));

	for(int i=1; i<=bitarray_get_max_bit(bitarray);i++){

		log_info(gameCard_logger,"pos del bit del bitarray",i);

		if (bitarray_test_bit(bitarray,i)==0){

			log_info(gameCard_logger,"aca el bitarray es : %d", bitarray_test_bit(bitarray, i));
			return i;

			}

	}
}


