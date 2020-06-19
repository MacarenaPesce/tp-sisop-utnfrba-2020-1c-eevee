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
	string_append(&lineaMetadataMagicNumber,metadata_fs->magicNumber);
	string_append(&lineaMetadataMagicNumber,"\0");


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
	//string_append(&tamanioBloque,"/0");

	string_append(&cantidadBloques,
			(string_itoa(config_get_int_value(configMetataNueva, "BLOCKS"))));
	//string_append(&tamanioBloque,"/0");


	string_append(&magicNumber,
			config_get_string_value(configMetataNueva, "MAGIC_NUMBER"));
	//string_append(&tamanioBloque,"/0");

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

	//FILE* bitmapArch = fopen(rutas_fs->pathArchivoBitMap,"wb");

	int bitmapArch=open(rutas_fs->pathArchivoBitMap,O_RDWR |O_CREAT);

		int blocksChar = metadata_fs->cantidadBloques/8;

		log_info(gameCard_logger,"tamanio para copiar en 0: %d",blocksChar);

		if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}

		log_info(gameCard_logger,"agrega 1 segun el numero: %d",blocksChar);

		char* bitmapData = string_new();

		bitmapData=string_repeat('0',blocksChar);


		log_info(gameCard_logger,"mostrame el bit char: %s", bitmapData);

		log_info(gameCard_logger,"mostrame la longitud de opiado: %d",(string_length(bitmapData)) );


		write(bitmapArch,bitmapData,sizeof(char)*string_length(bitmapData));

	 bitarray =	bitarray_create_with_mode(bitmapData, blocksChar, LSB_FIRST);


				for(int i=0; i<=bitarray_get_max_bit(bitarray);i++){
				//Seteo el bitmap en 0 para la posicion requerida
				bitarray_clean_bit(bitarray,i);
				}


		log_info(gameCard_logger,"aca tengo el bitarray: %s", bitarray->bitarray);
		//free(bitmapData);

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

void agregarAstring(void* elem){

	char* var=elem;
	string_append(&stringAux,var);
}

int espacioPokemon(t_list* list){

stringAux=string_new();
   list_iterate(list,agregarAstring);

	return string_length(stringAux);
}



bool noHayEspacioParaPokemon(t_list* listaPokemon){

	int espacioDisponible=(metadata_fs->tamanioBLoques)*cantBloquesLibres();

	log_info(gameCard_logger,"aca queremos ver espcio Disponible: %d", espacioDisponible);
	log_info(gameCard_logger,"aca queremos ver el espacio A ocupar %d", espacioPokemon(listaPokemon));

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

	log_info(gameCard_logger, "aca entro para preparar arch .bin");

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

		log_info(gameCard_logger,"aca cierro archivo copiado");
}

void marcarBloqueOcupado(int bloqueLibre){

	log_info(gameCard_logger," bloque libre va a pasar a ocupado");

		bitarray_set_bit(bitarray,bloqueLibre);

	log_info(gameCard_logger,"efectivamente paso a : %d",bitarray_test_bit(bitarray,bloqueLibre));

	int blocksChar = (metadata_fs->tamanioBLoques/8);

	if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}

	log_info(gameCard_logger," cant de bloques: %d",blocksChar);

	log_info(gameCard_logger,"cant de bitarray: %s", bitarray->bitarray);

	bitmap = open(rutas_fs->pathArchivoBitMap,blocksChar);

	write(bitmap, bitarray ->bitarray ,sizeof(char)* blocksChar);

	msync(bitarray, sizeof(bitarray), MS_SYNC);

}

void agregarBloqueParaMetadataArchivo(int bloqueLibre){

	list_add(bloquesMetadataPokemon,string_itoa(bloqueLibre));
}


void copiarPersistiendoPokemon(){

	int bloqueLibre=obtenerPrimerBloqueLibre();

		log_info(gameCard_logger,"el primer bloque libre es: %d",bloqueLibre);

		copiarEnBloqueLibre(bloqueLibre,listaPokemon);

		log_info(gameCard_logger, "se ha copiado %s , correctamente en el bloque %d",listaPokemon,bloqueLibre);

		marcarBloqueOcupado(bloqueLibre);

		agregarBloqueParaMetadataArchivo(bloqueLibre);
}


void medirTamanioLineaPokemon(void* pokemon){

	cantElemPokemon=cantElemPokemon-1;
	char* elem=pokemon;
	tamanioPokemon=tamanioPokemon+string_length(elem);

	if (tamanioPokemon<=metadata_fs->tamanioBLoques ){
		string_append(&listaPokemon,elem);
	}
	else{

		copiarPersistiendoPokemon();

		listaPokemon=NULL;
		listaPokemon= string_new();
		string_append(&listaPokemon,elem);
		tamanioPokemon=string_length(listaPokemon);
	}


	//ultimo elemento de la lista
	if(cantElemPokemon==0){
		copiarPersistiendoPokemon();
		}

}



