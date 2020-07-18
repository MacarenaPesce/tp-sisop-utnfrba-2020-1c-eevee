#include "formatoFs.h"

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

int status= mkdir(rutas_fs->pathDirectorioMetadataFs, 0777);

	if (status == 0) {
		log_info(gameCard_logger, "Se ha creado el directorio Matadata");
	}

	FILE * archivoMetadata = fopen(rutas_fs->pathArchivoMetadataFs, "wb");

	char* tamBloq=string_itoa(metadata_fs->tamanioBLoques);
	char* lineaMetadataSize=string_new();
	string_append(&lineaMetadataSize,"BLOCK_SIZE=");
	string_append(&lineaMetadataSize,tamBloq);
	string_append(&lineaMetadataSize,"\n");
	free(tamBloq);

	fwrite(lineaMetadataSize, string_length(lineaMetadataSize),1, archivoMetadata);

	char* cantBloq=string_itoa(metadata_fs->cantidadBloques);
	char* lineaMetadataBlock=string_new();
	string_append(&lineaMetadataBlock,"BLOCKS=");
	string_append(&lineaMetadataBlock,cantBloq);
	string_append(&lineaMetadataBlock,"\n");

	free(cantBloq);
	fwrite(lineaMetadataBlock,string_length(lineaMetadataBlock), 1, archivoMetadata);


	char* lineaMetadataMagicNumber=string_new();
	string_append(&lineaMetadataMagicNumber,"MAGIC_NUMBER=");
	string_append(&lineaMetadataMagicNumber,metadata_fs->magicNumber);
	string_append(&lineaMetadataMagicNumber,"\0");

	fwrite(lineaMetadataMagicNumber, string_length(lineaMetadataMagicNumber), 1,archivoMetadata);

	fclose(archivoMetadata);

	log_info(gameCard_logger, " Se ha creado el archivo metadata.bin de FileSystem");

	free(lineaMetadataSize);
	free(lineaMetadataBlock);
	free(lineaMetadataMagicNumber);
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

void cargarMetadataFs() {

	log_info(gameCard_logger, "Cargando Metadata del FileSystem");

	metadata_fs = malloc(sizeof(t_metadata_fs));

	metadata_fs->cantidadBloques =cantidadBloques;
	metadata_fs->tamanioBLoques = tamanioDeBloque;
	metadata_fs->magicNumber = magicNumber;

	log_info(gameCard_logger,"Se ha cargado correctamente la metadata del FileSystem");

}


void crearBitmap() {


	log_info(gameCard_logger,"Iniciando creación del bitmap");

	FILE* bitmapArch = fopen(rutas_fs->pathArchivoBitMap,"wb");


		int blocksChar = metadata_fs->cantidadBloques/8;

		if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}

		//char* bitmapDato = string_new();

		char* bitmapDato=string_repeat('0',blocksChar);


		write(fileno(bitmapArch),bitmapDato,sizeof(char)*string_length(bitmapDato));

	 bitarray=bitarray_create_with_mode(bitmapDato, blocksChar, LSB_FIRST);

	 free(bitmapDato);

		log_info(gameCard_logger," Se ha creado el archivo bitmap.bin");

}

void crearDirectoriosParaFs() {

	int fueCreado = mkdir(rutas_fs->pathDirectorioFilesMetadata, 0777);
	if (fueCreado == 0) {
		log_info(gameCard_logger,"Se ha creado el directorio /Files");}

	int creado = mkdir(rutas_fs->pathDirectorioBloques, 0777);
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

		free(lineaDirectory);
		free(rutaMetaFiles);

		log_info(gameCard_logger, "Se ha creado el archivo metadata.bin del directorio Files");

}


bool existeDirectorio(char* ruta) {

	int estaAbierto=abrir_ruta(ruta);
	free(ruta);
	return  estaAbierto>-1;
}


void InicializarBloquesDeDatosFs() {

	int numBloque=1;

	for (numBloque = 1; numBloque<= metadata_fs->cantidadBloques; numBloque++) {

		char* pathBloque = string_new();
		char* nombreBloque= string_new();

		char* bloq=string_itoa(numBloque);

		string_append(&pathBloque, rutas_fs->pathDirectorioBloques);
		string_append(&pathBloque,"/");
		string_append(&nombreBloque,bloq);
		string_append(&pathBloque,nombreBloque);
		string_append(&pathBloque, ".bin");

		FILE * nuevoBloque = fopen(pathBloque, "wb");
		fclose(nuevoBloque);

		free(bloq);
		free(pathBloque);
		free(nombreBloque);

	}

}

void abrirBitmap() {

	log_info(gameCard_logger,"abriendo el bitmap...");

	int bitmap = open(rutas_fs->pathArchivoBitMap, O_RDWR);

	if (bitmap==-1){
		log_error(gameCard_logger, "Error al hacer open");
	}


	if (fstat(bitmap, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat\n");
		close(bitmap);
	}


	bmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
			bitmap, 0);


	if (bmap == MAP_FAILED) {
		log_error(gameCard_logger, "Fallo el mmap");
	}

	int bytesAEscribirAux = metadata_fs->cantidadBloques / 8;

	if (metadata_fs->cantidadBloques % 8 != 0){ bytesAEscribirAux++;}

	bitarray = bitarray_create_with_mode(bmap,bytesAEscribirAux,LSB_FIRST);

	//msync(bitarray, sizeof(bitarray), MS_SYNC);

	log_info(gameCard_logger,"se ha abierto correctamente el bitmap");

}

void desconectarFs(){

	if(metadata_fs!=NULL){
		log_info(gameCard_logger,"se va a liberar estructuras de la metadata...");
		free(metadata_fs->magicNumber);
		free(metadata_fs);
		log_info(gameCard_logger,"se ha liberado correctamente");
	}


	if(rutas_fs!=NULL){
		log_info(gameCard_logger,"se va a liberar estructuras de las rutas del fs...");
		free(rutas_fs->pathArchivoBitMap);
		free(rutas_fs->pathArchivoMetadataFs);
		free(rutas_fs->pathDirectorioBloques);
		free(rutas_fs->pathDirectorioFilesMetadata);
		free(rutas_fs->pathDirectorioMetadataFs);
		free(rutas_fs->puntoDeMontaje);
		free(rutas_fs);
		log_info(gameCard_logger,"se va a liberaro correctamente");
	}

	if(bitarray!=NULL){
		log_info(gameCard_logger,"se va a liberar memoria que se utilizo para el bitarray...");
		bitarray_destroy(bitarray);
		log_info(gameCard_logger,"se ha liberado correctamente");
	}

	if(bmap!=NULL){
		log_info(gameCard_logger,"se va a liberar memoria que se utilizo para el bitmap...");
	munmap(bmap, mystat.st_size);
	log_info(gameCard_logger,"se ha liberado correctamente");}

	/*
if(pokemonEnMemoria!=NULL){
		log_info(gameCard_logger,"se va a liberar pokemon de memoria...");

		free(pokemonEnMemoria);
	log_info(gameCard_logger,"se ha liberado correctamente");*/

}

