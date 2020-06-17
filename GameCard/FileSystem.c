#include <stdlib.h>
#include <stdio.h>
#include "FileSystem.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <commons/collections/list.h>

bool noCumpleConRutasfs() {
	return (abrir_ruta(rutas_fs->pathDirectorioMetadataFs) < 0)
			| (abrir_ruta(rutas_fs->pathArchivoMetadataFs) < 0)
			| (abrir_ruta(rutas_fs->pathArchivoBitMap) < 0);
}

void crearFileSystemVacio() {

	crearMetadataFs();
	crearBitmap();
	crearDirectoriosParaFs();
	crearMetadataDirectorioFiles();
	InicializarBloquesDeDatosFs();

}


void crearMetadataFs() {

int status= mkdir(rutas_fs->pathDirectorioMetadataFs, 0777); //analizar permisos

	if (status == 0) {
		log_info(gameCard_logger, "Se ha creado el directorio Matadata");
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
	printf("quiero ver mg:%s", metadata_fs->magicNumber);
	string_append(&lineaMetadataMagicNumber,metadata_fs->magicNumber);


	fwrite(lineaMetadataMagicNumber, string_length(lineaMetadataMagicNumber), 1,archivoMetadata);

	fclose(archivoMetadata);

	log_info(gameCard_logger, " Se ha creado el archivo metadata.bin de FileSystem");
}

void cargarRutasFs() {

	log_info(gameCard_logger, "Carganado las rutas correspondientes al FileSystem");

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

	log_info(gameCard_logger, "Se han cargado todas las rutas correctamente");
}

int abrir_ruta(char *ruta) {
	return open(ruta, O_RDONLY);

}

void cargarMetadataFs(char *ruta) {

	log_info(gameCard_logger, "Cargando Metadata del FileSystem");

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

	log_info(gameCard_logger,"Se ha cargado correctamente la metadata del FileSystem");

	config_destroy(configMetataNueva);
	free(tamanioBloque);
	free(cantidadBloques);
	free(magicNumber);

}


void crearBitmap() {

	log_info(gameCard_logger,"Iniciando creación del bitmap");

	FILE *bitmapArch = fopen(rutas_fs->pathArchivoBitMap,"w");

		int blocksChar = metadata_fs->cantidadBloques/8;

		if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}

		char* bitmapData = string_new();

		bitmapData=string_repeat('0',blocksChar);

		fwrite(&bitmapData,sizeof(char),string_length(bitmapData),bitmapArch);

		free(bitmapData);
		fclose(bitmapArch);

		log_info(gameCard_logger," Se ha creado el archivo bitmap.bin");

}

void crearDirectoriosParaFs() {

	int fueCreado = mkdir(rutas_fs->pathDirectorioFilesMetadata, 0777); //analizar permisos
	if (fueCreado == 0) {
		log_info(gameCard_logger,"Se ha creado el directorio /Files");}

	int creado = mkdir(rutas_fs->pathDirectorioBloques, 0777); //analizar permisos
			if (creado == 0) {
		log_info(gameCard_logger, "Se ha creado el directorio /Blocks");}

}


void crearMetadataDirectorioFiles(){


	char* rutaMetaFiles=string_new();

	string_append(&rutaMetaFiles,rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetaFiles,"/Metadata.bin");

	FILE * archivoMetadata = fopen(rutaMetaFiles, "wb");

		char* lineaDirectory=string_new();

		string_append(&lineaDirectory,"Directory=Y");
		fwrite(lineaDirectory, string_length(lineaDirectory),1, archivoMetadata);

		fclose(archivoMetadata);

		log_info(gameCard_logger, "Se ha creado el archivo metadata.bin del directorio Files");

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


bool existePokemon(char* nombrePokemon) {

	char* ruta_pokemon= string_new();
	string_append(&ruta_pokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&ruta_pokemon,nombrePokemon);
	return existeDirectorio(ruta_pokemon);

	}

bool cumpleTamanio(int espacioBloque,int espacioAocupar){

	return espacioAocupar<=espacioBloque;
}

int cantBloquesLibres(){

	int libres=0;

	for(int j=1;j<=bitarray_get_max_bit(bitarray);j++){

		if ( bitarray_test_bit(bitarray,j)==0){

		libres=libres+1;
	}
	}

	return libres;
}


int espacioPokemon(t_list* listaPokemon){

	int espacioPoke=0;

	for(int i=0;i<=list_size(listaPokemon);i++){

			int espacioPoke=espacioPoke+sizeof(list_get(listaPokemon,i));
		}

	return espacioPoke;
}


bool noHayEspacioParaPokemon(t_list* listaPokemon){

	int espacioDisponible=(metadata_fs->tamanioBLoques)*cantBloquesLibres();

	return espacioDisponible<espacioPokemon(listaPokemon);

}


t_list*  cargarPokemon(t_new_pokemon* pokemon){

	    char* posx= string_itoa(pokemon->coordenadas.posx);
		char* posy=string_itoa(pokemon->coordenadas.posy);
		char* guion="-";
		char* igual="=";
		char* cantidad=string_itoa(pokemon->cantidad);

		t_list* listaPokemon= list_create();

		list_add(listaPokemon,posx);
		list_add(listaPokemon,guion);
		list_add(listaPokemon,posy);
		list_add(listaPokemon,igual);
		list_add(listaPokemon,cantidad);

		return listaPokemon;

}

void copiarEnBloqueLibre(int bloqueLibre,char* lineaAcopiar){

	log_info(gameCard_logger, "aca entro al archivo");

	    char* rutaBloqueLibre=string_new();
		char* nombreBloque=string_new();

		string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloqueLibre,"/");
		string_append(&nombreBloque,(string_itoa(bloqueLibre)));
		string_append(&rutaBloqueLibre,nombreBloque);
		string_append(&rutaBloqueLibre, ".bin");
		FILE *bloque = fopen(rutaBloqueLibre, "wb");

		fwrite(lineaAcopiar,string_length(lineaAcopiar),1,bloque);
		fclose(bloque);

}

