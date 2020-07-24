#include "FileSystem.h"

bool existePokemon(char* nombrePokemon) {

	log_info(gameCard_logger, "validando si existe el pokemon: %s",
			nombrePokemon);

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

	pthread_mutex_lock(&semMutexBitmap);

	int libres = 0;

	for (int j = 0; j < bitarray_get_max_bit(bitarray); j++) {

		if (bitarray_test_bit(bitarray, j) == 0) {

			libres = libres + 1;
		}
	}

	pthread_mutex_unlock(&semMutexBitmap);

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

	free(posx);
	free(posy);
	free(cantidad);

	return lineaPokemon;

}

void copiarEnBloque(char* bloqueLibre, char* lineaAcopiar) {

	if (bloqueLibre != NULL) {

		log_info(gameCard_logger, "se va a escribir en bloques del pokemon");

		log_info(gameCard_logger, "accediendo al bloque %s.bin", bloqueLibre);

		log_info(gameCard_logger, "se va a copiar: %s", lineaAcopiar);

		char* rutaBloqueLibre = string_new();

		string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
		string_append(&rutaBloqueLibre, "/");
		string_append(&rutaBloqueLibre, bloqueLibre);
		string_append(&rutaBloqueLibre, ".bin");
		FILE *bloque = fopen(rutaBloqueLibre, "wb");

		free(rutaBloqueLibre);

		/*log_warning(gameCard_logger,"validando ruta : %s", rutaBloqueLibre);*/

		fseek(bloque, 0, SEEK_SET);
		fwrite(lineaAcopiar, string_length(lineaAcopiar), 1, bloque);
		fclose(bloque);

		log_info(gameCard_logger, "se ha copiado correctamente");
	}

	else {
		log_error(gameCard_logger, "error inesperado: bloque libre null!");
		exit(-1);
	}

}

void marcarBloqueOcupado(int bloqueLibre) {

	log_info(gameCard_logger,
			" el bloque %d pasará a estar ocupado en el bitmap", bloqueLibre);

	msync(bitarray, sizeof(bitarray), MS_SYNC);

	bitarray_set_bit(bitarray, bloqueLibre);

	msync(bmap, sizeof(bitarray), MS_SYNC);

	log_info(gameCard_logger,
			"efectivamente paso a tener le valor : %d en el bitmap ",
			bitarray_test_bit(bitarray, bloqueLibre));

}

void agregarBloqueParaMetadataArchivo(char* bloqueLibre) {

	list_add(bloquesMetadataPokemon, bloqueLibre);
	log_info(gameCard_logger,
			"se ha agregado el bloque: %s a la lista de bloques para la metadata",
			bloqueLibre);
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

void crearMetadataArchPoke(char* pokemon, int tamanio, t_list* bloques) {

	crearDirectorioPokemon(pokemon);

	char* rutaMetadata = string_new();

	string_append(&rutaMetadata, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetadata, "/");
	string_append(&rutaMetadata, pokemon);
	string_append(&rutaMetadata, "/Metadata.bin");

	FILE* metadataPoke = fopen(rutaMetadata, "wb");

	log_info(gameCard_logger, "ruta es: %s", rutaMetadata);

	log_info(gameCard_logger, "Creando el archivo metadata del pokemon");

	char* lineaDirectorio = string_new();
	string_append(&lineaDirectorio, "DIRECTORY=N");
	string_append(&lineaDirectorio, "\n");
	rewind(metadataPoke);
	fwrite(lineaDirectorio, string_length(lineaDirectorio), 1, metadataPoke);

	char* tam = string_itoa(tamanio);

	char* lineaSize = string_new();
	string_append(&lineaSize, "SIZE=");
	string_append(&lineaSize, tam);
	string_append(&lineaSize, "\n");
	fwrite(lineaSize, string_length(lineaSize), 1, metadataPoke);

	log_info(gameCard_logger, "el tamanio del archivo en la metadata será: %d",
			tamanio);

	char* lineaBloquesOcupados = string_new();
	int tamanioMaxList = list_size(bloques);
	log_info(gameCard_logger,
			"la cantidad de bloques que va a tener la metadata : %d",
			tamanioMaxList);

	string_append(&lineaBloquesOcupados, "BLOCKS=[");

	char* lineaBloq = string_new();

	for (int i = 0; i < tamanioMaxList; i++) {
		log_info(gameCard_logger, "aca entra en for");
		log_info(gameCard_logger, "pos %d y elem : %s", i,
				list_get(bloques, i));

		if (list_get(bloques, i) != NULL) {

			char* bloq = list_get(bloques, i);

			log_info(gameCard_logger, "mostrame el bloque %s", bloq);
			string_append(&lineaBloq, bloq);

			if (i != (tamanioMaxList - 1)) {

				string_append(&lineaBloq, ",");

			}

		}
	}

	//list_clean(bloquesMetadataPokemon);
	//list_clean(bloquesNuevos);
	string_append(&lineaBloquesOcupados, lineaBloq);
	string_append(&lineaBloquesOcupados, "]\n");
	fwrite(lineaBloquesOcupados, string_length(lineaBloquesOcupados), 1,
			metadataPoke);

	log_info(gameCard_logger, "la lista de bloques en la metadata será:%s",
			lineaBloquesOcupados);

	char* lineaOpen = string_new();
	string_append(&lineaOpen, "OPEN=N");
	fwrite(lineaOpen, string_length(lineaOpen), 1, metadataPoke);

	fclose(metadataPoke);

	free(rutaMetadata);
	free(lineaBloquesOcupados);
	free(lineaOpen);
	free(lineaBloq);
	free(lineaDirectorio);
	free(lineaSize);

	free(tam);
	log_info(gameCard_logger,
			"Se ha creado el archivo metadata.bin del Pokemon");

}

void crearDirectorioPokemon(char* pokemon) {

	log_info(gameCard_logger, "Creando el directorio Pokemon : %s", pokemon);

	char* directorioPoke = string_new();

	string_append(&directorioPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&directorioPoke, "/");
	string_append(&directorioPoke, pokemon);

	int fueCreado = mkdir(directorioPoke, 0777); //analizar permisos
	if (fueCreado == 0) {
		log_info(gameCard_logger, "Se ha creado el directorio / %s", pokemon);
	}

	free(directorioPoke);
}

void copiarEnArchivo(int fd, char* dato, int tamanioDato) {

	write(fd, dato, tamanioDato);

}

int obtenerPrimerBloqueLibre() {

	for (int i = 1; i <= bitarray_get_max_bit(bitarray); i++) {

		if (bitarray_test_bit(bitarray, i) == 0) {

			return i;

		}

	}

}

void crearPokemon(t_new_pokemon* poke) {

	pokemonEnMemoria = string_new();
	posAcopiar = string_new();


	char* pokeCargado = cargarPokemon(poke);
	string_append(&posAcopiar, pokeCargado);
	free(pokeCargado);

	//bloquesNuevos = list_create();

	log_info(gameCard_logger, "iniciando la creacion del archivo...");

	if (noHayEspacioParaPokemon(posAcopiar)) {
		log_error(gameCard_logger, "No hay espacio para un nuevo Pokemon");
		exit(-1);
	}

	int cantBloquesNecesarios = cantBloquesNecesariosPara(posAcopiar);

	log_info(gameCard_logger, "cantidad de bloques que necesita: %d",
			cantBloquesNecesarios);

	pthread_mutex_lock(&semMutexBitmap);
	desde = 0;
	bloquesMetadataPokemon=list_create();
	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);
	list_iterate(bloquesNuevos, persistirCambiosEnBloquesPropios);
	pthread_mutex_unlock(&semMutexBitmap);
	crearMetadataArchPoke(poke->pokemon, string_length(posAcopiar),
			bloquesNuevos);

	list_destroy_and_destroy_elements(bloquesNuevos, destruirBloque);
	list_destroy(bloquesMetadataPokemon);
	//free(bloquesNuevos);
	free(pokemonEnMemoria);
	free(posAcopiar);
	log_info(gameCard_logger,
			"se ha creado con éxito el archivo del pokemon %s", poke->pokemon);
}

