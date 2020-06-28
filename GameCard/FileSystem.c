#include <stdlib.h>
#include <stdio.h>
#include "FileSystem.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include "formatoFs.h"


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

void copiarEnBloque(char* bloqueLibre,char* lineaAcopiar){

	log_info(gameCard_logger, "accediendo al bloque %s.bin", bloqueLibre);

	log_info(gameCard_logger, "mostrame que vas a copiar: %s", lineaAcopiar);

	    char* rutaBloqueLibre=string_new();

		string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloqueLibre,"/");
		string_append(&rutaBloqueLibre,bloqueLibre);
		string_append(&rutaBloqueLibre, ".bin");
		FILE *bloque = fopen(rutaBloqueLibre, "wb");

		log_info(gameCard_logger,"copiando pokemon en el bloque %s.bin",bloqueLibre);
		fseek(bloque,0,SEEK_SET);
		fwrite(lineaAcopiar,string_length(lineaAcopiar),1,bloque);
		//fclose(bloque);

		contenidoBloque=string_new();

		log_info(gameCard_logger,"se ha copiado correctamente");
}

void marcarBloqueOcupado(int bloqueLibre){

	log_info(gameCard_logger," bloque libre va a pasar a ocupado en bitmap");

	log_info(gameCard_logger, "el bloque a cambiar es %d",bloqueLibre);

		bitarray_set_bit(bitarray,bloqueLibre);

	log_info(gameCard_logger,"efectivamente paso a : %d ",bitarray_test_bit(bitarray,bloqueLibre));

	msync(bitarray, sizeof(bitarray), MS_SYNC);

}

void agregarBloqueParaMetadataArchivo(char* bloqueLibre){

	list_add(bloquesMetadataPokemon,bloqueLibre);
}


void copiarPersistiendoPokemon(){

	int bloqueLibre=obtenerPrimerBloqueLibre();

		log_info(gameCard_logger,"el primer bloque libre es: %d",bloqueLibre);

		copiarEnBloque(bloqueLibre,listaPokemon);

		log_info(gameCard_logger, "se ha copiado  correctamente en el bloque %d",bloqueLibre);

		marcarBloqueOcupado(bloqueLibre);

		agregarBloqueParaMetadataArchivo(bloqueLibre);
}



