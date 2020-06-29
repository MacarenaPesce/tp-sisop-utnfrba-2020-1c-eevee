#include <stdlib.h>
#include <stdio.h>
#include "FileSystem.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include "formatoFs.h"

bool existePokemon(char* nombrePokemon) {

	char* ruta_pokemon = string_new();
	string_append(&ruta_pokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&ruta_pokemon, "/");
	string_append(&ruta_pokemon, nombrePokemon);
	return existeDirectorio(ruta_pokemon);

}

bool cumpleTamanio(int espacioBloque, int espacioAocupar) {

	return espacioAocupar <= espacioBloque;
}

int cantBloquesLibres() {

	int libres = 0;

	for (int j = 1; j <= bitarray_get_max_bit(bitarray); j++) {

		if (bitarray_test_bit(bitarray, j) == 0) {

			libres = libres + 1;
		}
	}

	return libres;
}

int espacioPokemon(char* lineaPoke) {

	return string_length(lineaPoke);
}

int espacioLibreEnfs() {

	return metadata_fs->tamanioBLoques * cantBloquesLibres();

}

bool noHayEspacioParaPokemon(char* listaPokemon) {

	int espacioDisponible = espacioLibreEnfs();

	return espacioDisponible < espacioPokemon(listaPokemon);

}

char* cargarPokemon(t_new_pokemon* pokemon) {

	char* lineaPokemon = string_new();

	char* posx = string_itoa(pokemon->coordenadas.posx);
	char* posy = string_itoa(pokemon->coordenadas.posy);
	char* guion = "-";
	char* igual = "=";
	char* cantidad = string_itoa(pokemon->cantidad);
	char* salto = "\n";

	string_append(&lineaPokemon, posx);
	string_append(&lineaPokemon, guion);
	string_append(&lineaPokemon, posy);
	string_append(&lineaPokemon, igual);
	string_append(&lineaPokemon, cantidad);
	string_append(&lineaPokemon, salto);

	return lineaPokemon;

}

void copiarEnBloque(char* bloqueLibre, char* lineaAcopiar) {

	log_info(gameCard_logger, "accediendo al bloque %s.bin", bloqueLibre);

	log_info(gameCard_logger, "mostrame que vas a copiar: %s", lineaAcopiar);

	char* rutaBloqueLibre = string_new();

	string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloqueLibre, "/");
	string_append(&rutaBloqueLibre, bloqueLibre);
	string_append(&rutaBloqueLibre, ".bin");
	FILE *bloque = fopen(rutaBloqueLibre, "wb");

	log_info(gameCard_logger, "copiando pokemon en el bloque %s.bin",
			bloqueLibre);
	fseek(bloque, 0, SEEK_SET);
	fwrite(lineaAcopiar, string_length(lineaAcopiar), 1, bloque);
	//fclose(bloque);

	contenidoBloque = string_new();

	log_info(gameCard_logger, "se ha copiado correctamente");
}

void marcarBloqueOcupado(int bloqueLibre) {

	log_info(gameCard_logger, " bloque libre va a pasar a ocupado en bitmap");

	log_info(gameCard_logger, "el bloque a cambiar es %d", bloqueLibre);

	bitarray_set_bit(bitarray, bloqueLibre);

	log_info(gameCard_logger, "efectivamente paso a : %d ",
			bitarray_test_bit(bitarray, bloqueLibre));

	msync(bitarray, sizeof(bitarray), MS_SYNC);

}

void agregarBloqueParaMetadataArchivo(char* bloqueLibre) {

	list_add(bloquesMetadataPokemon, bloqueLibre);
}

void copiarPersistiendoPokemon() {

	int bloqueLibre = obtenerPrimerBloqueLibre();

	log_info(gameCard_logger, "el primer bloque libre es: %d", bloqueLibre);

	copiarEnBloque(string_itoa(bloqueLibre), listaPokemon);

	log_info(gameCard_logger, "se ha copiado  correctamente en el bloque %d",
			bloqueLibre);

	marcarBloqueOcupado(bloqueLibre);

	agregarBloqueParaMetadataArchivo(string_itoa(bloqueLibre));
}