void destruirBloque(char* elem) {
	free(elem);
}

bool entraEnBloque(char* lineaPokemon) {

	return metadata_fs->tamanioBLoques >= string_length(lineaPokemon);
}

void copiarPokemonEnMemoria(void* unBloque) {

	log_info(gameCard_logger,
			"abriendo el bloque %s para copiar pokemon en memoria", unBloque);
	char* rutaBloque = string_new();
	unBloq = string_new();

	string_append(&unBloq, unBloque);

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, unBloq);
	string_append(&rutaBloque, ".bin");

	int fdBloq = open(rutaBloque, O_RDWR);

	if (fdBloq <= -1) {
		log_error(gameCard_logger, "Error al abrir el archivo");
		exit(-1);
	}

	struct stat mystat;

	if (fstat(fdBloq, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat");
		close(fdBloq);
		exit(-1);
	};

	/*	if (pokemonEnMemoria == NULL) {
	 pokemonEnMemoria = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
	 MAP_SHARED, fdBloq, 0);
	 }

	 else {*/

	char* mapeo = (char*) mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
	MAP_SHARED, fdBloq, 0);

	string_append(&pokemonEnMemoria, mapeo);

	//free(auxPokeEnMemo);

	//}

	free(unBloq);
	free(rutaBloque);

	close(fdBloq);

}

void llenarListaBloquesPoke(char* poke) {

	agregarSemaforoPokemon(poke);

	char* rutaMetadataPokemon = string_new();

	string_append(&rutaMetadataPokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetadataPokemon, "/");
	string_append(&rutaMetadataPokemon, poke);
	string_append(&rutaMetadataPokemon, "/Metadata.bin");

	t_config* metadataPokemon = config_create(rutaMetadataPokemon);

	pthread_mutex_lock(dictionary_get(semaforosPokemon, poke));

	int tamanioArch = config_get_int_value(metadataPokemon, "SIZE");

	int cantBloq = tamanioArch / metadata_fs->tamanioBLoques;

	if (tamanioArch % metadata_fs->tamanioBLoques != 0) {
		cantBloq++;
	}

	char** listaBloques = config_get_array_value(metadataPokemon, "BLOCKS");

	for (int j = 0; j < cantBloq; j++) {

		if (listaBloques[j] != NULL) {
			char* bloq = listaBloques[j];
			list_add(bloquesMetadataPokemon, bloq);
			log_info(gameCard_logger, "%s", bloq);
			//free(bloq);
		}

	}

	pthread_mutex_unlock(dictionary_get(semaforosPokemon, poke));

	/*int i=0;
	 while(listaBloques[i]!=NULL){
	 free(listaBloques[i]);
	 i++;
	 }*/
	free(listaBloques);
	free(rutaMetadataPokemon);
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
	char* posCharx = string_itoa(posx);
	char* posChary = string_itoa(posy);

	string_append(&nuevaPos, posCharx);
	string_append(&nuevaPos, "-");
	string_append(&nuevaPos, posChary);
	string_append(&nuevaPos, "=");

	free(posCharx);
	free(posChary);

	log_info(gameCard_logger, "la posicion a cambiar es :%s %d ", nuevaPos,
			pokemonAeditar->cantidad);

	nuevaCant = pokemonAeditar->cantidad;

	pokemon = string_new();
	string_append(&pokemon, pokemonAeditar->pokemon);
}