void crearMetadataArchPoke(char* pokemon, int tamanio){

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
		rewind(metadataPoke);
		fwrite(lineaDirectorio, string_length(lineaDirectorio), 1,metadataPoke);

		char* lineaSize=string_new();
		string_append(&lineaSize,"SIZE=");
		string_append(&lineaSize, (string_itoa(tamanio)));
		string_append(&lineaSize,"\n");
		fwrite(lineaSize, string_length(lineaSize), 1,metadataPoke);

		char* lineaBloquesOcupados=string_new();
		int tamanioMaxList=list_size(bloquesMetadataPokemon);
		string_append(&lineaBloquesOcupados,"BLOCKS=[");



		for (int i=0; i< tamanioMaxList; i++){

			log_info(gameCard_logger, "aca entra en for");
			log_info(gameCard_logger,"pos %d y elem : %s", i,list_get(bloquesMetadataPokemon,i));

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



int obtenerPrimerBloqueLibre(){

	for(int i=1; i<=bitarray_get_max_bit(bitarray);i++){;

		if (bitarray_test_bit(bitarray,i)==0){

			return i;

			}

	}

	return -1;
}

void crearPokemon(t_new_pokemon* poke){

	pokemonEnMemoria=string_new();
	posAcopiar=string_new();

string_append(&posAcopiar,cargarPokemon(poke));

	//inicializacion de lista de bloques del metadata
	bloquesMetadataPokemon=list_create();

	log_info(gameCard_logger,"iniciando creacion de un pokemon");

	if (noHayEspacioParaPokemon(posAcopiar)){
		     log_error(gameCard_logger,"No hay espacio para un nuevo Pokemon");
		     exit(-1);
			}


	int cantBloquesNecesarios=cantBloquesNecesariosPara(posAcopiar);

	log_info(gameCard_logger,"cantidad de bloques que necesita: %d",cantBloquesNecesarios);

	bloquesNuevos= list_create();

	bloquesNuevos= obtenerBloquesNuevos(cantBloquesNecesarios);

	list_iterate(bloquesNuevos,persistirCambiosEnBloquesPropios);

	crearMetadataArchPoke(poke->pokemon,string_length(posAcopiar));

}

bool entraEnBloque(char* lineaPokemon){

	return metadata_fs->tamanioBLoques>=string_length(lineaPokemon);
}


void copiarPokemonEnMemoria(char* unBloque){


	log_info(gameCard_logger,"aca se va a abrir el bloque del poke");

	char* rutaBloque;
	rutaBloque=string_new();

	log_info(gameCard_logger,"aca me llega el bloque: %s",unBloque);

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque,"/");
	string_append(&rutaBloque,unBloque);
	string_append(&rutaBloque,".bin");

	log_info(gameCard_logger,"aca quiero ver ruta: %s",rutaBloque);

	int fdBloq=open(rutaBloque,O_RDWR);

	if (fdBloq<=-1){log_error(gameCard_logger,"Error al abrir el archivo");}

	struct stat mystat;

			if (fstat(fdBloq, &mystat) < 0) {
				log_error(gameCard_logger, "Error en el fstat\n");
				close(fdBloq);}


			log_info(gameCard_logger,"aca llega : %s",rutaBloque);

	//char* contenido= string_new();

	pokemonEnMemoria= mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
			fdBloq, 0);

	if(pokemonEnMemoria==NULL){

		log_error(gameCard_logger,"hay error aca memo");}

	/*if(contenido==NULL){

	log_error(gameCard_logger,"hay error aca");}

if(pokemonEnMemoria==NULL){

	log_error(gameCard_logger,"hay error aca memo");}

pokemonEnMemoria=string_new();

	//string_append(&pokemonEnMemoria,contenido);

string_append(pokemonEnMemoria,contenido);

	log_info(gameCard_logger,"mostrame memo : %s", pokemonEnMemoria);

	if(!string_ends_with(pokemonEnMemoria,"\n")){

		string_append(&pokemonEnMemoria,"\n");
	}*/
}


void  llenarListaBloquesPoke(char* poke){

	log_info(gameCard_logger, "accediendo a la metadata del pokemon: %s", poke);

		char* rutaMetadataPokemon=string_new();

		string_append(&rutaMetadataPokemon, rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaMetadataPokemon,"/");
		string_append(&rutaMetadataPokemon,poke);
		string_append(&rutaMetadataPokemon,"/Metadata.bin");


	log_info(gameCard_logger,"aca valido ruta: %s",rutaMetadataPokemon);
			//el archivo metadata.bin se parece al archivo de configuracion
			//se reutilizan estructuras
			t_config* metadataPokemon = config_create(rutaMetadataPokemon);


			int size=config_get_int_value(metadataPokemon,"SIZE");

			int i = size/metadata_fs->tamanioBLoques;

			if(size%metadata_fs->tamanioBLoques!=0){ i++;}

			char** listaBloques=config_get_array_value(metadataPokemon,"BLOCKS");

			for(int j=1;j<=i;j++){

				log_info(gameCard_logger,"aca mostrame que tiene listBlqoues %s", listaBloques[i-1]);
				list_add(bloquesMetadataPokemon,listaBloques[i-1]);
			}

			//string_iterate_lines(listaBloques,agregarAlista);

			config_destroy(metadataPokemon);

}

void agregarAlista(char* bloque){

	log_info(gameCard_logger,"bloque a lista de bloques metadata: %s",bloque);
	list_add(bloquesMetadataPokemon,bloque);
}