void crearMetadataArchPoke(char* pokemon, int tamanio) {

	log_info(gameCard_logger, "Creando el directorio Pokemon : %s", pokemon);

	char* directorioPoke = string_new();

	string_append(&directorioPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&directorioPoke, "/");
	string_append(&directorioPoke, pokemon);

	int fueCreado = mkdir(directorioPoke, 0777); //analizar permisos
	if (fueCreado == 0) {
		log_info(gameCard_logger, "Se ha creado el directorio / %s", pokemon);
	}

	char* rutaMetadataPoke = string_new();

	string_append(&rutaMetadataPoke, directorioPoke);
	string_append(&rutaMetadataPoke, "/Metadata.bin");

	FILE* metadataPoke = fopen(rutaMetadataPoke, "wb");

	log_info(gameCard_logger, "Creando el archivo metadata del pokemon");

	char* lineaDirectorio = string_new();
	string_append(&lineaDirectorio, "DIRECTORY=N");
	string_append(&lineaDirectorio, "\n");
	rewind(metadataPoke);
	fwrite(lineaDirectorio, string_length(lineaDirectorio), 1, metadataPoke);

	char* lineaSize = string_new();
	string_append(&lineaSize, "SIZE=");
	string_append(&lineaSize, (string_itoa(tamanio)));
	string_append(&lineaSize, "\n");
	fwrite(lineaSize, string_length(lineaSize), 1, metadataPoke);

	char* lineaBloquesOcupados = string_new();
	int tamanioMaxList = list_size(bloquesMetadataPokemon);
	string_append(&lineaBloquesOcupados, "BLOCKS=[");

	for (int i = 0; i < tamanioMaxList; i++) {

		log_info(gameCard_logger, "aca entra en for");
		log_info(gameCard_logger, "pos %d y elem : %s", i,
				list_get(bloquesMetadataPokemon, i));

		string_append(&lineaBloquesOcupados,
				list_get(bloquesMetadataPokemon, i));

		if (i != (tamanioMaxList - 1)) {

			string_append(&lineaBloquesOcupados, ",");

		}

	}

	list_clean(bloquesMetadataPokemon);
	string_append(&lineaBloquesOcupados, "]\n");
	fwrite(lineaBloquesOcupados, string_length(lineaBloquesOcupados), 1,
			metadataPoke);

	char* lineaOpen = string_new();
	string_append(&lineaOpen, "OPEN=N");
	fwrite(lineaOpen, string_length(lineaOpen), 1, metadataPoke);

	fclose(metadataPoke);

	log_info(gameCard_logger,
			"Se ha creado el archivo metadata.bin del Pokemon");
}

void copiarEnArchivo(int fd, char* dato, int tamanioDato) {

	write(fd, dato, tamanioDato);

}

int obtenerPrimerBloqueLibre() {

	for (int i = 1; i <= bitarray_get_max_bit(bitarray); i++) {
		;

		if (bitarray_test_bit(bitarray, i) == 0) {

			return i;

		}

	}

	return -1;
}

void crearPokemon(t_new_pokemon* poke) {

	pokemonEnMemoria = string_new();
	posAcopiar = string_new();

	string_append(&posAcopiar, cargarPokemon(poke));

	//inicializacion de lista de bloques del metadata
	bloquesMetadataPokemon = list_create();

	log_info(gameCard_logger, "iniciando creacion de un pokemon");

	if (noHayEspacioParaPokemon(posAcopiar)) {
		log_error(gameCard_logger, "No hay espacio para un nuevo Pokemon");
		exit(-1);
	}

	int cantBloquesNecesarios = cantBloquesNecesariosPara(posAcopiar);

	log_info(gameCard_logger, "cantidad de bloques que necesita: %d",
			cantBloquesNecesarios);

	bloquesNuevos = list_create();

	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);

	list_iterate(bloquesNuevos, persistirCambiosEnBloquesPropios);

	crearMetadataArchPoke(poke->pokemon, string_length(posAcopiar));

}

bool entraEnBloque(char* lineaPokemon) {

	return metadata_fs->tamanioBLoques >= string_length(lineaPokemon);
}

void copiarPokemonEnMemoria(char* unBloque) {

	log_info(gameCard_logger, "aca se va a abrir el bloque del poke");

	char* rutaBloque;
	rutaBloque = string_new();

	log_info(gameCard_logger, "aca me llega el bloque: %s", unBloque);

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, unBloque);
	string_append(&rutaBloque, ".bin");

	log_info(gameCard_logger, "aca quiero ver ruta: %s", rutaBloque);

	int fdBloq = open(rutaBloque, O_RDWR);

	if (fdBloq <= -1) {
		log_error(gameCard_logger, "Error al abrir el archivo");
	}

	struct stat mystat;

	if (fstat(fdBloq, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat\n");
		close(fdBloq);
	}

	log_info(gameCard_logger, "aca llega : %s", rutaBloque);

	//char* contenido= string_new();

	if (pokemonEnMemoria == NULL) {

		pokemonEnMemoria = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
		MAP_SHARED, fdBloq, 0);
	}

	else {

		char* aux = string_new();

		aux = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
				fdBloq, 0);

		string_append(&pokemonEnMemoria, aux);

	}

	log_info(gameCard_logger, "ME MO RIA ACA: %s", pokemonEnMemoria);

	if (pokemonEnMemoria == NULL) {

		log_error(gameCard_logger, "hay error aca memo");
	}
}