void modificarPokemon(t_new_pokemon* pokemonAeditar) {

	log_info(gameCard_logger, "se va a modificar el archivo");

	pokemonEnMemoria = string_new();

	bloquesMetadataPokemon = list_create();

	pokeAmodificarInicializate(pokemonAeditar);

	log_info(gameCard_logger, "Los bloques que conforman al archivo pokemon:");
	llenarListaBloquesPoke(pokemon);

	log_info(gameCard_logger,
			"conociendo sus bloques, se carga pokemon en memoria");
	copiarPokemonAmemoria(bloquesMetadataPokemon);

	log_info(gameCard_logger, "el pokemon que se cargo en memoria: %s",
			pokemonEnMemoria);

	if (estaPosicionEnMemoria(pokemonEnMemoria, nuevaPos)) {

		log_info(gameCard_logger,
				"La posicion existe en el archivo, hay que modificarla");

		modificarPosicion(nuevaPos, nuevaCant, pokemonEnMemoria);

	}

	else {

		log_info(gameCard_logger,
				"la posicion no existe en el archivo del pokemon %s, hay que agregarla",
				pokemon);

		int posUltimoBloque = list_size(bloquesMetadataPokemon) - 1;

		char* ultBloque = list_get(bloquesMetadataPokemon, posUltimoBloque);

		//bloqEnMemo = string_new();

		bloqEnMemo = traerAmemoriaUltimoBloque(ultBloque);

		char* posCant = string_itoa(nuevaCant);
		string_append(&nuevaPos, posCant);
		free(posCant);

		string_append(&nuevaPos, "\n");

		int espacioOcupadoBloque = string_length(bloqEnMemo);

		int espacioEnBloque = metadata_fs->tamanioBLoques
				- espacioOcupadoBloque;

		int sizeMetadata = obtenerEspacioMetadata(pokemon);

		int espacioNuevaLinea = string_length(nuevaPos);

		int nuevoSizeMetadata = sizeMetadata + espacioNuevaLinea;

		char* stringAcopiar = string_new();

		string_append(&stringAcopiar, bloqEnMemo);

		//free(bloqEnMemo);

		string_append(&stringAcopiar, nuevaPos);

		if (espacioEnBloque >= espacioNuevaLinea) {

			log_info(gameCard_logger,
					"el archivo tiene espacio y se copia la posicion en el último bloque que le fue asignado");

			agregarPosicionEnUltimoBLoque(ultBloque, stringAcopiar,
					nuevoSizeMetadata);

		} else {

			log_info(gameCard_logger,
					"no alcanza el espacio que existe en el ultimo bloque asignado");

			log_info(gameCard_logger,
					"se analiza si hay bloques libres disponibles en el fs...");

			if (espacioLibreEnfs() == 0) {

				log_info(gameCard_logger, "no hay bloques libres en el fs");
				log_info(gameCard_logger,
						"se quitara salto de linea y se validará si entra en el ultimo bloque asignado");

				agregarPosicionSinSaltoDeLinea(espacioEnBloque,
						espacioNuevaLinea, ultBloque, nuevoSizeMetadata);

			} else {

				int debug = 0;

				if (debug) {
					log_debug(gameCard_logger,
							"ult bloq %s, espacio ocupado : %d, espacioLibre :%d,a copiar %s",
							ultBloque, stringAcopiar, espacioOcupadoBloque,
							espacioEnBloque, espacioNuevaLinea,
							nuevoSizeMetadata);
				}

				log_info(gameCard_logger,
						"hay bloques libres disponibles en el fs");
				agregarPosicionPokemonAbloquesNuevos(ultBloque, stringAcopiar,
						espacioOcupadoBloque, espacioEnBloque,
						espacioNuevaLinea, nuevoSizeMetadata);
			}
		}

		free(stringAcopiar);
		free(ultBloque);

	}

	free(pokemonEnMemoria);
	free(pokemon);
	free(nuevaPos);

	list_destroy(bloquesMetadataPokemon);

}

void liberarElem(void* elem) {
	free(elem);
}

int obtenerEspacioMetadata(char* pokemon) {

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

	int sizeMetadata = config_get_int_value(configPoke, "SIZE");

	pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

	free(rutaPoke);
	config_destroy(configPoke);

	return sizeMetadata;
}

void agregarPosicionSinSaltoDeLinea(int espacioEnBloque, int espacioNuevaLinea,
		char* ultBloque, int nuevoSizeMetadata) {

	if (string_ends_with(nuevaPos, "\n")) {

		char* nuevaPosSinSaltoDeLinea = string_substring(nuevaPos, 1,
				string_length(nuevaPos) - 1);

		char* copiarPos = string_new();

		string_append(&copiarPos, bloqEnMemo);

		string_append(&copiarPos, nuevaPosSinSaltoDeLinea);

		espacioNuevaLinea = string_length(nuevaPosSinSaltoDeLinea);

		if (espacioEnBloque == espacioNuevaLinea) {

			agregarPosicionEnUltimoBLoque(ultBloque, copiarPos,
					nuevoSizeMetadata - 1);

		}

		else {

			log_error(gameCard_logger,
					"no hay espacio para agregar nueva posicion");
		}
	}
}