void inicializarEstructurasParaModificarPokemon(t_new_pokemon* pokemonAeditar){

	log_info(gameCard_logger, "aca se va a modificar el pokemon");

	bloquesMetadataPokemon=list_create();

	//me traigo la linea que pasa new_pokemon
		nuevaPos=string_new();

		int posx=pokemonAeditar->coordenadas.posx;
		int posy=pokemonAeditar->coordenadas.posy;
		string_append(&nuevaPos,string_itoa(posx));
		string_append(&nuevaPos,"-");
		string_append(&nuevaPos,string_itoa(posy));
		string_append(&nuevaPos,"=");

		log_info(gameCard_logger,"aca se cargo linea pokemon: %s",nuevaPos);
		nuevaCant=pokemonAeditar->cantidad;
		tamanioNuevaPos=string_length(nuevaPos);
		pokemonEnMemoria=string_new();

		pokemonAguardar=string_new();
		posAcopiar=string_new();
		cantBloquesOcupadosPorPoke=0;

		cantBlok=0;

		cantElem=0;

		copiarSinSaltoUltimo=string_new();

		pokemon=string_new();
	 string_append(&pokemon,pokemonAeditar->pokemon);

	 desde=0;
	 copiado=0;
	 hasta=0;

}

void pokeAmodificarInicializate(t_new_pokemon* pokemonAeditar){

	nuevaPos=string_new();

		int posx=pokemonAeditar->coordenadas.posx;
		int posy=pokemonAeditar->coordenadas.posy;
		string_append(&nuevaPos,string_itoa(posx));
		string_append(&nuevaPos,"-");
		string_append(&nuevaPos,string_itoa(posy));
		string_append(&nuevaPos,"=");

		log_info(gameCard_logger,"aca se cargo linea pokemon: %s",nuevaPos);
		nuevaCant=pokemonAeditar->cantidad;

		pokemon=string_new();
		string_append(&pokemon,pokemonAeditar->pokemon);
}

void modificarPokemon(t_new_pokemon* pokemonAeditar){

	pokemonEnMemoria=string_new();

	bloquesMetadataPokemon= list_create();

	log_info(gameCard_logger,"aca se va a modificar el pokemon");

	pokeAmodificarInicializate(pokemonAeditar);

	log_info(gameCard_logger,"mostrame nueva pos %s",nuevaPos);

	llenarListaBloquesPoke(pokemon);

	log_info(gameCard_logger,"metadta size: %d",list_size(bloquesMetadataPokemon));

	copiarPokemonAmemoria(bloquesMetadataPokemon);

	log_info (gameCard_logger,"ACA poke en memoria %s",pokemonEnMemoria);

	log_info (gameCard_logger,"ACA nueva Pos!!! %s",nuevaPos);


	if( estaPosicionEnMemoria(pokemonEnMemoria,nuevaPos)){

		log_info(gameCard_logger,"pos esta en momoria, hay que modificarla ");
		log_info(gameCard_logger,"aca hay que modificar la pos: %s",nuevaPos);
		log_info(gameCard_logger,"aca quiero ver memroria: %s",pokemonEnMemoria);
		modificarPosicion(nuevaPos,nuevaCant,pokemonEnMemoria);
	}

	else{

		log_info (gameCard_logger,"ACA ENTRASTE PARA CREAR UNA NUEVA LINEA");

		int posUltimoBloque=list_size(bloquesMetadataPokemon)-1;

		log_info (gameCard_logger,"ACA MOSTRAME ULT POS LISTA DE BLOKS: %d",posUltimoBloque);

		char* ultBloque=list_get(bloquesMetadataPokemon,posUltimoBloque);

		log_info (gameCard_logger,"ACA MOSTRAME ult bloque: %s",ultBloque);

		char* bloqEnMemo;

		traerAmemoriaUltimoBloque(ultBloque,bloqEnMemo);

		log_info (gameCard_logger,"ACA ya lesite arch: %s",ultBloque);

		string_append(&nuevaPos,string_itoa(nuevaCant));

		string_append(&nuevaPos,"\n");

		log_info (gameCard_logger,"ACA vas a ver nueva Pos: %s",nuevaPos);

		if (metadata_fs->tamanioBLoques-string_length(bloqEnMemo)<=string_length(nuevaPos)){


			log_info (gameCard_logger,"ACA vas a ver nueva Pos: %s",nuevaPos);

			log_info(gameCard_logger,"logueate aca");

			//string_append(&bloqEnMemo,nuevaPos);

			log_info(gameCard_logger,"pasate en appende OOOOKKKK");

			log_info (gameCard_logger,"ACA vas a ver la memo: %s",bloqEnMemo);


			int result= msync(bloqEnMemo, string_length(bloqEnMemo), MS_SYNC);

			if (result==-1){

				log_error(gameCard_logger,"yes, aca falla sincronozacion");
			}


			//msync(bloqueEnMemo, );
			//copiarEnUltimoBloque(ultBloque, bloqEnMemo);
		}




	}

}


