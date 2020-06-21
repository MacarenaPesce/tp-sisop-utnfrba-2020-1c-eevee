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

int status= mkdir(rutas_fs->pathDirectorioMetadataFs, 0777);

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

	FILE* bitmapArch = fopen(rutas_fs->pathArchivoBitMap,"wb");

	fdBitmap=fileno(bitmapArch);


		int blocksChar = metadata_fs->cantidadBloques/8;

		if (metadata_fs->cantidadBloques % 8 != 0) { blocksChar++;}


		char* bitmapData = string_new();

		bitmapData=string_repeat('0',blocksChar);


		write(fileno(bitmapArch),bitmapData,sizeof(char)*string_length(bitmapData));

	 bitarray =	bitarray_create_with_mode(bitmapData, blocksChar, LSB_FIRST);

		log_info(gameCard_logger," Se ha creado el archivo bitmap.bin");

		fclose(bitmapArch);

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
	string_append(&ruta_pokemon, "/");
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

int espacioPokemon(char* lineaPoke){


	return string_length(lineaPoke);
}



bool noHayEspacioParaPokemon(char* listaPokemon){

	int espacioDisponible=(metadata_fs->tamanioBLoques)*cantBloquesLibres();

	return espacioDisponible<espacioPokemon(listaPokemon);


}


char*  cargarPokemon(t_new_pokemon* pokemon){

	char* lineaPokemon=string_new();

	char* posx= string_itoa(pokemon->coordenadas.posx);
			char* posy=string_itoa(pokemon->coordenadas.posy);
			char* guion="-";
			char* igual="=";
			char* cantidad=string_itoa(pokemon->cantidad);
			char* salto="\n";

	string_append(&lineaPokemon,posx);
	string_append(&lineaPokemon,guion);
	string_append(&lineaPokemon,posy);
	string_append(&lineaPokemon,igual);
	string_append(&lineaPokemon,cantidad);
	string_append(&lineaPokemon,salto);

		return lineaPokemon;

}

void copiarEnBloqueLibre(int bloqueLibre,char* lineaAcopiar){

	log_info(gameCard_logger, "accediendo al bloque %d.bin", bloqueLibre);

	    char* rutaBloqueLibre=string_new();
		char* nombreBloque=string_new();

		string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloqueLibre,"/");
		string_append(&nombreBloque,(string_itoa(bloqueLibre)));
		string_append(&rutaBloqueLibre,nombreBloque);
		string_append(&rutaBloqueLibre, ".bin");
		FILE *bloque = fopen(rutaBloqueLibre, "wb");

		log_info(gameCard_logger,"copiando pokemon en el bloque %d.bin",bloqueLibre);
		fwrite(lineaAcopiar,string_length(lineaAcopiar),1,bloque);
		fclose(bloque);

		log_info(gameCard_logger,"se ha copiado el pokemon correctamente");
}

void marcarBloqueOcupado(int bloqueLibre){

	log_info(gameCard_logger," bloque libre va a pasar a ocupado en bitmap");

	log_info(gameCard_logger, "el bloque a cambiar es %d",bloqueLibre);

		bitarray_set_bit(bitarray,bloqueLibre);

	log_info(gameCard_logger,"efectivamente paso a : %d ",bitarray_test_bit(bitarray,bloqueLibre));

	msync(bitarray, sizeof(bitarray), MS_SYNC);

}

void agregarBloqueParaMetadataArchivo(int bloqueLibre){

	list_add(bloquesMetadataPokemon,string_itoa(bloqueLibre));
}


void copiarPersistiendoPokemon(){

	int bloqueLibre=obtenerPrimerBloqueLibre();

		log_info(gameCard_logger,"el primer bloque libre es: %d",bloqueLibre);

		copiarEnBloqueLibre(bloqueLibre,listaPokemon);

		log_info(gameCard_logger, "se ha copiado  correctamente en el bloque %d",bloqueLibre);

		marcarBloqueOcupado(bloqueLibre);

		agregarBloqueParaMetadataArchivo(bloqueLibre);
}