void modificarPosicion(char* nuevaPos, int cantidad, char* pokemonEnMemoria) {

	copiado = desde = 0;

	int tamanioMetadata;

	posAcopiar = string_new();

	char** posiciones = string_split(pokemonEnMemoria, "\n");

	string_iterate_lines(posiciones, agregarCantidadNuevaAposicion);

	free(posiciones);

	/*	int j=0;
	 while(posiciones[j]!=NULL){
	 free(posiciones[j]);
	 j++;
	 }*/

	int cantBloqNecesarios = cantBloquesNecesariosPara(posAcopiar);

	int cantBloqOcupados = cantBloquesNecesariosPara(pokemonEnMemoria);

//	free(pokemonEnMemoria);

	int bloqNuevos = cantBloqNecesarios - cantBloqOcupados;

	if (bloqNuevos == 0) {

		log_info(gameCard_logger,
				"no se requieren nuevos bloques ante la modificacion del archivo");

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			char* elem = list_get(bloquesMetadataPokemon, i);

			if (elem != NULL) {
				persistirCambiosEnBloquesPropios(
						list_get(bloquesMetadataPokemon, i));
			} else {

				log_error(gameCard_logger, "error al modificar el archivo");
			}
		}

		tamanioMetadata = string_length(posAcopiar);

		if (obtenerEspacioMetadata(pokemon) != tamanioMetadata) {
			pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
			cambiarTamanioMetadata(pokemon, tamanioMetadata);
			modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);
			pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));
		}

		log_info(gameCard_logger, "se ha modificado correctamente el pokemon");

	}

	else {

		log_info(gameCard_logger,
				"se necesitan nuevos bloques para modifcar el pokemon");
		bloquesNuevos = list_create();

		bloquesNuevos = obtenerBloquesNuevos(cantBloqNecesarios);

		copiado = desde = 0;

		log_info(gameCard_logger,
				"se van a copiar en el archivo las modificaciones");

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			int aCopiar = string_length(posAcopiar);
			;
			if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

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

		//validar cuando

		tamanioMetadata = string_length(posAcopiar);

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
		cambiarTamanioMetadata(pokemon, tamanioMetadata);
		modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);
		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));
		log_info(gameCard_logger, "se ha modificado correctamente el pokemon");
	}

}

char* traerAmemoriaUltimoBloque(char* ultBloque) {

	char* rutaBloque;
	rutaBloque = string_new();

	char* ultBloq = string_new();
	string_append(&ultBloq, ultBloque);

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, ultBloq);
	string_append(&rutaBloque, ".bin");

	free(ultBloq);
	int fdBloq = open(rutaBloque, O_RDWR);

	if (fdBloq <= -1) {
		log_error(gameCard_logger, "Error al abrir el archivo");
	}

	struct stat mystat;

	if (fstat(fdBloq, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat\n");
		close(fdBloq);
	}

	char* bloqueEnMemo;

	bloqueEnMemo = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
	MAP_SHARED, fdBloq, 0);

	if (bloqueEnMemo == NULL) {

		log_error(gameCard_logger, "hay error en la carga en memoria");
	}

	free(rutaBloque);
	return bloqueEnMemo;

}

void agregarPosicionEnUltimoBLoque(char* ultBloq, char* stringAcopiar,
		int nuevoSizeMetadata) {

	log_info(gameCard_logger, "Se copia %s en el bloque: %s.bin", stringAcopiar,
			ultBloq);

	if (stringAcopiar == NULL) {

		log_error(gameCard_logger, "Error en los datos a copiar");
	}

	pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

	copiarEnBloque(ultBloq, stringAcopiar);

	cambiarTamanioMetadata(pokemon, nuevoSizeMetadata);

	pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

	log_info(gameCard_logger, "Se agrego correctamente la posicion");

}

void agregarPosicionPokemonAbloquesNuevos(char* ultBloque, char* stringAcopiar,
		int espacioOcupadoBloque, int espacioEnBloque, int espacioNuevaLinea,
		int nuevaSizeMetadata) {

	log_info(gameCard_logger,
			"ult Bloq: %s, a copiar: %s, espacio en bloqocupado:%d y espacio %d y espacio nueva linea %d",
			ultBloque, stringAcopiar, espacioOcupadoBloque, espacioEnBloque,
			espacioNuevaLinea);

	char* copiarEnUltBloque = string_new();

	string_append(&copiarEnUltBloque, bloqEnMemo);

	if ((espacioEnBloque == 0) & (!string_ends_with(copiarEnUltBloque, "\n"))) {

		string_append(&copiarEnUltBloque, "\n");

		espacioNuevaLinea = espacioNuevaLinea + 1;
	}

	//log_info(gameCard_logger,"mostrame que hay en copiarEnULtimoBloque %s", copiarEnUltBloque);

	char* auxPokeEnMemo = string_substring(stringAcopiar, espacioOcupadoBloque,
			espacioEnBloque);
	string_append(&copiarEnUltBloque, auxPokeEnMemo);
	free(auxPokeEnMemo);

	//log_info(gameCard_logger, "aca se va a copiar en ult bloque %s",copiarEnUltBloque);

	copiarEnBloque(ultBloque, copiarEnUltBloque);

	free(copiarEnUltBloque);

	char* acopiarEnBloq = string_substring(stringAcopiar, espacioEnBloque,
			espacioNuevaLinea - espacioEnBloque);
	int cantBloquesNecesarios = cantBloquesNecesariosPara(acopiarEnBloq);

	free(acopiarEnBloq);

	log_info(gameCard_logger, "cantidad de bloques que se necesitan: %d",
			cantBloquesNecesarios);

	//bloquesNuevos = list_create();

	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);

	copiado = 0;

	desde = metadata_fs->tamanioBLoques;

	char* auxCopiar = string_substring(stringAcopiar, desde,
			string_length(stringAcopiar));
	int aCopiar = string_length(auxCopiar);
	free(auxCopiar);

	for (int i = 0; i < list_size(bloquesNuevos); i++) {

		/*log_info(gameCard_logger,"ACA QÉ COPIAS? %s",string_substring(stringAcopiar, metadata_fs->tamanioBLoques,
		 aCopiar));*/

		if ((aCopiar - copiado) <= metadata_fs->tamanioBLoques) {

			/*log_info(gameCard_logger, "mostrame que va a copiar: %s",
			 string_substring(stringAcopiar, desde,
			 aCopiar-copiado));*/

			char* copiarABloque = string_substring(stringAcopiar, desde,
					aCopiar);
			copiarEnBloque(list_get(bloquesNuevos, i), copiarABloque);
			free(copiarABloque);
		}

		else {

			/*log_error(gameCard_logger,"copiar-copiado = %d",(aCopiar - copiado));

			 log_info(gameCard_logger,"avisame el desde %d",desde);

			 log_warning(gameCard_logger,"mostrame que le mandas a copiaEnBloq");

			 log_info(gameCard_logger," se va a copiar : %s",string_substring(stringAcopiar, desde,
			 metadata_fs->tamanioBLoques));*/

			char* aCopiaEnBloq = string_substring(stringAcopiar, desde,
					metadata_fs->tamanioBLoques);
			copiarEnBloque(list_get(bloquesNuevos, i), aCopiaEnBloq);
			free(aCopiaEnBloq);

			desde = desde + metadata_fs->tamanioBLoques;

		}

	}

	pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
	cambiarTamanioMetadata(pokemon, nuevaSizeMetadata);
	modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);
	pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

}