void copiarEnUltimoBloque(ultBloque, bloqEnMemo){



		}





void traerAmemoriaUltimoBloque(char* ultBloque,char* bloqueEnMemo){

	log_info(gameCard_logger,"aca se va a abrir el bloque del poke");

		char* rutaBloque;
		rutaBloque=string_new();

		log_info(gameCard_logger,"aca me llega el bloque: %s",ultBloque);

		string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloque,"/");
		string_append(&rutaBloque,ultBloque);
		string_append(&rutaBloque,".bin");

		log_info(gameCard_logger,"aca quiero ver ruta: %s",rutaBloque);

		int fdBloq=open(rutaBloque,O_RDWR);

		if (fdBloq<=-1){log_error(gameCard_logger,"Error al abrir el archivo");}

		struct stat mystat;

				if (fstat(fdBloq, &mystat) < 0) {
					log_error(gameCard_logger, "Error en el fstat\n");
					close(fdBloq);}


				log_info(gameCard_logger,"aca llega : %s",rutaBloque);

		//char* contenido= string_new();

		bloqueEnMemo= mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
				fdBloq, 0);

		if(pokemonEnMemoria==NULL){

			log_error(gameCard_logger,"hay error aca memo");}


}


void modificarPosicion(char* nuevaPos,int cantidad,char* pokemonEnMemoria){


	log_info (gameCard_logger,"ACA MODIFICAME.....");
	copiado=desde=0;

	int tamanioMetadata ;

	posAcopiar=string_new();

	char** posiciones=string_split(pokemonEnMemoria,"\n");

	string_iterate_lines(posiciones,agregarCantidadNuevaAposicion);

	log_info(gameCard_logger,"aca pos modificada%s",posAcopiar);
	//log_info (gameCard_logger,"mostrame que tenes en poke memo: %s",pokemonEnMemoria);


	int cantBloqNecesarios=cantBloquesNecesariosPara(posAcopiar);

	log_info(gameCard_logger,"aca mostrame cuanto ocupa posModificada:%d", cantBloqNecesarios);


	log_info(gameCard_logger,"aca mostrame memoria:%s",pokemonEnMemoria);

	int cantBloqOcupados=cantBloquesNecesariosPara(pokemonEnMemoria);

	log_info(gameCard_logger,"poke en memoria ocupa:%d",cantBloqOcupados);

	int bloqNuevos=cantBloqNecesarios-cantBloqOcupados;

	//llenarListaBloquesPoke(pokemon);

	if(bloqNuevos==0){

		log_info(gameCard_logger,"mostrame size de lista metadata pokemon; %d", list_size(bloquesMetadataPokemon));

		log_info(gameCard_logger,"Copiar en el mismo bloque");

		log_info(gameCard_logger,"mostrame JJJJJposacopiar: %s", posAcopiar);

		for(int i=0; i<list_size(bloquesMetadataPokemon);i++){

			char* elem=list_get(bloquesMetadataPokemon,i);
			if(elem!=NULL){
			log_info(gameCard_logger,"aca accedo a lista de bloques, pos: %d", i);
			persistirCambiosEnBloquesPropios(list_get(bloquesMetadataPokemon,i));
		}
		}

	tamanioMetadata=string_length(posAcopiar);

	}/*

	else {

		int tamanioMetadata =string_length(posAcopiar);

		char* posAux1=string_new();

		char* posAux2=string_new();

		string_append(&posAux1,string_substring(posAcopiar,0,metadata_fs->tamanioBLoques*cantBloqOcupados));


		string_append(&posAux2,string_substring(posAcopiar,metadata_fs->tamanioBLoques*cantBloqOcupados,string_length(posAcopiar)));

		log_info(gameCard_logger,"Copiar en el mismo bloque");

		//posAcopiar=string_new();

		string_append(&posAcopiar,posAux1);

		list_iterate(bloquesMetadataPokemon, persistirCambiosEnBloquesPropios);

		posAcopiar=string_new();

		string_append(&posAcopiar,posAux1);

			log_info(gameCard_logger,"cantidad de bloques que necesita: %d",bloqNuevos);

			bloquesNuevos= list_create();

			bloquesNuevos= obtenerBloquesNuevos(bloqNuevos);

			list_iterate(bloquesNuevos,persistirCambiosEnBloquesNuevos);

	}*/

	crearMetadataArchPoke(pokemon,tamanioMetadata);

}

