#include <stdlib.h>
#include <stdio.h>
#include "FileSystem.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

bool noCumpleConRutasfs() {

	// se intenta hacer open de cada ruta propia del fs
	//si open da negativo no existe ruta

	return abrir_ruta(rutas_fs->pathDirectorioMetadataFs) < 0
			| abrir_ruta(rutas_fs->pathArchivoMetadataFs) < 0
			| abrir_ruta(rutas_fs->pathArchivoBitMap) < 0;
}

void crearFileSystemVacio() {

	//debería tomar del archivo configuracion lo que necesito para crear fs
	//NO TENGO OTRO LADO DE DONDE SACAr ESA INFORMACION
	//tamanio de bloque, cantidad total de bloques y el string "TALL_GRASS"

	crearMetadataFs();
	crearBitmap();
	crearDirectoriosParaFs();
	mostrarContenidoFs();

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
	FILE * archivoMetadata = fopen(rutas_fs->pathArchivoMetadataFs, "wb");
	fwrite(&metadata_fs->magicNumber, sizeof(metadata_fs->magicNumber), 1,
			archivoMetadata);
	fwrite(&metadata_fs->tamanioBLoques, sizeof(metadata_fs->tamanioBLoques), 1,
			archivoMetadata);
	fwrite(&metadata_fs->cantidadBloques, sizeof(metadata_fs->cantidadBloques),
			1, archivoMetadata);
	fclose(archivoMetadata);
	log_info(gameCard_logger, " Se ha creado el archivo metadata.bin");
}

void cargarRutasFs() {

	log_info(gameCard_logger, "carganado las rutas correspondientes al fs");
	//logger(escribir_loguear,l_info,"Hi, bienvenido a GameCard!");
	//logger(escribir_loguear,l_info,"Por ahora solo valida si existe fs o no");
	//se tiene referncia por fuera para liberar memoria después
	rutas_fs = malloc(sizeof(t_rutas_fs));
	rutas_fs->puntoDeMontaje = string_new();
	rutas_fs->pathDirectorioMetadataFs = string_new();
	rutas_fs->pathArchivoMetadataFs = string_new();
	rutas_fs->pathArchivoBitMap = string_new();
	rutas_fs->pathDirectorioFilesMetadata = string_new();
	rutas_fs->pathDirectorioBloques = string_new();

	//logger(escribir_loguear,l_info,"validando contenido de las rutas propias del fs...");
	//tomar path del punto de montaje (es un atributo extern)
	string_append(&(rutas_fs->puntoDeMontaje), punto_montaje_fs);
	//logger(escribir_loguear,l_info,"El punto de montaje es: %s", rutas_fs->puntoDeMontaje);

	//armar ruta directorio Metadata
	string_append(&(rutas_fs->pathDirectorioMetadataFs),
			rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioMetadataFs), "/Metadata");
	//logger(escribir_loguear,l_info,"Ruta Metadata del fs es: %s", rutas_fs->pathDirectorioMetadataFs);

	//armar ruta archivo metadata.bin
	string_append(&(rutas_fs->pathArchivoMetadataFs),
			rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoMetadataFs), "/Metadata.bin");
	//logger(escribir_loguear,l_info,"Ruta metadata. bin es : %s", rutas_fs->pathArchivoMetadataFs);

	//armar ruta archivo bitmap.bin
	string_append(&(rutas_fs->pathArchivoBitMap),
			rutas_fs->pathDirectorioMetadataFs);
	string_append(&(rutas_fs->pathArchivoBitMap), "/Bitmap.bin");
	//logger(escribir_loguear,l_info,"Ruta metadata.bin es: %s", rutas_fs->pathArchivoBitMap);

	//armar ruta Files Metadata
	string_append(&(rutas_fs->pathDirectorioFilesMetadata),
			rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioFilesMetadata), "/Files");
	//logger(escribir_loguear,l_info,"ruta files es: %s", rutas_fs->pathDirectorioFilesMetadata);

	//armar ruta de bloques
	string_append(&(rutas_fs->pathDirectorioBloques), rutas_fs->puntoDeMontaje);
	string_append(&(rutas_fs->pathDirectorioBloques), "/Blocks");
	//logger(escribir_loguear,l_info,"ruta bloques es : %s", rutas_fs->pathDirectorioBloques);

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

	log_info(gameCard_logger,
			"se ha cargado correctamente la metadata del FileSystem");

	//free(configMetataNueva);
	config_destroy(configMetataNueva);
	free(tamanioBloque);
	free(cantidadBloques);
	free(magicNumber);
	//config_destroy(configMetataNueva);

}

void liberarMemoriaFs() {

	//free(metadata_fs->magicNumber);
	free(metadata_fs);
	free(gameCard_logger);
	//free(config_game_card);
	free(rutas_fs);
}