bool estaPosicionEnMemoria(char* pokemonEnMemoria, char* nuevaPos) {

	log_info(gameCard_logger,
			"verificando si la posicion existe en el archivo del pokemon");
	return (string_contains(pokemonEnMemoria, nuevaPos) == 1);

}

void contardorDeBloquesOcupadosPorPokemon(char** bloksOcupadosPorPokemon) {
	string_iterate_lines(bloksOcupadosPorPokemon, sumameBloques);
}

void copiarPokemonAmemoria(t_list* listBloqPokemon) {

	for (int i = 0; i < list_size(listBloqPokemon); i++) {

		//log_warning(gameCard_logger,"aca mostrame pos en lista");
		//log_warning(gameCard_logger, "aca mostrame num bloque %s", bloq);
		copiarPokemonEnMemoria(list_get(listBloqPokemon, i));

	}

	//list_iterate(listBloqPokemon, copiarPokemonEnMemoria);
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

	persistirCambiosEnBloquesPropios(bloqueNuevo);

}
void cambiarTamanioMetadata(char* pokemon, int tamanioNuevo) {
	//int tamanioAgregar) {

	log_info(gameCard_logger, "se modifica el tamanio del archivo pokemon %s",
			pokemon);

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	//int cantidadNueva = (config_get_int_value(configPoke, "SIZE"))
	//+ tamanioAgregar;

	log_info(gameCard_logger, "El nuevo tamaño del archivo será: %d",
			tamanioNuevo);
	//cantidadNueva);

	//pthread_mutex_lock(dictionary_get(semaforosPokemon,pokemon));

	char* nuevoTamanioMeta = string_itoa(tamanioNuevo);
	config_set_value(configPoke, "SIZE", nuevoTamanioMeta);
	//string_itoa(cantidadNueva));
	free(nuevoTamanioMeta);
	config_save_in_file(configPoke, rutaPoke);

//	pthread_mutex_unlock(dictionary_get(semaforosPokemon,pokemon));

	log_info(gameCard_logger,
			"se ha modificado el size del archivo adecuadamente");

	free(rutaPoke);
	config_destroy(configPoke);

}

void obtenerCantBloques(char* bloque) {

	cantBloquesPoke = cantBloquesPoke + 1;
}

t_list* obtenerBloquesNuevos(int cantBloqNecesarios) {

	t_list* bloquesLibres;

	bloquesLibres = list_create();

	while (cantBloqNecesarios != 0) {

		int bloqLib = obtenerPrimerBloqueLibre();

		if (bloqLib != NULL) {

			log_info(gameCard_logger, "se le asigna al pokemon el bloque %d",
					bloqLib);

			char* bloque = string_itoa(bloqLib);
			list_add(bloquesLibres, bloque);

			marcarBloqueOcupado(bloqLib);

			agregarBloqueParaMetadataArchivo(bloque);
		}

		else {
			log_error(gameCard_logger,
					"error inesperado, el bloque esta en null!");
			exit(-1);
		}

		cantBloqNecesarios = cantBloqNecesarios - 1;

	}

	return bloquesLibres;
}

void persistirCambiosEnBloquesPropios(void* bloque) {

	int aCopiar = string_length(posAcopiar);

	char* bloq = (char*) bloque;

	log_warning(gameCard_logger, "aca entro en persistir en bloques propios");
	if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

		log_warning(gameCard_logger, "aCopiar %d,copiado %d, desde %d", aCopiar,
				copiado, desde);
		char* auxString = string_substring(posAcopiar, desde, aCopiar - desde);
		copiarEnBloque(bloq, auxString);
		free(auxString);
	}

	else {

		/*log_warning(gameCard_logger,"aca me dijiste que llegabas");*/

		log_warning(gameCard_logger, "aCopiar %d,copiado %d, desde %d", aCopiar,
				copiado, desde);

		char* auxString = string_substring(posAcopiar, desde,
				metadata_fs->tamanioBLoques);
		copiarEnBloque(bloq, auxString);

		desde = desde + metadata_fs->tamanioBLoques;
		free(auxString);

	}

	//free(bloq);
//log_warning(gameCard_logger,"aCopiar %d,copiado %d, desde %d",aCopiar,copiado,desde);
}