void agregarNuevaPosicion(nuevaPos,cantidad,pokemonEnMemoria){


}

bool estaPosicionEnMemoria(char* pokemonEnMemoria, char* nuevaPos){

	return (string_contains(pokemonEnMemoria,nuevaPos)==1);

}

void contardorDeBloquesOcupadosPorPokemon(char** bloksOcupadosPorPokemon){
	string_iterate_lines(bloksOcupadosPorPokemon,sumameBloques);
}

void copiarPokemonAmemoria(t_list* listBloqPokemon){

	list_iterate(listBloqPokemon,copiarPokemonEnMemoria);
}

void sumameBloques(char* bloq){

	cantBlok=cantBlok +1;

	log_info(gameCard_logger,"mostrame que tenes en cantBloc: %d",cantBlok);
}


void copiarEnBloques(void* bloque){

	log_info (gameCard_logger,"aca quiero operar con un bloque nuevo");
	char* bloqueNuevo=string_new();

	string_append(&bloqueNuevo,bloque);

	log_info(gameCard_logger,"el bloque nuevo es : %s", bloqueNuevo);

	persistirCambiosEnBloquesNuevos(bloqueNuevo);

}
void cambiarTamanioMetadata(char* pokemon,int tamanioAgregar){

	t_config* configPoke;

	char* rutaPoke=string_new();
	string_append(&rutaPoke,rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke,"/");
	string_append(&rutaPoke,pokemon);
	string_append(&rutaPoke,"/Metadata.bin");

	configPoke=config_create(rutaPoke);

	log_info(gameCard_logger,"aca quiero chequear ruta %s",rutaPoke);

	int cantidadNueva=(config_get_int_value(configPoke,"SIZE"))+tamanioAgregar;

	log_info(gameCard_logger,"aca quiero validar cantidad: %d", cantidadNueva);

	config_set_value(configPoke,"SIZE",string_itoa(cantidadNueva));

	config_save_in_file(configPoke,rutaPoke);

}




void obtenerCantBloques(char* bloque){

	cantBloquesPoke=cantBloquesPoke+1;
}


void persistirCambiosEnBloquesNuevos(char* bloqueNuevo){


		persistirCambiosEnBloquesPropios(bloqueNuevo);

		//agregarBloqueParaMetadataArchivo(bloqueNuevo);

}

t_list* obtenerBloquesNuevos(int cantBloqNecesarios){

	log_info(gameCard_logger,"ACA BLOQUE LIBRES , CANT: %d",cantBloqNecesarios);
	log_info(gameCard_logger, "aca tengo que obtener bloques nuevos");
	t_list* bloquesLibres;
	bloquesLibres=list_create();

	while(cantBloqNecesarios!=0){

		int bloqLib=obtenerPrimerBloqueLibre();

		list_add(bloquesLibres,string_itoa(bloqLib));

		log_info(gameCard_logger,"aca la cant de nec: %d",cantBloqNecesarios);

		log_info (gameCard_logger,"aca dame el primer blque libre: %d",bloqLib);

		marcarBloqueOcupado(bloqLib);

		agregarBloqueParaMetadataArchivo(string_itoa(bloqLib));

	cantBloqNecesarios=cantBloqNecesarios-1;

	}

	log_info(gameCard_logger,"CANT LISTA BLOQUES  : %d", list_size(bloquesLibres));
	return bloquesLibres;
}