void llenarListaBloquesPoke(char* poke) {

	log_info(gameCard_logger, "accediendo a la metadata del pokemon: %s", poke);

	char* rutaMetadataPokemon = string_new();

	string_append(&rutaMetadataPokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetadataPokemon, "/");
	string_append(&rutaMetadataPokemon, poke);
	string_append(&rutaMetadataPokemon, "/Metadata.bin");

	log_info(gameCard_logger, "aca valido ruta: %s", rutaMetadataPokemon);
	//el archivo metadata.bin se parece al archivo de configuracion
	//se reutilizan estructuras
	t_config* metadataPokemon = config_create(rutaMetadataPokemon);

	int tamanioArch = config_get_int_value(metadataPokemon, "SIZE");

	int cantBloq = tamanioArch / metadata_fs->tamanioBLoques;

	if (tamanioArch % metadata_fs->tamanioBLoques != 0) {
		cantBloq++;
	}

	char** listaBloques = config_get_array_value(metadataPokemon, "BLOCKS");

	for (int j = 0; j < cantBloq; j++) {

		log_info(gameCard_logger, "aca mostrame que tiene listBloques %s",
				listaBloques[j]);
		list_add(bloquesMetadataPokemon, listaBloques[j]);
	}

	//string_iterate_lines(listaBloques,agregarAlista);

	config_destroy(metadataPokemon);

}

void agregarAlista(char* bloque) {

	log_info(gameCard_logger, "bloque a lista de bloques metadata: %s", bloque);
	list_add(bloquesMetadataPokemon, bloque);
}

void inicializarEstructurasParaModificarPokemon(t_new_pokemon* pokemonAeditar) {

	log_info(gameCard_logger, "aca se va a modificar el pokemon");

	bloquesMetadataPokemon = list_create();

	//me traigo la linea que pasa new_pokemon
	nuevaPos = string_new();

	int posx = pokemonAeditar->coordenadas.posx;
	int posy = pokemonAeditar->coordenadas.posy;
	string_append(&nuevaPos, string_itoa(posx));
	string_append(&nuevaPos, "-");
	string_append(&nuevaPos, string_itoa(posy));
	string_append(&nuevaPos, "=");

	log_info(gameCard_logger, "aca se cargo linea pokemon: %s", nuevaPos);
	nuevaCant = pokemonAeditar->cantidad;
	tamanioNuevaPos = string_length(nuevaPos);
	pokemonEnMemoria = string_new();

	pokemonAguardar = string_new();
	posAcopiar = string_new();
	cantBloquesOcupadosPorPoke = 0;

	cantBlok = 0;

	cantElem = 0;

	copiarSinSaltoUltimo = string_new();

	pokemon = string_new();
	string_append(&pokemon, pokemonAeditar->pokemon);

	desde = 0;
	copiado = 0;
	hasta = 0;

}

void pokeAmodificarInicializate(t_new_pokemon* pokemonAeditar) {

	nuevaPos = string_new();

	int posx = pokemonAeditar->coordenadas.posx;
	int posy = pokemonAeditar->coordenadas.posy;
	string_append(&nuevaPos, string_itoa(posx));
	string_append(&nuevaPos, "-");
	string_append(&nuevaPos, string_itoa(posy));
	string_append(&nuevaPos, "=");

	log_info(gameCard_logger, "aca se cargo linea pokemon: %s", nuevaPos);
	nuevaCant = pokemonAeditar->cantidad;

	pokemon = string_new();
	string_append(&pokemon, pokemonAeditar->pokemon);
}