void agregarCantidadNuevaAposicion(char* posicion) {

	if (string_starts_with(posicion, nuevaPos) == 1) {

		tamanioNuevaPos = string_length(nuevaPos);

		log_info(gameCard_logger,
				"A la cantidad de la posicion %s se ve a sumar %d", posicion,
				nuevaCant);

		char* tamanioPos = string_substring_from(posicion, tamanioNuevaPos);
		nuevaCant = nuevaCant + atoi(tamanioPos);

		free(tamanioPos);

		log_info(gameCard_logger, "la nueva posicion es : %s %d", nuevaPos,
				nuevaCant);

		char* cant = string_itoa(nuevaCant);
		string_append(&posAcopiar, nuevaPos);
		string_append(&posAcopiar, cant);
		string_append(&posAcopiar, "\n");
		free(cant);

	} else {

		string_append(&posAcopiar, posicion);
		string_append(&posAcopiar, "\n");
	}

	free(posicion);
}

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

	char* lineaBloquesOcupados = string_new();
	int tamanioMaxList = list_size(bloquesPokemon);
	string_append(&lineaBloquesOcupados, "[");

	for (int i = 0; i < tamanioMaxList; i++) {
		/*log_info(gameCard_logger, "aca entra en for");
		 log_info(gameCard_logger, "pos %d y elem : %s", i,list_get(bloquesMetadataPokemon, i));
		 */
		if (list_get(bloquesMetadataPokemon, i) != NULL) {
			string_append(&lineaBloquesOcupados,
					list_get(bloquesMetadataPokemon, i));
		} else {
			log_error(gameCard_logger,
					"error inesperado,la pocion se encuentra en null!");
		}

		if (i != (tamanioMaxList - 1)) {

			string_append(&lineaBloquesOcupados, ",");

		}

	}

	list_clean(bloquesMetadataPokemon);
	string_append(&lineaBloquesOcupados, "]");

	//pthread_mutex_lock(dictionary_get(semaforosPokemon,pokemon));

	config_set_value(configPoke, "BLOCKS", lineaBloquesOcupados);

	config_save_in_file(configPoke, rutaPoke);

	free(rutaPoke);
	config_destroy(configPoke);

	//pthread_mutex_unlock(dictionary_get(semaforosPokemon,pokemon));

	log_info(gameCard_logger, "la nueva lista de bloques del archivo: %s",
			lineaBloquesOcupados);
	free(lineaBloquesOcupados);
}

bool estaAbiertoArchivo(char* pokemon) {

	log_info(gameCard_logger, "validando si el archivo se encuentra abierto");

	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

		log_info(gameCard_logger, "se va a activar el semaforo del pokemon");

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));


		char* estadoArchivo = string_new();
		string_append(&estadoArchivo, config_get_string_value(configPoke, "OPEN"));

		log_info(gameCard_logger, "El estado del archivo es %s", estadoArchivo);

		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

		free(rutaPoke);
		config_destroy(configPoke);

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

	if (!estaAbiertoArchivo(poke)) {

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

		config_set_value(configPoke, "OPEN", "Y");

		config_save_in_file(configPoke, rutaPoke);

		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

		log_info(gameCard_logger, "Se ha abierto el archivo del pokemon: %s",
				poke);
	}
}

void cerrarArchivo(char* poke) {

	log_info(gameCard_logger, "se va a cerrar el archivo del pokemon %s", poke);
	t_config* configPoke;

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, poke);
	string_append(&rutaPoke, "/Metadata.bin");

	configPoke = config_create(rutaPoke);

	pthread_mutex_lock(dictionary_get(semaforosPokemon, poke));

	config_set_value(configPoke, "OPEN", "N");

	config_save_in_file(configPoke, rutaPoke);

	pthread_mutex_unlock(dictionary_get(semaforosPokemon, poke));

	log_info(gameCard_logger, "se ha cerrado el archivo del pokemon");

	free(rutaPoke);
	config_destroy(configPoke);

}

uint32_t capturarPokemon(t_catch_pokemon* pokeAatrapar) {

	posAcopiar=string_new();
	log_info(gameCard_logger, "se intentará capturar un pokemon");

	bloquesMetadataPokemon = list_create();

	nuevaPos = string_new();

	int posx = pokeAatrapar->coordenadas.posx;
	int posy = pokeAatrapar->coordenadas.posy;

	char* possx = string_itoa(posx);
	char* possy = string_itoa(posy);

	string_append(&nuevaPos, possx);
	string_append(&nuevaPos, "-");
	string_append(&nuevaPos, possy);
	string_append(&nuevaPos, "=");

	log_info(gameCard_logger, "la posicion a buscar es (%d,%d) "
			"del pokemon %s", posx, posy, pokeAatrapar->pokemon);

	free(possx);
	free(possy);

	pokemon = string_new();
	string_append(&pokemon, pokeAatrapar->pokemon);

	llenarListaBloquesPoke(pokemon);

	pokemonEnMemoria = string_new();

	copiarPokemonAmemoria(bloquesMetadataPokemon);

	if (estaPosicionEnMemoria(pokemonEnMemoria, nuevaPos)) {

		log_info(gameCard_logger,
				"se validó que la posicion existe en el archivo");

		log_info(gameCard_logger,"mostrame poke en memo: %s",pokemonEnMemoria);
		int cantBloqOcupados = cantBloquesNecesariosPara(pokemonEnMemoria);

		log_info(gameCard_logger,"cant de bloques ocupados: %d",cantBloqOcupados);

		char** posiciones = string_split(pokemonEnMemoria, "\n");

		string_iterate_lines(posiciones, capturarPokeEnPos);

		int i = 0;
		while (posiciones[i] != NULL) {
			free(posiciones[i]);
			i++;
		}

		free(posiciones);

		log_info(gameCard_logger,"pos a copiar es: %s", posAcopiar);
		int cantBloqSinPosicion = cantBloquesNecesariosPara(posAcopiar);

		log_info(gameCard_logger,"cant de bloques sin posicion: %d",cantBloqSinPosicion);

		if (cantBloqSinPosicion == 0) {

			log_info(gameCard_logger,
					"los bloques del pokemon se quedaran vacios con su captura");

			log_info(gameCard_logger, "se va a eliminar el pokemon");
			marcarBloquesLibres(bloquesMetadataPokemon);

			pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
			vaciarBloques(bloquesMetadataPokemon);
			eliminarMetadataPokemon(pokemon);
			pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

		}

		else {

			int cantBloqAliberar = cantBloqOcupados - cantBloqSinPosicion;

			log_info(gameCard_logger,
					"la cantidad de bloques a liberar porque quedan vacios: %d",
					cantBloqAliberar);

			int tamanioListBloques = list_size(bloquesMetadataPokemon);

			log_info(gameCard_logger,"mostrame cant bloque en metadata: %d",tamanioListBloques);

			if(cantBloqAliberar==0){

		log_info(gameCard_logger, " se va a actualizar el contenido del archivo");

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			desde = 0;
		persistirCambiosEnBloquesPropios(list_get(bloquesMetadataPokemon, i));

		}
			}


			for (int i = 0; i < cantBloqAliberar; i++) {

				char* elem = string_new();

				string_append(&elem,
						list_get(bloquesMetadataPokemon,
								tamanioListBloques - 1 - i));

				marcarBloqueLibreBitmap(atoi(elem));

				limpiarBloque(elem);

				list_remove(bloquesMetadataPokemon, tamanioListBloques - 1 - i);

				log_info(gameCard_logger,
						" se va a actualizar el contenido del archivo");

				for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

					desde = 0;
					persistirCambiosEnBloquesPropios(
							list_get(bloquesMetadataPokemon, i));

				}

				log_info(gameCard_logger,
						"Se ha actualizado el archivo (los bloques) del pokemon");

				pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

				cambiarTamanioMetadata(pokemon, string_length(posAcopiar));

				modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);

				pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

			}


			cerrarArchivo(pokemon);

			sleep(tiempo_retardo_operacion);

			return OK;
		}
	}

	else {

		log_error(gameCard_logger,
				"No existe la posicion dentro del archivo del pokemon: %s",
				pokemon);

		cerrarArchivo(pokemon);

		sleep(tiempo_retardo_operacion);

		return FAIL;
	}

}