void crearMetadataArchPoke(char* pokemon, char* lineaPoke){

	log_info(gameCard_logger,"Creando el directorio Pokemon : %s", pokemon);

	char* directorioPoke=string_new();

		string_append(&directorioPoke,rutas_fs->pathDirectorioFilesMetadata);
		string_append(&directorioPoke,"/");
		string_append(&directorioPoke,pokemon);

		int fueCreado = mkdir(directorioPoke, 0777); //analizar permisos
		if (fueCreado == 0) {
			log_info(gameCard_logger,"Se ha creado el directorio / %s",pokemon);}

	char* rutaMetadataPoke=string_new();

		string_append(&rutaMetadataPoke,directorioPoke);
		string_append(&rutaMetadataPoke,"/Metadata.bin");

		FILE* metadataPoke = fopen(rutaMetadataPoke, "wb");


		log_info(gameCard_logger,"Creando el archivo metadata del pokemon");

		char* lineaDirectorio=string_new();
		string_append(&lineaDirectorio,"DIRECTORY=N");
		string_append(&lineaDirectorio,	"\n");
		fwrite(lineaDirectorio, string_length(lineaDirectorio), 1,metadataPoke);

		char* lineaSize=string_new();
		string_append(&lineaSize,"SIZE=");
		string_append(&lineaSize, (string_itoa(espacioPokemon(lineaPoke))));
		string_append(&lineaSize,"\n");
		fwrite(lineaSize, string_length(lineaSize), 1,metadataPoke);

		char* lineaBloquesOcupados=string_new();
		int tamanioMaxList=list_size(bloquesMetadataPokemon);
		string_append(&lineaBloquesOcupados,"BLOCKS=[");
		for (int i=0; i<= (tamanioMaxList-1); i++){

			string_append(&lineaBloquesOcupados,list_get(bloquesMetadataPokemon,i));

			if(i != (tamanioMaxList-1)){

				string_append(&lineaBloquesOcupados,",");

			}

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


	log_info(gameCard_logger,"abriendo el bitmap...");

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


	if (bmap == MAP_FAILED) {
		log_error(gameCard_logger, "Fallo el mmap");
	}

	int bytesAEscribirAux = metadata_fs->tamanioBLoques / 8;

	if (metadata_fs->tamanioBLoques % 8 != 0){ bytesAEscribirAux++;}

	bitarray = bitarray_create_with_mode(bmap, bytesAEscribirAux,LSB_FIRST);

	log_info(gameCard_logger,"se ha abierto correctamente el bitmap");


}


int obtenerPrimerBloqueLibre(){

	for(int i=1; i<=bitarray_get_max_bit(bitarray);i++){;

		if (bitarray_test_bit(bitarray,i)==0){

			return i;

			}

	}

	return -1;
}

void crearPokemon(t_new_pokemon* poke){

	char* lineaPoke=cargarPokemon(poke);

	bloquesMetadataPokemon=list_create();

	log_info(gameCard_logger,"iniciando creacion de un pokemon");


	if (noHayEspacioParaPokemon(lineaPoke)){
		     log_error(gameCard_logger,"No hay espacio para un nuevo Pokemon");
		     exit(-1);
			}

	if(entraEnBloque(lineaPoke)){

		int bloqueLibre=obtenerPrimerBloqueLibre();

		log_info(gameCard_logger,"el primer bloque libre es: %d",bloqueLibre);

		copiarEnBloqueLibre(bloqueLibre,lineaPoke);

		log_info(gameCard_logger, "se ha copiado pokemon correctamente en el bloque %d",bloqueLibre);

		marcarBloqueOcupado(bloqueLibre);

		agregarBloqueParaMetadataArchivo(bloqueLibre);

		crearMetadataArchPoke(poke->pokemon,lineaPoke);
	}

	else {log_error(gameCard_logger, "el bloque no tiene espacio suficiente");
	         exit(-1);}


}

bool entraEnBloque(char* lineaPokemon){

	return metadata_fs->tamanioBLoques>=string_length(lineaPokemon);
}


void copiarPokemonEnMemoria(char* unBloque){

	char* rutaBloque;

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque,"/");
	string_append(&rutaBloque,unBloque);
	string_append(&rutaBloque,".bin");

	int fdBloq=open(rutaBloque,O_RDWR);

	struct stat mystat;

			if (fstat(fdBloq, &mystat) < 0) {
				log_error(gameCard_logger, "Error en el fstat\n");
				close(fdBloq);}


	char* contenidoBloque= mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
			fdBloq, 0);


	string_append(&pokemonEnMemoria,contenidoBloque);
}