void	persistirCambiosEnBloquesPropios(void* bloque){


	int aCopiar=string_length(posAcopiar);

	//log_info(gameCard_logger,"aca mostrame aCopiar: %s ", aCopiar);
	if((aCopiar-copiado)<metadata_fs->tamanioBLoques){

		log_info(gameCard_logger,"mostrame que va a copiar: %s", posAcopiar);
		log_info(gameCard_logger,"aca mostrame desde: %d y aCopiar-desde : %d ", desde,aCopiar-desde );


		log_info(gameCard_logger,"mostrame que vas a copiar",string_substring(posAcopiar,desde,aCopiar-desde));
		copiarEnBloque(bloque,string_substring(posAcopiar,desde,aCopiar-desde));
	}

	else{

		copiarEnBloque(bloque,string_substring(posAcopiar,desde,metadata_fs->tamanioBLoques));

		desde=desde+metadata_fs->tamanioBLoques;

	}

}



void agregarCantidadNuevaAposicion(char* posicion){

	log_info(gameCard_logger,"mostrame posicion : %s", posicion);

	if(string_starts_with(posicion,nuevaPos)==1){

		tamanioNuevaPos=string_length(nuevaPos);
		log_info(gameCard_logger,"aca para cambiar cantidad");

		log_info(gameCard_logger,"cantidad: %d",nuevaCant);

		log_info(gameCard_logger,"aca mostrame lo que vas a sumar: %d",atoi(string_substring_from(posicion,tamanioNuevaPos)));

		nuevaCant=nuevaCant+atoi(string_substring_from(posicion,tamanioNuevaPos));

		log_info(gameCard_logger,"aca mostrame cantidad: %d",nuevaCant);

		string_append(&posAcopiar,nuevaPos);
		string_append(&posAcopiar,string_itoa(nuevaCant));
		string_append(&posAcopiar,"\n");

	}
	else{

		string_append(&posAcopiar,posicion);
		string_append(&posAcopiar,"\n");
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


void cantBloquesOcupadosPorPokemon(char* bloque){

	 cantBloquesOcupadosPorPoke=cantBloquesOcupadosPorPoke+1;
}


bool noHayEspacioParaModificaciones(char* posValidas){

	int espacioDisponible=(cantBloquesOcupadosPorPoke+
			(cantBloquesLibres()))*metadata_fs->tamanioBLoques;

	 return string_length(posValidas)>espacioDisponible;


	 }


void eliminarLineaPokemon(t_new_pokemon* poke){


}

char** obtenerbloquesMetadata(char* pokemon){

	t_config* configPoke;

		char* rutaPoke=string_new();
		string_append(&rutaPoke,rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaPoke,"/");
		string_append(&rutaPoke,pokemon);
		string_append(&rutaPoke,"/Metadata.bin");

		configPoke=config_create(rutaPoke);

		char** bloques = config_get_array_value(configPoke,"BLOCKS");

		return bloques;

}

int cantBloquesNecesariosPara(char* pokeCompleto){

	int cantBloques=string_length(pokeCompleto)/metadata_fs->tamanioBLoques;

	if(string_length(pokeCompleto)%metadata_fs->tamanioBLoques!=0){cantBloques++;}

	return cantBloques;
}

void quitarSaltoDeLinea(char* texto){

	string_iterate_lines(string_split(texto,"\n"), cantidadElementos);
	string_iterate_lines(string_split(texto,"\n"),copiarSinUltimoSalto);
}

void cantidadElementos(char* text){

	cantElem=cantElem+1;
}

void copiarSinUltimoSalto(char* text){

	if((cantElem-1)>0){
	string_append(&copiarSinSaltoUltimo,text);
	string_append(&copiarSinSaltoUltimo,"\n");
}else{

	string_append(&copiarSinSaltoUltimo,text);

}
}

void modificarBloquesMetadata(char* poke, char** bloquesPokemon){

	t_config* configPoke;

		char* rutaPoke=string_new();
		string_append(&rutaPoke,rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaPoke,"/");
		string_append(&rutaPoke,pokemon);
		string_append(&rutaPoke,"/Metadata.bin");

		configPoke=config_create(rutaPoke);

		log_info(gameCard_logger,"aca quiero chequear ruta %s",rutaPoke);


		config_set_value(configPoke,"BLOCKS",bloquesPokemon);

		config_save_in_file(configPoke,rutaPoke);


	}