void modificarPokemon(t_new_pokemon* pokemonAeditar) {

	pokemonEnMemoria = string_new();

	bloquesMetadataPokemon = list_create();

	pokeAmodificarInicializate(pokemonAeditar);

	log_info(gameCard_logger, "mostrame nueva pos %s", nuevaPos);

	llenarListaBloquesPoke(pokemon);

	log_info(gameCard_logger, "CANT BLOQUES METADATA: %d",
			list_size(bloquesMetadataPokemon));

	copiarPokemonAmemoria(bloquesMetadataPokemon);

	log_info(gameCard_logger, "ACA poke en memoria %s", pokemonEnMemoria);

	log_info(gameCard_logger, "ACA nueva Pos!!! %s", nuevaPos);

	if (estaPosicionEnMemoria(pokemonEnMemoria, nuevaPos)) {

		log_info(gameCard_logger,
				"La posicion existe en el archivo, hay que modificarla");

		modificarPosicion(nuevaPos, nuevaCant, pokemonEnMemoria);
	}

	else {

		log_info(gameCard_logger, "se va a crear una nueva posicion");

		int posUltimoBloque = list_size(bloquesMetadataPokemon) - 1;

		log_info(gameCard_logger, "ACA MOSTRAME ULT POS LISTA DE BLOKS: %d",
				posUltimoBloque);

		char* ultBloque = list_get(bloquesMetadataPokemon, posUltimoBloque);

		log_info(gameCard_logger, "ACA MOSTRAME ult bloque: %s", ultBloque);

		bloqEnMemo = string_new();

		bloqEnMemo = traerAmemoriaUltimoBloque(ultBloque);

		log_info(gameCard_logger, "mostrame que tiene ult blok: %s",
				bloqEnMemo);

		log_info(gameCard_logger, "ACA ya leiste arch: %s", ultBloque);

		string_append(&nuevaPos, string_itoa(nuevaCant));

		string_append(&nuevaPos, "\n");

		log_info(gameCard_logger, "ACA vas a ver nueva Pos: %s", nuevaPos);

		log_info(gameCard_logger, "mostrame bloq en memo: %s", bloqEnMemo);

		int espacioOcupadoBloque = string_length(bloqEnMemo);

		int espacioEnBloque = metadata_fs->tamanioBLoques
				- espacioOcupadoBloque;

		int espacioNuevaLinea = string_length(nuevaPos);

		char* stringAcopiar = string_new();

		log_info(gameCard_logger, "aca pasame ocupado bloq: %d",
				espacioOcupadoBloque);
		log_info(gameCard_logger, "aca pasame espacio libre bloq: %d",
				espacioEnBloque);
		log_info(gameCard_logger, "aca pasame espacio nuevaLinea: %d",
				espacioNuevaLinea);

		string_append(&stringAcopiar, bloqEnMemo);

		string_append(&stringAcopiar, nuevaPos);

		log_info(gameCard_logger, "aca pasame string a copia: %s",
				stringAcopiar);

		if (espacioEnBloque >= espacioNuevaLinea) {

			agregarPosicionEnUltimoBLoque(ultBloque, stringAcopiar,
					espacioNuevaLinea);

		} else {

			if (espacioLibreEnfs() == 0) {

				agregarPosicionSinSaltoDeLinea(espacioEnBloque,
						espacioNuevaLinea, ultBloque);

			} else {

				agregarPosicionPokemonAbloquesNuevos(ultBloque, stringAcopiar,
						espacioOcupadoBloque, espacioEnBloque,
						espacioNuevaLinea);
			}
		}

	}

}

void agregarPosicionSinSaltoDeLinea(int espacioEnBloque, int espacioNuevaLinea,
		char* ultBloque) {

	if (string_ends_with(nuevaPos, "\n")) {

		char* nuevaPosSinSaltoDeLinea = string_substring(nuevaPos, 1,
				string_length(nuevaPos) - 1);

		char* copiarPos = string_new();

		string_append(&copiarPos, bloqEnMemo);

		string_append(&copiarPos, nuevaPosSinSaltoDeLinea);

		espacioNuevaLinea = string_length(nuevaPosSinSaltoDeLinea);

		if (espacioEnBloque == espacioNuevaLinea) {

			agregarPosicionEnUltimoBLoque(ultBloque, copiarPos,
					espacioNuevaLinea);

		}

		else {

			log_error(gameCard_logger,
					"no hay espacio para agregar nueva posicion");
		}
	}
}