void marcarBloqueOcupado(int bloqueLibre){

	bitarray_set_bit(bitarray,bloqueLibre);

}

void agregarBloqueParaMetadataArchivo(int bloqueLibre){

	list_add(bloquesMetadataPokemon,string_itoa(bloqueLibre));
}


void medirTamanioLineaPokemon(void* pokemon){

	char* elem=pokemon;

	//string_append(&listAux,elem);

	log_info(gameCard_logger,"aca quiero ver con que valor llega tamanioPokemen %d:",tamanioPokemon);

	tamanioPokemon=tamanioPokemon+string_length(elem);

	log_info(gameCard_logger,"tamanio + longitud del nuevo elemento : %d", tamanioPokemon);

	if (tamanioPokemon<=metadata_fs->tamanioBLoques){

		log_info(gameCard_logger,"tamanio Poke menor a bloque es:%d",tamanioPokemon);
		string_append(&listaPokemon,elem);

		log_info(gameCard_logger,"aca linea paó a ser: %s",listaPokemon);
	}

	else {

		int bloqueLibre=obtenerPrimerBloqueLibre();

		log_info(gameCard_logger,"el primer bloque libre es: %d",bloqueLibre);

		copiarEnBloqueLibre(bloqueLibre,listaPokemon);

		log_info(gameCard_logger, "se ha copiado %s , correctamente en el bloque %d",listaPokemon,bloqueLibre);

		marcarBloqueOcupado(bloqueLibre);

		agregarBloqueParaMetadataArchivo(bloqueLibre);


		listaPokemon=NULL;
		listaPokemon= string_new();
		//listAux=NULL;
		//listAux=string_new();
		string_append(&listaPokemon,elem);
		tamanioPokemon=string_length(listaPokemon);


	}

}



void crearMetadataArchPoke(char* pokemon){

	char* rutaMetadataPoke=string_new();

		string_append(&rutaMetadataPoke,rutas_fs->pathDirectorioBloques);
		string_append(&rutaMetadataPoke,"/");
		string_append(&rutaMetadataPoke,pokemon);
		string_append(&rutaMetadataPoke,"/Metadata.bin");
		FILE* metadataPoke = fopen(rutaMetadataPoke, "wb");

		char* lineaDirectorio=string_new();
		string_append(&lineaDirectorio,"DIRECTORY=N\n");
		fwrite(lineaDirectorio, string_length(lineaDirectorio), 1,metadataPoke);

		char* lineaSize=string_new();
		string_append(&lineaSize,"SIZE=");
		string_append(&lineaSize, (string_itoa(espacioPokemon(listaPokemon))));
		string_append(&lineaSize,"\n");
		fwrite(lineaSize, string_length(lineaSize), 1,metadataPoke);

		char* lineaBloquesOcupados=string_new();
		string_append(&lineaBloquesOcupados,"BLOCKS=[");
		for (int i=0, j=0; i<=list_size(bloquesMetadataPokemon) & j<=list_size(bloquesMetadataPokemon)-1 ; i++, j++){

			string_append(&lineaBloquesOcupados,list_get(bloquesMetadataPokemon,i));
			string_append(&lineaBloquesOcupados,",");

		}

		list_clean(bloquesMetadataPokemon);
		string_append(&lineaBloquesOcupados,"]\n");
		fwrite(lineaBloquesOcupados, string_length(lineaBloquesOcupados), 1,metadataPoke);

		char* lineaOpen=string_new();
		string_append(&lineaOpen,"OPEN=N");
		fwrite(lineaOpen, string_length(lineaOpen), 1,metadataPoke);

		fclose(metadataPoke);

		log_info(gameCard_logger,"Se ha creado el archivo metadata.bin del Pokemon");
}





void copiarEnArchivo(int fd,char* dato,int tamanioDato){

	write(fd,dato,tamanioDato);

}




void abrirBitmap() {

	log_info(gameCard_logger,"Cargando el bitmap");

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

	log_info(gameCard_logger,"Bitmap abierto");
}


int obtenerPrimerBloqueLibre(){

	log_info(gameCard_logger,"Entrando a primer bloque libre");

	for(int i=1; i<=bitarray_get_max_bit(bitarray);i++){;

		if (bitarray_test_bit(bitarray,i)==0){

			return i;

			}

	}

	return -1;
}

void crearPokemon(t_new_pokemon* poke){


	t_list* pokemonACargar=cargarPokemon(poke);

	int tamanioPokemon=0;
	listaPokemon=string_new();
	bloquesMetadataPokemon=list_create();
	listAux=string_new();

	list_iterate(pokemonACargar,medirTamanioLineaPokemon);

	crearMetadataArchPoke(poke->pokemon);

	/*if (noHayEspacioParaPokemon(listaPokemon)){
			log_info(gameCard_logger,"No hay espacio para un nuevo Pokemon");
			list_clean_and_destroy_elements(listaPokemon,0);
			exit(-1);
		}*/
}