void crearBitmap() {

	FILE  *archBitmap = fopen(rutas_fs->pathArchivoBitMap,"wb");

		char* a = string_repeat(0,metadata_fs->cantidadBloques);
		t_bitarray* bitarray = bitarray_create_with_mode(a,(metadata_fs->cantidadBloques/8),LSB_FIRST);
		for(int i=1;i< bitarray_get_max_bit(bitarray)  ;i++){
			bitarray_set_bit(bitarray,i);
		}

		fwrite(&bitarray,sizeof(t_bitarray),1,archBitmap);

/*

	// se van a crear el archivo bitmap.bin
	// w= crea un fichero en modo binario para escribir
	//FILE * archivoMetadata = fopen(rutas_fs->pathArchivoBitMap,"wb");

	//abrir el archivo y crearlo si no existe
	int archBitmap = open(rutas_fs->pathArchivoBitMap, O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR);
	uint32_t cantBloques = metadata_fs->cantidadBloques;

	//quitar parte fraccionaria con ftruncate para evitar errores
	ftruncate(archBitmap, cantBloques);

	/*   ftruncate () hacen que el archivo normal nombrado por ruta o  al que hace referencia
	 fd se trunca a un tamaño de bytes de longitud precisa */
	//int bloquesDisponibles = 0;


		/* char* bitarraycontent = mmap(NULL, (cantBloques / 8),
			PROT_READ | PROT_WRITE, MAP_SHARED, archBitmap, 0);

	bitarray = bitarray_create_with_mode(bitarraycontent, (cantBloques / 8),
			LSB_FIRST);

	log_info(gameCard_logger, "aca bit max es %d",bitarray_get_max_bit(bitarray));

	for (int i = 1; i < bitarray_get_max_bit(bitarray); i++) {

		//	bloquesDisponibles = bloquesDisponibles + 1;
	//		log_info(gameCard_logger,"mostrame tu contenido amigo bitmap: %d",bitarray_test_bit(bitarray,i));

		bitarray_set_bit(bitarray,i);

	}

	msync(bitarraycontent,archBitmap, MS_SYNC);



	//fwrite(&bitarray,sizeof(bitarray),1,archBitmap); */

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



	//aca se van a inicializar los bloques

	//valido el espacio ?

	//int espacioArchivos= metadata_fs->cantidadBloques*metadata_fs->tamanioBLoques;

	InicializarBloquesDeDatosFs();

	log_info(gameCard_logger, "se han creado los directorios /Files y /Blocks");

}


bool existeDirectorio(char* ruta) {

	return abrir_ruta(ruta) > -1;
}



void InicializarBloquesDeDatosFs() {

	int numBloque=1;
	char* nombreBloque= string_new();

	log_info(gameCard_logger,"dame cant bloques: %d",metadata_fs->cantidadBloques);

	for (numBloque = 1; numBloque <= metadata_fs->cantidadBloques; numBloque++) {

		char* pathBloque = string_new();

		//log_info(gameCard_logger,"dame path de /block: %s",rutas_fs->pathDirectorioBloques);
		string_append(&pathBloque, rutas_fs->pathDirectorioBloques);
		//log_info(gameCard_logger,"dame path aux: %s",pathBloque);
		string_append(&pathBloque,"/");
		strcpy(nombreBloque,(string_itoa(numBloque)));
		//log_info(gameCard_logger,"path nombre Bloque: %s",nombreBloque);
		string_append(&pathBloque,nombreBloque);
		//log_info(gameCard_logger, "path bloque con nombre bloque:%s");
		string_append(&pathBloque, ".bin");
		//log_info(gameCard_logger, "dame path con el nombre completo: %s", pathBloque);

		//se va a crear ese archivo si no existe
		FILE * nuevoBloque = fopen(pathBloque, "wb");
		//fclose(nuevoBloque);
    	//free(pathBloque);
	//	free(nombreBloque);

	}

}

/*********************************ACA OPERACIONES************************/

bool existePokemon(char* nombrePokemon) {

	char* ruta_pokemon= string_new();
	string_append(&ruta_pokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&ruta_pokemon,"/Pokemon/");
	string_append(&ruta_pokemon,nombrePokemon);
	log_info(gameCard_logger,"la ruta buscada es : %s", ruta_pokemon);
	return existeDirectorio(ruta_pokemon);

	}

void crearPokemon(t_new_pokemon* pokemon){

	//buscar primer bloque libre y cargar alli hasta la cantidad de bytes
	//indicada por tamaño de bloque
	//si no alcanza buscar otro bloque libre y así hasta que termine de copiarlos datos

	//por cada bloque que ocupa hay que modificar el bitmap y el metadata agregar un nuevo bloque
	//agregar tamanio y si es archivo es y y el open inicilaizarlo

}


int32_t buscarPrimerBloqueLibre() {


}

void mostrarContenidoFs(){

	//para verificar que las cosas se han cargado correctamente

	//mostrar bitmap
	log_info(gameCard_logger,"mostrar bitmap");
	struct stat s;
	int status;

	log_info(gameCard_logger,"mostrame tu ruta: %s", rutas_fs->pathArchivoBitMap);
	int bitmap = open(rutas_fs->pathArchivoBitMap, O_RDWR);

	status = fstat(bitmap, &s);
	int tamanioBitmap= s.st_size;

	bitmapAux= mmap(NULL, tamanioBitmap, PROT_READ | PROT_WRITE, MAP_SHARED, bitmap, 0);

	if(bitmapAux==NULL){
		log_info(gameCard_logger,"Bitmap esta vacio o no se puedo abrir arch");
	}

	log_info(gameCard_logger, "estoy mostrando bitmap: %s",bitmapAux);

	//mostrar metadataFs.bin

/*	t_config* configGame= config_create(rutas_fs->pathArchivoMetadataFs);

	int tamanioBloque= config_get_int_value(configGame,"BLOCK_SIZE");
	int cantBloques= config_get_int_value(configGame,"BLOCKS");
	char* nombreFileSystem= config_get_string_value(configGame,"MAGIC_NUMBER");

	log_info(gameCard_logger,"dame tamanio bloque: %d",tamanioBloque);
	log_info(gameCard_logger,"dame cant bloques: %d", cantBloques);
	log_info(gameCard_logger,"dame nombre fs: %s", nombreFileSystem);
	*/
}