void marcarBloquesLibres(t_list* bloquesOcupados) {

	for (int i = 0; i < list_size(bloquesOcupados); i++) {

		log_info(gameCard_logger, "Marcar posicion %d del bitmap como libre",
				i);

		marcarBloqueLibreBitmap(atoi(list_get(bloquesOcupados, i)));

		log_info(gameCard_logger, "La poscion del bitmap paso a: %d",
				bitarray_test_bit(bitarray, i));
	}

}

void marcarBloqueLibreBitmap(int numBloque) {

	pthread_mutex_lock(&semMutexBitmap);

	bitarray_clean_bit(bitarray, numBloque);

	msync(bmap, sizeof(bitarray), MS_SYNC);

	pthread_mutex_unlock(&semMutexBitmap);

	log_info(gameCard_logger,
			"se ha marcado como libre en el bitmap el bloque %d", numBloque);

}

void capturarPokeEnPos(char* posicion) {

	log_info(gameCard_logger, "mostrame posicion : %s", posicion);

	if (string_starts_with(posicion, nuevaPos)) {

		log_info(gameCard_logger, "la posicion %s existe en el archivo",
				posicion);

		tamanioNuevaPos = string_length(nuevaPos);

		int cantEnPos = atoi(string_substring_from(posicion, tamanioNuevaPos));

		log_info(gameCard_logger, "La cantidad es: %d", cantEnPos);

		if (cantEnPos >= 1) {

			log_info(gameCard_logger,
					"se decrementa en 1 unidad la cantidad del pokemon");

			cantEnPos = cantEnPos - 1;

			if (cantEnPos == 0) {
				log_info(gameCard_logger, "la cantidad pasa a: %d", cantEnPos);
				log_info(gameCard_logger, "se va a eliminar la posicion: %s",
						posicion);
			}

			else {

				log_info(gameCard_logger, "la cantidad pasa a : %d", cantEnPos);

				string_append(&posAcopiar, nuevaPos);
				string_append(&posAcopiar, string_itoa(cantEnPos));
				string_append(&posAcopiar, "\n");

				log_info(gameCard_logger, "la posicion queda: %s", nuevaPos);

			}

		}


	}

	else {

		log_info(gameCard_logger,"voy a copiar la pos : %s en posAcopiar:%s",posicion,posAcopiar);

				string_append(&posAcopiar, posicion);
				string_append(&posAcopiar, "\n");
			}
}

void vaciarBloques(t_list* bloquesMetadataPokemon) {

	for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

		char* bloq = string_new();

		string_append(&bloq, list_get(bloquesMetadataPokemon, i));

		limpiarBloque(bloq);

	}
}

void limpiarBloque(char* bloq) {

	log_info(gameCard_logger, "bloque accedido es : %s", bloq);

	char* rutaBloque = string_new();

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, bloq);
	string_append(&rutaBloque, ".bin");

	remove(rutaBloque);

	FILE* estaCreado = fopen(rutaBloque, "w+b");

	if (estaCreado == NULL) {

		log_error(gameCard_logger, "Se ha producido un error");
	}

	else {
		log_info(gameCard_logger, "se ha borrado contenido correctamente");

	}

	free(rutaBloque);

}

void eliminarMetadataPokemon(char* poke) {

	char* rutaPoke = string_new();
	string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaPoke, "/");
	string_append(&rutaPoke, pokemon);
	string_append(&rutaPoke, "/Metadata.bin");


	int estaBorrado = remove(rutaPoke);

	free(rutaPoke);

	if (estaBorrado == 0) {
		log_info(gameCard_logger, " Se ha borrado la metadata del pokemon:%s",
				poke);

		char* directorioPoke = string_new();
		string_append(&directorioPoke, rutas_fs->pathDirectorioFilesMetadata);
		string_append(&directorioPoke, "/");
		string_append(&directorioPoke, pokemon);

		int seBorroDirectorio = rmdir(directorioPoke);

		if (seBorroDirectorio == 0) {

			log_info(gameCard_logger,
					"se ha borrado correctamente el pokemon: %s", poke);
		}

		else {
			log_error(gameCard_logger, "error al borrar el directorio: %s",
					poke);
		}

		free(directorioPoke);

	}

	else {
		log_error(gameCard_logger, "Error al intentar borrar metadata de %s",
				poke);
	}

	//free(rutaPoke);
}