void modificarPosicion(char* nuevaPos, int cantidad, char* pokemonEnMemoria) {

	log_info(gameCard_logger, "ACA MODIFICAME.....");
	copiado = desde = 0;

	int tamanioMetadata;

	posAcopiar = string_new();

	char** posiciones = string_split(pokemonEnMemoria, "\n");

	string_iterate_lines(posiciones, agregarCantidadNuevaAposicion);

	log_info(gameCard_logger, "aca pos modificada%s", posAcopiar);
//log_info (gameCard_logger,"mostrame que tenes en poke memo: %s",pokemonEnMemoria);

	int cantBloqNecesarios = cantBloquesNecesariosPara(posAcopiar);

	log_info(gameCard_logger, "aca mostrame cuanto ocupa posModificada:%d",
			cantBloqNecesarios);

	log_info(gameCard_logger, "aca mostrame memoria:%s", pokemonEnMemoria);

	int cantBloqOcupados = cantBloquesNecesariosPara(pokemonEnMemoria);

	log_info(gameCard_logger, "poke en memoria ocupa:%d", cantBloqOcupados);

	int bloqNuevos = cantBloqNecesarios - cantBloqOcupados;

//llenarListaBloquesPoke(pokemon);

	if (bloqNuevos == 0) {

		log_info(gameCard_logger, "mostrame size de lista metadata pokemon; %d",
				list_size(bloquesMetadataPokemon));

		log_info(gameCard_logger, "Copiar en el mismo bloque");

		log_info(gameCard_logger, "mostrame JJJJJposacopiar: %s", posAcopiar);

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			char* elem = list_get(bloquesMetadataPokemon, i);
			if (elem != NULL) {
				log_info(gameCard_logger,
						"aca accedo a lista de bloques, pos: %d", i);
				persistirCambiosEnBloquesPropios(
						list_get(bloquesMetadataPokemon, i));
			}
		}

		tamanioMetadata = string_length(posAcopiar);

		crearMetadataArchPoke(pokemon, tamanioMetadata);

	}

	else {
		bloquesNuevos = list_create();

		bloquesNuevos = obtenerBloquesNuevos(cantBloqNecesarios);

		log_info(gameCard_logger, "mostrame size de BLOC NUEVOS",
				list_size(bloquesNuevos));

		copiado = desde = 0;

		log_info(gameCard_logger, "mostrame que long tenes bloques nuevos: %d",
				list_size(bloquesNuevos));

		//list_iterate(bloquesNuevos,persistirCambiosEnBloquesPropios);

		log_info(gameCard_logger, "mostrame que long tenes bloq metada %d",
				list_size(bloquesMetadataPokemon));

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			int aCopiar = string_length(posAcopiar);

			//log_info(gameCard_logger,"aca mostrame aCopiar: %s ", aCopiar);
			if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

				log_info(gameCard_logger, "mostrame que va a copiar: %s",
						string_substring(posAcopiar, copiado,
								aCopiar - copiado));
				log_info(gameCard_logger,
						"aca mostrame copiado: %d y aCopiar : %d ", copiado,
						aCopiar);

				log_info(gameCard_logger, "mostrame que vas a copiar",
						string_substring(posAcopiar, copiado,
								aCopiar - copiado));

				copiarEnBloque(list_get(bloquesMetadataPokemon, i),
						string_substring(posAcopiar, copiado,
								aCopiar - copiado));
			}

			else {

				copiarEnBloque(list_get(bloquesMetadataPokemon, i),
						string_substring(posAcopiar, copiado,
								metadata_fs->tamanioBLoques));

				copiado = copiado + metadata_fs->tamanioBLoques;

			}

		}

		crearMetadataArchPoke(pokemon, tamanioMetadata);
	}

}

char* traerAmemoriaUltimoBloque(char* ultBloque) {

	log_info(gameCard_logger, "aca se va a abrir el bloque del poke");

	char* rutaBloque;
	rutaBloque = string_new();

	log_info(gameCard_logger, "aca me llega el bloque: %s", ultBloque);

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, ultBloque);
	string_append(&rutaBloque, ".bin");

	log_info(gameCard_logger, "aca quiero ver ruta: %s", rutaBloque);

	int fdBloq = open(rutaBloque, O_RDWR);

	if (fdBloq <= -1) {
		log_error(gameCard_logger, "Error al abrir el archivo");
	}

	struct stat mystat;

	if (fstat(fdBloq, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat\n");
		close(fdBloq);
	}

	log_info(gameCard_logger, "aca llega : %s", rutaBloque);

	char* bloqueEnMemo;

	bloqueEnMemo = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
	MAP_SHARED, fdBloq, 0);

	if (bloqueEnMemo == NULL) {

		log_error(gameCard_logger, "hay error aca memo");
	}

	return bloqueEnMemo;

}

void agregarPosicionEnUltimoBLoque(char* ultBloq, char* stringAcopiar,
		int espacioNuevaLinea) {

	log_info(gameCard_logger, "Se copia en el último bloque : %s", ultBloq);

	if (stringAcopiar == NULL) {

		log_error(gameCard_logger, "Error en los datos a copiar");
	}

	copiarEnBloque(ultBloq, stringAcopiar);

	cambiarTamanioMetadata(pokemon, espacioNuevaLinea);

	log_info(gameCard_logger, "Se agrego correctamente la posicion");

}