void crearMetadataArchPoke(char* pokemon,t_list* listPok){

	log_info(gameCard_logger,"Creando el directorio Pokemon : %s", pokemon);

	char* directorioPoke=string_new();

		string_append(&directorioPoke,rutas_fs->pathDirectorioFilesMetadata);
		string_append(&directorioPoke,"/");
		string_append(&directorioPoke,pokemon);

		int fueCreado = mkdir(directorioPoke, 0777); //analizar permisos
		if (fueCreado == 0) {
			log_info(gameCard_logger,"Se ha creado el directorio / %s",pokemon);}

	char* rutaMetadataPoke=string_new();

	log_info(gameCard_logger,"mostrame ruta: %s",directorioPoke);

		string_append(&rutaMetadataPoke,directorioPoke);
		string_append(&rutaMetadataPoke,"/Metadata.bin");

		FILE* metadataPoke = fopen(rutaMetadataPoke, "wb");


		log_info(gameCard_logger,"Creando el archivo metadata del pokemon");

		char* lineaDirectorio=string_new();
		string_append(&lineaDirectorio,"DIRECTORY=N");
		string_append(&lineaDirectorio,	"\n");
		fwrite(lineaDirectorio, string_length(lineaDirectorio), 1,metadataPoke);

		log_info(gameCard_logger,"se ha copiado correctamente directorio");

		char* lineaSize=string_new();
		string_append(&lineaSize,"SIZE=");
		string_append(&lineaSize, (string_itoa(espacioPokemon(listPok))));
		string_append(&lineaSize,"\n");
		fwrite(lineaSize, string_length(lineaSize), 1,metadataPoke);

		char* lineaBloquesOcupados=string_new();
		string_append(&lineaBloquesOcupados,"BLOCKS=[");
		for (int i=0, j=0; i<=list_size(bloquesMetadataPokemon) & j<=(list_size(bloquesMetadataPokemon)-1) ; i++, j++){

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


/*	int bytesBitmap = metadata_fs->cantidadBloques / 8;

	if (metadata_fs->cantidadBloques % 8 != 0){ bytesBitmap++;}

		FILE* f = fopen(rutas_fs->pathArchivoBitMap,"rb+");

		//Si f no es null entonces el archivo ya existe
		if(f == NULL){
			log_info(gameCard_logger,"Error al querer obtenr el archivo bitmap");
			return;
		}
		//Creo el array de bitmap
		char* bitmapData = malloc(bytesBitmap*sizeof(char));
		//Leo los bitmaps
		fread(bitmapData, sizeof(char), bytesBitmap, f);
		t_bitarray* bitarray =	bitarray_create_with_mode(bitmapData, bytesBitmap, LSB_FIRST);

		log_info(gameCard_logger, "aca hay que ver medida bitarray: %s", bitarray_get_max_bit(bitarray));
		for(int i=0; i<=bitarray_get_max_bit(bitarray);i++){
		//Seteo el bitmap en 0 para la posicion requerida
		bitarray_clean_bit(bitarray,i);
		}
		//Vuelvo a guardar
		//rewind(f);
		//fwrite(bitarray->bitarray,sizeof(char),bytesBitmap,f);
		fclose(f);
		//free(bitmapData);
	//	bitarray_destroy(bitarray);*/

	log_info(gameCard_logger,"ruta bitmap es :%s", rutas_fs->pathArchivoBitMap);

	int bitmap = open(rutas_fs->pathArchivoBitMap, O_RDWR);

	if (bitmap==-1){
		log_error(gameCard_logger, "Error al hacer open");
	}

	struct stat mystat;

	if (fstat(bitmap, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat\n");
		close(bitmap);
	}

	char *bmap;
	bmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
			bitmap, 0);

//	log_info(logger,"inicialicé mmap");

	if (bmap == MAP_FAILED) {
		log_error(gameCard_logger, "Fallo el mmap");
	}

	int bytesAEscribirAux = metadata_fs->tamanioBLoques / 8;

	if (metadata_fs->tamanioBLoques % 8 != 0)
		bytesAEscribirAux++;
	bitarray = bitarray_create_with_mode(bmap, bytesAEscribirAux,
			MSB_FIRST);



	//log_info(gameCard_logger,"Cargando el bitmap");

	//bitmap = open(rutas_fs->pathArchivoBitMap,"rb+");

	//FILE* f = fopen(rutas_fs->pathArchivoBitMap,"rb+");

	//struct stat estadoArch;

	//stat(rutas_fs->pathArchivoBitMap,&estadoArch);

	//tamBmap=estadoArch.st_size;

//	log_info(gameCard_logger,"aca valido tamanio archivo bitmap: %d",tamBmap);

	//bmap =(char*) mmap(NULL, tamBmap, PROT_WRITE | PROT_READ, MAP_SHARED,bitmap, 0);


	//int bytesBitmap = metadata_fs->cantidadBloques / 8;

	//if (metadata_fs->cantidadBloques % 8 != 0){ bytesBitmap++;}

	//char* bitmapData = malloc(bytesBitmap*sizeof(char));

	//log_info(gameCard_logger,"aca quiero ver bitmapData %s",bitmapData);


	//bitarray =	bitarray_create_with_mode(bitmapData, bytesBitmap, LSB_FIRST);



//	log_info(gameCard_logger,"valido el tamanio en bytes: %d",bytesBitmap);

	//for (int i=1; i<=bitarray_get_max_bit(bitarray);i++){

	//bitarray_clean_bit(bitarray,i);
	//rewind(f);
	//fwrite(bitarray->bitarray,sizeof(char),bytesBitmap,f);
    //fclose(f);
	//}

	/*


	//bitarray = bitarray_create_with_mode(bmap, bytesBitmap,MSB_FIRST);


	log_info(gameCard_logger,"aca valido si hay maximo en bitarray %d",bitarray_get_max_bit(bitarray));

	log_info(gameCard_logger,"aca valido contenido del bitarray %d",bitarray->bitarray);

	for (int i=1; i<=bitarray_get_max_bit(bitarray);i++){

		log_info(gameCard_logger,"validando si entro en la pos %d", i);

		log_info(gameCard_logger,"validando lo que hay en bitarray: %d", bitarray_test_bit(bitarray,i));}

	msync(bmap, sizeof(bitarray), MS_SYNC);

	log_info(gameCard_logger,"Bitmap abierto");
	*/
}


int obtenerPrimerBloqueLibre(){

	log_info(gameCard_logger,"Entrando a primer bloque libre");

	for(int i=1; i<=bitarray_get_max_bit(bitarray);i++){;

		if (bitarray_test_bit(bitarray,i)==0){

			return i;

			}

	}

	msync(bmap, sizeof(bitarray), MS_SYNC);

	return -1;
}

void crearPokemon(t_new_pokemon* poke){

	t_list* pokemonACargar=cargarPokemon(poke);

	log_info(gameCard_logger,"iniciando creacion de un pokemon");


	if (noHayEspacioParaPokemon(pokemonACargar)){
		     log_info(gameCard_logger,"No hay espacio para un nuevo Pokemon");
		     exit(-1);
			}

	int tamanioPokemon=0;
	listaPokemon=string_new();
	bloquesMetadataPokemon=list_create();
	listAux=string_new();
	cantElemPokemon=list_size(pokemonACargar);
	list_iterate(pokemonACargar,medirTamanioLineaPokemon);
	log_info(gameCard_logger," comienza a crear la metadata del pokemon");
	crearMetadataArchPoke(poke->pokemon,pokemonACargar);

}

//valido antes de existia o no pokemon aca asumo que existe pokemon y hay que modificarlo
void modificarPokemon(t_new_pokemon* poke){

	/*levantar el archivo completo a memoria, calcular lo que tengan que calcular y
	después bajarlo a los archivos de bloques (agregando o eliminando si hace falta)
	cada vez que agrego/elimino/actualizo alguna posición reescribo todos los bloques
	(archivos .bin de la carpeta Blocks) del Pokemon afectado. Siempre reutilizando
	los mismos bloques que el Pokemon tiene asignados (para evitar liberar
	y pedir nuevos bloques), y agregando en caso de que necesite mas bloques, o
	quitando en caso de que necesite menos bloques.
	De esta manera, me aseguro que los bloques no excedan nunca del tamaño máximo,
	 y también me aseguro de no que no exista fragmentación interna en los bloques,
	 quedando espacios sin utilizar.
	  El tema del flag de Open, si, eso les puede dar un flag claro, el tema como bien comentaron
	  es la posible condición de carrera al tocar ese mismo archivo de metadata (que no es lo mismo
	  que operar con todos los bloques)
	  Ahí esta bien la idea de agregar un semáforo del metadata para que 2 hilos
	  no hagan cambios sobre el metadata al mismo tiempo,
	  una vez cambiado el metadata no deberían tener mas problemas ya que
	  2 hilos no van a ir a modificar el mismo set de bloques
	  *
	  * */
}