char** getBloquesPoke(char* poke){

	log_info(gameCard_logger, "accediendo a la metadata del pokemon: %s", poke);

		char* rutaMetadataPokemon=string_new();

		string_append(&rutaMetadataPokemon, rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaMetadataPokemon,"/");
		string_append(&rutaMetadataPokemon,poke);
		string_append(&rutaMetadataPokemon,".bin");


			//el archivo metadata.bin se parece al archivo de configuracion
			//se reutilizan estructuras
			t_config* metadataPokemon = config_create(rutaMetadataPokemon);


			char** listaBloques=config_get_array_value(metadataPokemon,"BLOCKS");

			config_destroy(metadataPokemon);

			return listaBloques;

}
//valido antes de existia o no pokemon aca asumo que existe pokemon y hay que modificarlo
void modificarPokemon(t_new_pokemon* pokemonAeditar){

	nuevaPos=string_new();

	int posx=pokemonAeditar->coordenadas.posx;
	int posy=pokemonAeditar->coordenadas.posy;
	string_append(&nuevaPos,string_itoa(posx));
	string_append(&nuevaPos,"-");
	string_append(&nuevaPos,string_itoa(posy));
	string_append(&nuevaPos,"=");

	nuevaCant=pokemonAeditar->cantidad;
	tamanioNuevaPos=string_length(nuevaPos);

	pokemonEnMemoria=string_new();
	//obtengo los bloques que ocupa el pokemon
	char** bloquesDelPokemon=getBloquesPoke(pokemonAeditar->pokemon);

	string_iterate_lines(bloquesDelPokemon,copiarPokemonEnMemoria);

	pokemonAguardar=string_new();

	posValidas=string_new();

	cantBloquesOcupadosPorPoke=0;

	if(string_contains(pokemonEnMemoria,nuevaPos)==1){

		char** posiciones=string_split(pokemonEnMemoria,"\n");

		string_iterate_lines(posiciones,operarPosiciones);

		string_iterate_lines(bloquesDelPokemon,cantBloquesOcupadosPorPoke);


	}


	//aca copiar la estructura obtenida de operar posiciones copiar en los bloque que ya tengo
	//si no me entra voy a buscar un bloque libre y termino copiando alli

}


void agregarPokemon(char** bloquesDelPokemon,char* posValidas){


}

void operarPosiciones(char* posicion){

	if(string_starts_with(posicion,nuevaPos)==1){

		nuevaCant=nuevaCant+atoi(string_substring_from(posicion,tamanioNuevaPos));

		string_append(&posValidas,nuevaPos);
		string_append(&posValidas,string_itoa(nuevaCant));
		string_append(&posValidas,"\n");

	}
	else{

		string_append(&posValidas,posicion);
		string_append(&posValidas,"\n");
	}
}

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


void cantBloquesOcupadosPorPoke(char* bloque){

	 cantBloquesOcupadosPorPoke=cantBloquesOcupadosPorPoke+1;
}


void pensarNombre(char* posValidas){

	 if (string_length(posValidas)> cantBloquesOcupadosPorPoke*metadata_fs->tamanioBLoques){

	 }

}