void agregarPosicionPokemonAbloquesNuevos(char* ultBloque, char* stringAcopiar,
		int espacioOcupadoBloque, int espacioEnBloque, int espacioNuevaLinea) {

	char* copiarEnUltBloque = string_new();

	string_append(&copiarEnUltBloque, bloqEnMemo);

	if (espacioEnBloque == 0 & !string_ends_with(copiarEnUltBloque, "\n")) {

		string_append(&copiarEnUltBloque, "\n");

		espacioNuevaLinea = espacioNuevaLinea + 1;
	}

	string_append(&copiarEnUltBloque,
			string_substring(stringAcopiar, espacioOcupadoBloque,
					espacioEnBloque));

	copiarEnBloque(ultBloque, copiarEnUltBloque);

	int cantBloquesNecesarios = cantBloquesNecesariosPara(
			string_substring(stringAcopiar, espacioEnBloque,
					espacioNuevaLinea - espacioEnBloque));

	log_info(gameCard_logger, "cantidad de bloques que necesita: %d",
			cantBloquesNecesarios);

	bloquesNuevos = list_create();

	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);

	log_info(gameCard_logger, "mostrame size de BLOC NUEVOS",
			list_size(bloquesNuevos));

	copiado = desde = 0;

	log_info(gameCard_logger, "mostrame que tenes bloques nuevos: %d",
			list_size(bloquesNuevos));

	//list_iterate(bloquesNuevos,persistirCambiosEnBloquesPropios);

	for (int i = 0; i < list_size(bloquesNuevos); i++) {

		copiado = 0;
		int aCopiar = string_length(
				string_substring(nuevaPos, espacioEnBloque,
						espacioNuevaLinea - espacioEnBloque));

		//log_info(gameCard_logger,"aca mostrame aCopiar: %s ", aCopiar);
		if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

			log_info(gameCard_logger, "mostrame que va a copiar: %s",
					string_substring(nuevaPos, espacioEnBloque,
							espacioNuevaLinea - espacioEnBloque));
			log_info(gameCard_logger,
					"aca mostrame desde: %d y aCopiar-desde : %d ", desde,
					aCopiar - desde);

			log_info(gameCard_logger, "mostrame que vas a copiar",
					string_substring(
							string_substring(nuevaPos, espacioEnBloque,
									espacioNuevaLinea - espacioEnBloque), desde,
							aCopiar - desde));
			copiarEnBloque(list_get(bloquesNuevos, i),
					string_substring(
							string_substring(nuevaPos, espacioEnBloque,
									espacioNuevaLinea - espacioEnBloque), desde,
							aCopiar - desde));
		}

		else {

			copiarEnBloque(list_get(bloquesNuevos, i),
					string_substring(posAcopiar, desde,
							metadata_fs->tamanioBLoques));

			desde = desde + metadata_fs->tamanioBLoques;

		}

		int tamanioNuevo = espacioNuevaLinea;

		cambiarTamanioMetadata(pokemon, espacioNuevaLinea);

		modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);

	}
}

bool estaPosicionEnMemoria(char* pokemonEnMemoria, char* nuevaPos) {

	return (string_contains(pokemonEnMemoria, nuevaPos) == 1);

}

void contardorDeBloquesOcupadosPorPokemon(char** bloksOcupadosPorPokemon) {
	string_iterate_lines(bloksOcupadosPorPokemon, sumameBloques);
}

void copiarPokemonAmemoria(t_list* listBloqPokemon) {

	list_iterate(listBloqPokemon, copiarPokemonEnMemoria);
}

void sumameBloques(char* bloq) {

	cantBlok = cantBlok + 1;

	log_info(gameCard_logger, "mostrame que tenes en cantBloc: %d", cantBlok);
}

void copiarEnBloques(void* bloque) {

	log_info(gameCard_logger, "aca quiero operar con un bloque nuevo");
	char* bloqueNuevo = string_new();

	string_append(&bloqueNuevo, bloque);

	log_info(gameCard_logger, "el bloque nuevo es : %s", bloqueNuevo);

	persistirCambiosEnBloquesNuevos(bloqueNuevo);

}
void cambiarTamanioMetadata(char* pokemon, int tamanioAgregar) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	log_info(gameCard_logger, "aca quiero chequear ruta %s", rutaPoke);

	int cantidadNueva = (config_get_int_value(configPoke, "SIZE"))
			+ tamanioAgregar;

	log_info(gameCard_logger, "aca quiero validar cantidad: %d", cantidadNueva);

	config_set_value(configPoke, "SIZE", string_itoa(cantidadNueva));

	config_save_in_file(configPoke, rutaPoke);

}

void obtenerCantBloques(char* bloque) {

	cantBloquesPoke = cantBloquesPoke + 1;
}

void persistirCambiosEnBloquesNuevos(char* bloqueNuevo) {

	persistirCambiosEnBloquesPropios(bloqueNuevo);

//agregarBloqueParaMetadataArchivo(bloqueNuevo);

}