t_list* obtenerPosicionesPokemon(char* pokemon) {

	log_info(gameCard_logger,
			"se intenta obtener las posiciones del pokemon : %s", pokemon);
	log_info(gameCard_logger, "los bloques que ocupa:");

	bloquesMetadataPokemon = list_create();
	llenarListaBloquesPoke(pokemon);

	pokemonEnMemoria = string_new();
	copiarPokemonAmemoria(bloquesMetadataPokemon);

	//log_info(gameCard_logger, "De ellos se copio en memoria las posiciones: %s", pokemonEnMemoria);

	char** posiciones = string_split(pokemonEnMemoria, "\n");

	pokemonesParaLocalized = list_create();

	string_iterate_lines(posiciones, agregarPosicionAlistaParaLocalized);


	log_info(gameCard_logger,
			" las posiciones del pokemon %s obtenidas :", pokemon);

	for (int i = 0; i < list_size(pokemonesParaLocalized); i++) {

		t_coordenadas * coord = list_get(pokemonesParaLocalized, i);
		log_info(gameCard_logger, "(%d, %d) \n", coord->posx, coord->posy);
	}

log_info(gameCard_logger,"espera de acceso a disco : %d segundos",tiempo_retardo_operacion);
	sleep(tiempo_retardo_operacion);
	cerrarArchivo(pokemon);
	free(posiciones);

	return pokemonesParaLocalized;

}

void agregarPosicionAlistaParaLocalized(char* posicion) {

	char** parse = string_split(posicion, "=");
	/*DOS ELEMENTOS
	 1-3=7
	 */
	int cantidad_pokemons = atoi(parse[1]);

	/*free(parse[0]);
	 free(parse[1]);
	 free(parse);*/

	char** parse1 = string_split(parse[0], "-");

	int posx = atoi(parse1[0]);
	int posy = atoi(parse1[1]);

	free(parse1[0]);
	free(parse1[1]);
	free(parse);
	free(parse1);
	/* free(parse[0]);
	 free(parse[1]);
	 free(parse); */

	t_coordenadas_cantidad * elemento = malloc(sizeof(t_coordenadas_cantidad));

	elemento->coordenadas.posx = posx;
	elemento->coordenadas.posy = posy;

	list_add(pokemonesParaLocalized, (void*) elemento);
	//free(parse);
	//free(parse[1]);
	free(posicion);
}

void liberarMemoria() {
//aca empezar a liberar memoria
	free(rutas_fs);
	free(metadata_fs);
//munmap(bmap, tamBmap);
	bitarray_destroy(bitarray);
	config_destroy(config_game_card);
}

/*************************semáforos****************************/

void agregarSemaforoPokemon(char* poke) {

	pthread_mutex_lock(&mutexSemPokemones);

	if (!dictionary_has_key(semaforosPokemon, poke)) {

		static pthread_mutex_t semPoke = PTHREAD_MUTEX_INITIALIZER;

		dictionary_put(semaforosPokemon, poke, &semPoke);

		log_info(gameCard_logger, "Se agrega un semáforo "
				"para la metadata del pokemon %s", poke);

	}

	pthread_mutex_unlock(&mutexSemPokemones);

}

void eliminarSemaforoPokemon(char* poke) {

	pthread_mutex_lock(&mutexSemPokemones);

	dictionary_remove(semaforosPokemon, poke);

	pthread_mutex_unlock(&mutexSemPokemones);

	log_info(gameCard_logger, "se elimina el "
			"semáforo de la metadata del pokemon %s", poke);

}

void inicializarSemaforosParaPokemon() {

	semaforosPokemon = dictionary_create();
	pthread_mutex_init(&semMutexBitmap, NULL);
	pthread_mutex_init(&mutexSemPokemones, NULL);
}

/****************fin semáforos***************************/

void desconectarFs() {

	if (metadata_fs != NULL) {
		log_info(gameCard_logger,
				"se va a liberar estructuras de la metadata...");
		free(metadata_fs->magicNumber);
		free(metadata_fs);
		log_info(gameCard_logger, "se ha liberado correctamente");
	}

	if (rutas_fs != NULL) {
		log_info(gameCard_logger,
				"se va a liberar estructuras de las rutas del fs...");
		free(rutas_fs->pathArchivoBitMap);
		free(rutas_fs->pathArchivoMetadataFs);
		free(rutas_fs->pathDirectorioBloques);
		free(rutas_fs->pathDirectorioFilesMetadata);
		free(rutas_fs->pathDirectorioMetadataFs);
		free(rutas_fs->puntoDeMontaje);
		free(rutas_fs);
		log_info(gameCard_logger, "se ha liberado correctamente");
	}

	if (bitarray != NULL) {
		log_info(gameCard_logger,
				"se va a liberar memoria que se utilizo para el bitarray...");
		bitarray_destroy(bitarray);
		log_info(gameCard_logger, "se ha liberado correctamente");
	}

	if (bmap != NULL) {
		log_info(gameCard_logger,
				"se va a liberar memoria que se utilizo para el bitmap...");
		munmap(bmap, mystat.st_size);
		log_info(gameCard_logger, "se ha liberado correctamente");
	}

	if(bloquesMetadataPokemon != NULL){
	            list_destroy(bloquesMetadataPokemon);}

	/*if(string_length(unBloq)>0){
	 free(unBloq);
	 }*/
	/*if (paquete!=NULL){
	 log_info(gameCard_logger,"se libera la memoria del paquete de conexion");
	 free(paquete);
	 log_info(gameCard_logger,"se ha liberado correctamente");
	 }
	 */
	/*if(pokemonEnMemoria!=NULL){
	 log_info(gameCard_logger,"se va a liberar pokemon de memoria...");

	 free(pokemonEnMemoria);
	 log_info(gameCard_logger,"se ha liberado correctamente");

	 }*/
}