t_list* obtenerBloquesNuevos(int cantBloqNecesarios) {

	log_info(gameCard_logger, "ACA BLOQUE LIBRES , CANT: %d",
			cantBloqNecesarios);
	log_info(gameCard_logger, "aca tengo que obtener bloques nuevos");
	t_list* bloquesLibres;
	bloquesLibres = list_create();

	while (cantBloqNecesarios != 0) {

		int bloqLib = obtenerPrimerBloqueLibre();

		list_add(bloquesLibres, string_itoa(bloqLib));

		log_info(gameCard_logger, "aca la cant de nec: %d", cantBloqNecesarios);

		log_info(gameCard_logger, "aca dame el primer blque libre: %d",
				bloqLib);

		marcarBloqueOcupado(bloqLib);

		agregarBloqueParaMetadataArchivo(string_itoa(bloqLib));

		cantBloqNecesarios = cantBloqNecesarios - 1;

	}

	log_info(gameCard_logger, "CANT LISTA BLOQUES  : %d",
			list_size(bloquesLibres));
	return bloquesLibres;
}

void persistirCambiosEnBloquesPropios(void* bloque) {

	int aCopiar = string_length(posAcopiar);

//log_info(gameCard_logger,"aca mostrame aCopiar: %s ", aCopiar);
	if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

		log_info(gameCard_logger, "mostrame que va a copiar: %s", posAcopiar);
		log_info(gameCard_logger,
				"aca mostrame desde: %d y aCopiar-desde : %d ", desde,
				aCopiar - desde);

		log_info(gameCard_logger, "mostrame que vas a copiar",
				string_substring(posAcopiar, desde, aCopiar - desde));
		copiarEnBloque(bloque,
				string_substring(posAcopiar, desde, aCopiar - desde));
	}

	else {

		copiarEnBloque(bloque,
				string_substring(posAcopiar, desde,
						metadata_fs->tamanioBLoques));

		desde = desde + metadata_fs->tamanioBLoques;

	}

}

void agregarCantidadNuevaAposicion(char* posicion) {

	log_info(gameCard_logger, "mostrame posicion : %s", posicion);

	if (string_starts_with(posicion, nuevaPos) == 1) {

		tamanioNuevaPos = string_length(nuevaPos);
		log_info(gameCard_logger, "aca para cambiar cantidad");

		log_info(gameCard_logger, "cantidad: %d", nuevaCant);

		log_info(gameCard_logger, "aca mostrame lo que vas a sumar: %d",
				atoi(string_substring_from(posicion, tamanioNuevaPos)));

		nuevaCant = nuevaCant
				+ atoi(string_substring_from(posicion, tamanioNuevaPos));

		log_info(gameCard_logger, "aca mostrame cantidad: %d", nuevaCant);

		string_append(&posAcopiar, nuevaPos);
		string_append(&posAcopiar, string_itoa(nuevaCant));
		string_append(&posAcopiar, "\n");

	} else {

		string_append(&posAcopiar, posicion);
		string_append(&posAcopiar, "\n");
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

void cantBloquesOcupadosPorPokemon(char* bloque) {

	cantBloquesOcupadosPorPoke = cantBloquesOcupadosPorPoke + 1;
}

bool noHayEspacioParaModificaciones(char* posValidas) {

	int espacioDisponible = (cantBloquesOcupadosPorPoke + (cantBloquesLibres()))
			* metadata_fs->tamanioBLoques;

	return string_length(posValidas) > espacioDisponible;

}

char** obtenerbloquesMetadata(char* pokemon) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	char** bloques = config_get_array_value(configPoke, "BLOCKS");

	return bloques;

}

int cantBloquesNecesariosPara(char* pokeCompleto) {

	int cantBloques = string_length(pokeCompleto) / metadata_fs->tamanioBLoques;

	if (string_length(pokeCompleto) % metadata_fs->tamanioBLoques != 0) {
		cantBloques++;
	}

	return cantBloques;
}

void quitarSaltoDeLinea(char* texto) {

	string_iterate_lines(string_split(texto, "\n"), cantidadElementos);
	string_iterate_lines(string_split(texto, "\n"), copiarSinUltimoSalto);
}

void cantidadElementos(char* text) {

	cantElem = cantElem + 1;
}

void copiarSinUltimoSalto(char* text) {

	if ((cantElem - 1) > 0) {
		string_append(&copiarSinSaltoUltimo, text);
		string_append(&copiarSinSaltoUltimo, "\n");
	} else {

		string_append(&copiarSinSaltoUltimo, text);

	}
}

void modificarBloquesMetadata(char* poke, t_list* bloquesPokemon) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	log_info(gameCard_logger, "aca quiero chequear ruta %s", rutaPoke);

	char* lineaBloquesOcupados = string_new();
	int tamanioMaxList = list_size(bloquesPokemon);
	string_append(&lineaBloquesOcupados, "[");

	for (int i = 0; i < tamanioMaxList; i++) {

		log_info(gameCard_logger, "aca entra en for");
		log_info(gameCard_logger, "pos %d y elem : %s", i,
				list_get(bloquesMetadataPokemon, i));

		string_append(&lineaBloquesOcupados,
				list_get(bloquesMetadataPokemon, i));

		if (i != (tamanioMaxList - 1)) {

			string_append(&lineaBloquesOcupados, ",");

		}

	}

	list_clean(bloquesMetadataPokemon);
	string_append(&lineaBloquesOcupados, "]");

	config_set_value(configPoke, "BLOCKS", lineaBloquesOcupados);

	config_save_in_file(configPoke, rutaPoke);

}

bool estaAbiertoArchivo(char* pokemon) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	char* estadoArchivo = config_get_string_value(configPoke, "OPEN");

	return string_equals_ignore_case(estadoArchivo, "Y");

}

void abrirArchivo(char* poke) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, poke);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	log_info(gameCard_logger, "aca quiero chequear ruta %s", rutaPoke);

	config_set_value(configPoke, "OPEN", "Y");

	config_save_in_file(configPoke, rutaPoke);

	log_info(gameCard_logger, " se ha abierto el archivo del pokemon: %s",
			poke);
}

void cerrarArchivo(char* poke) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, poke);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	config_set_value(configPoke, "OPEN", "N");

	config_save_in_file(configPoke, rutaPoke);
}

void capturarPokemon(t_appeared_pokemon* pokeAatrapar) {

	pokemonEnMemoria = string_new();

	bloquesMetadataPokemon = list_create();

	pokeAmodificarInicializate(pokeAatrapar);

	log_info(gameCard_logger, "mostrame nueva pos %s", nuevaPos);

	llenarListaBloquesPoke(pokemon);

	log_info(gameCard_logger, "CANT BLOQUES METADATA: %d",
			list_size(bloquesMetadataPokemon));

	copiarPokemonAmemoria(bloquesMetadataPokemon);

	log_info(gameCard_logger, "ACA poke en memoria %s", pokemonEnMemoria);

	log_info(gameCard_logger, "ACA nueva Pos!!! %s", nuevaPos);

	if (estaPosicionEnMemoria(pokemonEnMemoria, nuevaPos)) {

		posAcopiar=string_new();

		int cantBloqOcupados=cantBloquesNecesariosPara(pokemonEnMemoria);

		char** posiciones = string_split(pokemonEnMemoria, "\n");

		string_iterate_lines(posiciones,capturarPokeEnPos );

		int cantBloqSinPosicion=cantBloquesNecesariosPara(posAcopiar);

		if(cantBloqSinPosicion==0){

			marcarBloquesLibres(bloquesMetadataPokemon);
			vaciarBloques(bloquesMetadataPokemon);
			//elimino metadata
			//elimo directorio pokemon


		}


	}

	else {

		log_error(gameCard_logger, "No existe posicion");
	}
}


void marcarBLoquesLibres(t_list* bloquesOcupados){

	for( int i=0; i < list_size(bloquesOcupados);i++){

		liberararBloque(atoi(list_get(bloquesOcupados,i)));

	}

}

void liberarBloque(int numBloque){

	bitarray_clean_bit(bitarray,numBloque);

	msync(bitarray, sizeof(bitarray), MS_SYNC);

}
void capturarPokeEnPos(char* posicion) {

	log_info(gameCard_logger, "mostrame posicion : %s",posicion);

	if ( string_starts_with(posicion, nuevaPos)) {

		tamanioNuevaPos = string_length(nuevaPos);

int cantEnPos=atoi(string_substring_from(posicion, tamanioNuevaPos));

if(cantEnPos>1){

	cantEnPos=cantEnPos-1;

		log_info(gameCard_logger, "aca mostrame cantidad: %d", cantEnPos);

		string_append(&posAcopiar, nuevaPos);
		string_append(&posAcopiar, string_itoa(cantEnPos));
		string_append(&posAcopiar, "\n");
	}
}
}

void vaciarBloques(t_list* bloquesMetadataPokemon){

	for(int i=0 ; i < list_size(bloquesMetadataPokemon); i++){

	char* stringVacio=string_new();

	char* bloq=string_new();

	string_append(&bloq, list_get(bloquesMetadataPokemon,i));

		log_info(gameCard_logger, "bloque accedido es : %s", bloq);

		char* rutaBloque = string_new();

		string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloque, "/");
		string_append(&rutaBloque, bloq);
		string_append(&rutaBloque, ".bin");
		FILE *bloque = fopen(rutaBloque, "wb");

		log_info(gameCard_logger, "copiando pokemon en el bloque %s.bin",bloq);
		fseek(bloque, 0, SEEK_SET);
		fwrite(stringVacio,metadata_fs->tamanioBLoques, 1, bloque);
		//fclose(bloque);

		log_info(gameCard_logger, "se ha borrado coentenido correctamente");

	}
}

void eliminarMetadataPokemon(){}
