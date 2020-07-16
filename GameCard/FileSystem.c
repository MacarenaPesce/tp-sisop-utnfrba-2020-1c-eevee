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

	int libres = 0;

	pthread_mutex_lock(&semMutexBitmap);

	for (int j = 1; j <= bitarray_get_max_bit(bitarray); j++) {

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

	return lineaPokemon;

}

void copiarEnBloque(char* bloqueLibre, char* lineaAcopiar) {

	log_warning(gameCard_logger,"ACA LLEGAS BIEN");

	log_info(gameCard_logger, "se va a escribir en bloques del pokemon");

	log_info(gameCard_logger, "accediendo al bloque %s.bin", bloqueLibre);

	log_info(gameCard_logger, "se va a copiar: %s", lineaAcopiar);

	char* rutaBloqueLibre = string_new();

	string_append(&rutaBloqueLibre, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloqueLibre, "/");
	string_append(&rutaBloqueLibre, bloqueLibre);
	string_append(&rutaBloqueLibre, ".bin");
	FILE *bloque = fopen(rutaBloqueLibre, "wb");

log_warning(gameCard_logger,"validando ruta : %s", rutaBloqueLibre);

	fseek(bloque, 0, SEEK_SET);
	fwrite(lineaAcopiar, string_length(lineaAcopiar), 1, bloque);
	fclose(bloque);

	contenidoBloque = string_new();

	log_info(gameCard_logger, "se ha copiado correctamente");
}

void marcarBloqueOcupado(int bloqueLibre) {

	log_info(gameCard_logger,
			" el bloque %d pasará a estar ocupado en el bitmap", bloqueLibre);

	bitarray_set_bit(bitarray, bloqueLibre);

	log_info(gameCard_logger,
			"efectivamente paso a tener le valor : %d en el bitmap ",
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

	crearDirectorioPokemon(pokemon);

	char* rutaMetadata = string_new();

	string_append(&rutaMetadata, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetadata, "/");
	string_append(&rutaMetadata, pokemon);
	string_append(&rutaMetadata, "/Metadata.bin");

	FILE* metadataPoke = fopen(rutaMetadata, "wb");

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

		//log_info(gameCard_logger, "aca entra en for");
		//log_info(gameCard_logger, "pos %d y elem : %s", i,list_get(bloquesMetadataPokemon, i));

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

	return -1;
}

void crearPokemon(t_new_pokemon* poke) {

	pokemonEnMemoria = string_new();
	posAcopiar = string_new();

	string_append(&posAcopiar, cargarPokemon(poke));

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

	pthread_mutex_lock(&semMutexBitmap);
	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);
	list_iterate(bloquesNuevos, persistirCambiosEnBloquesPropios);
	pthread_mutex_unlock(&semMutexBitmap);
	//veer aca que se puede colgar el semaforo

	crearMetadataArchPoke(poke->pokemon, string_length(posAcopiar));

	log_info(gameCard_logger,
			"se ha creado con éxito el archivo del pokemon %s", poke->pokemon);
}

bool entraEnBloque(char* lineaPokemon) {

	return metadata_fs->tamanioBLoques >= string_length(lineaPokemon);
}

void copiarPokemonEnMemoria(void* unBloque) {

	char* rutaBloque;
	rutaBloque = string_new();

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, unBloque);
	string_append(&rutaBloque, ".bin");

	log_warning(gameCard_logger,"mostrame la rura: %s",rutaBloque);

	int fdBloq = open(rutaBloque, O_RDWR);

	log_warning(log_info,"mostrame que aparece en la ruta");

	log_warning(log_info,"mostrame que te llege como open %d",fdBloq);
	if (fdBloq <= -1) {
		log_error(gameCard_logger, "Error al abrir el archivo");
	}

	struct stat mystat;

	if (fstat(fdBloq, &mystat) < 0) {
		log_error(gameCard_logger, "Error en el fstat");
		close(fdBloq);
	}

	log_warning(gameCard_logger,"aca pasame tamanio arch: %d",mystat.st_size);
	
	if (pokemonEnMemoria == NULL) {

		pokemonEnMemoria = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ,
		MAP_SHARED, fdBloq, 0);
	}

	else {

		log_warning(gameCard_logger,"aca porque pokemon en memoria es null");

		pokemonEnMemoria=string_new();

		char* aux = string_new();

		aux =(char*)mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
				fdBloq, 0);

		string_append(&pokemonEnMemoria, aux);

	}

	if (pokemonEnMemoria == NULL) {

		log_error(gameCard_logger, "se produjo un error al cargar en memoria");
	}
}

void llenarListaBloquesPoke(char* poke) {

	char* rutaMetadataPokemon = string_new();

	string_append(&rutaMetadataPokemon, rutas_fs->pathDirectorioFilesMetadata);
	string_append(&rutaMetadataPokemon, "/");
	string_append(&rutaMetadataPokemon, poke);
	string_append(&rutaMetadataPokemon, "/Metadata.bin");

	t_config* metadataPokemon = config_create(rutaMetadataPokemon);

	pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

	int tamanioArch = config_get_int_value(metadataPokemon, "SIZE");

	int cantBloq = tamanioArch / metadata_fs->tamanioBLoques;

	if (tamanioArch % metadata_fs->tamanioBLoques != 0) {
		cantBloq++;
	}

	char** listaBloques = config_get_array_value(metadataPokemon, "BLOCKS");

	for (int j = 0; j < cantBloq; j++) {

		list_add(bloquesMetadataPokemon, listaBloques[j]);
		log_info(gameCard_logger, "%s", listaBloques[j]);
	}

	pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

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

		bloqEnMemo = string_new();

		bloqEnMemo = traerAmemoriaUltimoBloque(ultBloque);

		string_append(&nuevaPos, string_itoa(nuevaCant));

		string_append(&nuevaPos, "\n");

		int espacioOcupadoBloque = string_length(bloqEnMemo);

		int espacioEnBloque = metadata_fs->tamanioBLoques
				- espacioOcupadoBloque;

		int sizeMetadata = obtenerEspacioMetadata(pokemon);

		int espacioNuevaLinea = string_length(nuevaPos) + sizeMetadata;

		char* stringAcopiar = string_new();

		string_append(&stringAcopiar, bloqEnMemo);

		string_append(&stringAcopiar, nuevaPos);

		if (espacioEnBloque >= espacioNuevaLinea) {

			log_info(gameCard_logger,
					"el archivo tiene espacio y se copia la posicion en el último bloque que le fue asignado");

			agregarPosicionEnUltimoBLoque(ultBloque, stringAcopiar,
					espacioNuevaLinea);

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
						espacioNuevaLinea, ultBloque);

			} else {

				log_info(gameCard_logger,
						"hay bloques libres disponibles en el fs");
				agregarPosicionPokemonAbloquesNuevos(ultBloque, stringAcopiar,
						espacioOcupadoBloque, espacioEnBloque,
						espacioNuevaLinea);
			}
		}

	}

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

	return sizeMetadata;
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

	copiado = desde = 0;

	int tamanioMetadata;

	posAcopiar = string_new();

	char** posiciones = string_split(pokemonEnMemoria, "\n");

	string_iterate_lines(posiciones, agregarCantidadNuevaAposicion);

	int cantBloqNecesarios = cantBloquesNecesariosPara(posAcopiar);

	int cantBloqOcupados = cantBloquesNecesariosPara(pokemonEnMemoria);

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

		tamanioMetadata = obtenerEspacioMetadata(pokemon)
				+ string_length(posAcopiar);

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
		cambiarTamanioMetadata(pokemon, tamanioMetadata);
		modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);
		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

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

		tamanioMetadata = obtenerEspacioMetadata(pokemon)
				+ string_length(posAcopiar);

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

	string_append(&rutaBloque, rutas_fs->pathDirectorioBloques);
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, ultBloque);
	string_append(&rutaBloque, ".bin");

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

	return bloqueEnMemo;

}

void agregarPosicionEnUltimoBLoque(char* ultBloq, char* stringAcopiar,
		int espacioNuevaLinea) {

	log_info(gameCard_logger, "Se copia %s en el bloque: %s.bin", stringAcopiar,
			ultBloq);

	if (stringAcopiar == NULL) {

		log_error(gameCard_logger, "Error en los datos a copiar");
	}

	copiarEnBloque(ultBloq, stringAcopiar);

	cambiarTamanioMetadata(pokemon, espacioNuevaLinea);

	log_info(gameCard_logger, "Se agrego correctamente la posicion");

}

void agregarPosicionPokemonAbloquesNuevos(char* ultBloque, char* stringAcopiar,
		int espacioOcupadoBloque, int espacioEnBloque, int espacioNuevaLinea) {

	log_info(gameCard_logger, "volvimos al hasta aca llegaste");

	log_info(gameCard_logger,
			"ult Bloq: %s, a copiar: %s, espacio en bloqocupado:%d y espacio %d y espacio nueva linea %d",
			ultBloque, stringAcopiar, espacioOcupadoBloque, espacioEnBloque,
			espacioNuevaLinea);
	char* copiarEnUltBloque = string_new();

	string_append(&copiarEnUltBloque, bloqEnMemo);

	if ((espacioEnBloque == 0) & (!string_ends_with(copiarEnUltBloque, "\n"))) {

		log_info(gameCard_logger, "acaentraste a un ifs");
		log_info(gameCard_logger,
				"si puedes tu con Dios hablar preguntale si yo alguna vez te he dejado de adorar");
		string_append(&copiarEnUltBloque, "\n");

		espacioNuevaLinea = espacioNuevaLinea + 1;
	}

	string_append(&copiarEnUltBloque,
			string_substring(stringAcopiar, espacioOcupadoBloque,
					espacioEnBloque));

	log_info(gameCard_logger, "aca se va a copiar en bloque");
	copiarEnBloque(ultBloque, copiarEnUltBloque);

	int cantBloquesNecesarios = cantBloquesNecesariosPara(
			string_substring(stringAcopiar, espacioEnBloque,
					espacioNuevaLinea - espacioEnBloque));

	log_info(gameCard_logger, "cantidad de bloques que se necesitan: %d",
			cantBloquesNecesarios);

	bloquesNuevos = list_create();

	bloquesNuevos = obtenerBloquesNuevos(cantBloquesNecesarios);

	copiado = desde = 0;

	for (int i = 0; i < list_size(bloquesNuevos); i++) {

		copiado = 0;
		int aCopiar = string_length(
				string_substring(nuevaPos, espacioEnBloque,
						espacioNuevaLinea - espacioEnBloque));

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

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));
		cambiarTamanioMetadata(pokemon, espacioNuevaLinea);
		modificarBloquesMetadata(pokemon, bloquesMetadataPokemon);
		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

	}
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

	for(int i=0; i < list_size(listBloqPokemon);i++){

		log_warning(gameCard_logger,"aca mostrame pos en lista");
		char* bloq=string_new();
		bloq=list_get(listBloqPokemon,i);
		log_warning(gameCard_logger,"aca pos %s",bloq);

		copiarPokemonEnMemoria(list_get(listBloqPokemon,i));
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

	config_set_value(configPoke, "SIZE", string_itoa(tamanioNuevo));
	//string_itoa(cantidadNueva));

	config_save_in_file(configPoke, rutaPoke);

//	pthread_mutex_unlock(dictionary_get(semaforosPokemon,pokemon));

	log_info(gameCard_logger,
			"se ha modificado el size del archivo adecuadamente");

}

void obtenerCantBloques(char* bloque) {

	cantBloquesPoke = cantBloquesPoke + 1;
}

t_list* obtenerBloquesNuevos(int cantBloqNecesarios) {

	t_list* bloquesLibres;

	bloquesLibres = list_create();

	while (cantBloqNecesarios != 0) {

		int bloqLib = obtenerPrimerBloqueLibre();

		log_info(gameCard_logger, "se le asigna al pokemon el bloque %d",
				bloqLib);

		list_add(bloquesLibres, string_itoa(bloqLib));

		marcarBloqueOcupado(bloqLib);

		agregarBloqueParaMetadataArchivo(string_itoa(bloqLib));

		cantBloqNecesarios = cantBloqNecesarios - 1;
	}

	return bloquesLibres;
}

void persistirCambiosEnBloquesPropios(void* bloque) {

	int aCopiar = string_length(posAcopiar);

	log_warning(gameCard_logger,"aca entro en persistir en bloques propios");
	if ((aCopiar - copiado) < metadata_fs->tamanioBLoques) {

log_warning(gameCard_logger,"aCopiar %d,copiado %d, desde %d",aCopiar,copiado,desde);
		copiarEnBloque(bloque,
				string_substring(posAcopiar, desde, aCopiar - desde));
	}

	else {

		log_warning(gameCard_logger,"aca me dijiste que llegabas");

		log_warning(gameCard_logger,"aCopiar %d,copiado %d, desde %d",aCopiar,copiado,desde);

		copiarEnBloque(bloque,
				string_substring(posAcopiar, desde,
						metadata_fs->tamanioBLoques));

		desde = desde + metadata_fs->tamanioBLoques;

	}

log_warning(gameCard_logger,"aCopiar %d,copiado %d, desde %d",aCopiar,copiado,desde);
}

void agregarCantidadNuevaAposicion(char* posicion) {

	if (string_starts_with(posicion, nuevaPos) == 1) {

		tamanioNuevaPos = string_length(nuevaPos);

		log_info(gameCard_logger,
				"A la cantidad de la posicion %s se ve a sumar %d", posicion,
				nuevaCant);

		nuevaCant = nuevaCant
				+ atoi(string_substring_from(posicion, tamanioNuevaPos));

		log_info(gameCard_logger, "la nueva posicion es : %s %d", nuevaPos,
				nuevaCant);

		string_append(&posAcopiar, nuevaPos);
		string_append(&posAcopiar, string_itoa(nuevaCant));
		string_append(&posAcopiar, "\n");

	} else {

		string_append(&posAcopiar, posicion);
		string_append(&posAcopiar, "\n");
	}
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
		string_append(&lineaBloquesOcupados,
				list_get(bloquesMetadataPokemon, i));

		if (i != (tamanioMaxList - 1)) {

			string_append(&lineaBloquesOcupados, ",");

		}

	}

	list_clean(bloquesMetadataPokemon);
	string_append(&lineaBloquesOcupados, "]");

	//pthread_mutex_lock(dictionary_get(semaforosPokemon,pokemon));

	config_set_value(configPoke, "BLOCKS", lineaBloquesOcupados);

	config_save_in_file(configPoke, rutaPoke);

	//pthread_mutex_unlock(dictionary_get(semaforosPokemon,pokemon));

	log_info(gameCard_logger, "la nueva lista de bloques del archivo: %s",
			lineaBloquesOcupados);
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

	if (dictionary_get(semaforosPokemon, pokemon) == NULL) {
		log_error(gameCard_logger, "error con los semáforos de los pokemones");
	}

	else {
		log_info(gameCard_logger, "se va a activar el semaforo del pokemon");

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

		char* estadoArchivo = config_get_string_value(configPoke, "OPEN");

		log_info(gameCard_logger, "El estado del archivo es %s", estadoArchivo);

		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

		return string_equals_ignore_case(estadoArchivo, "Y");

	}

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

			log_info(gameCard_logger,
					"Se ha abierto el archivo del pokemon: %s", poke);
		}
	}

	void cerrarArchivo(char* poke) {

		log_info(gameCard_logger, "se va a cerrar el archivo del pokemon %s",
				poke);
		t_config* configPoke;

		char* rutaPoke = string_new();
		string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaPoke, "/");
		string_append(&rutaPoke, poke);
		string_append(&rutaPoke, "/Metadata.bin");

		configPoke = config_create(rutaPoke);

		pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

		config_set_value(configPoke, "OPEN", "N");

		config_save_in_file(configPoke, rutaPoke);

		pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

		log_info(gameCard_logger, "se ha cerrado el archivo del pokemon");
	}

	uint32_t capturarPokemon(t_catch_pokemon* pokeAatrapar) {

		log_info(gameCard_logger, "se intentará capturar un pokemon");
		pokemonEnMemoria = string_new();

		bloquesMetadataPokemon = list_create();

		nuevaPos = string_new();

		int posx = pokeAatrapar->coordenadas.posx;
		int posy = pokeAatrapar->coordenadas.posy;
		string_append(&nuevaPos, string_itoa(posx));
		string_append(&nuevaPos, "-");
		string_append(&nuevaPos, string_itoa(posy));
		string_append(&nuevaPos, "=");

		log_info(gameCard_logger, "la posicion a buscar es (%d,%d) "
				"del pokemon %s", posx, posy, pokeAatrapar->pokemon);

		pokemon = string_new();
		string_append(&pokemon, pokeAatrapar->pokemon);

		llenarListaBloquesPoke(pokemon);

		copiarPokemonAmemoria(bloquesMetadataPokemon);

		if (estaPosicionEnMemoria(pokemonEnMemoria, nuevaPos)) {

			log_info(gameCard_logger,
					"se validó que la posicion existe en el archivo");

			posAcopiar = string_new();

			int cantBloqOcupados = cantBloquesNecesariosPara(pokemonEnMemoria);

			char** posiciones = string_split(pokemonEnMemoria, "\n");

			string_iterate_lines(posiciones, capturarPokeEnPos);

			int cantBloqSinPosicion = cantBloquesNecesariosPara(posAcopiar);

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

				for (int i = 0; i < cantBloqAliberar; i++) {

					log_info(gameCard_logger, "");

					char* elem = string_new();

					string_append(&elem,
							list_get(bloquesMetadataPokemon,
									tamanioListBloques - 1 + i));

					marcarBloqueLibreBitmap(atoi(elem));

					limpiarBloque(elem);

					list_remove_and_destroy_element(bloquesMetadataPokemon,
							tamanioListBloques - 1 + i,
							list_get(bloquesMetadataPokemon,
									tamanioListBloques - 1 + i));

				}

				log_info(gameCard_logger,
						"mostrame size de lista metadata pokemon; %d",
						list_size(bloquesMetadataPokemon));

				log_info(gameCard_logger, "mostrame JJJJJposacopiar: %s",
						posAcopiar);

				for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

					char* elem = list_get(bloquesMetadataPokemon, i);
					if (elem != NULL) {
						log_info(gameCard_logger,
								"aca accedo a lista de bloques, pos: %d", i);
						persistirCambiosEnBloquesPropios(
								list_get(bloquesMetadataPokemon, i));
					}
				}

				pthread_mutex_lock(dictionary_get(semaforosPokemon, pokemon));

				crearMetadataArchPoke(pokemon, string_length(posAcopiar));

				pthread_mutex_unlock(dictionary_get(semaforosPokemon, pokemon));

			}

			cerrarArchivo(pokemon);

			sleep(tiempo_retardo_operacion);

			return OK;
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

			log_info(gameCard_logger,
					"Marcar posicion %d del bitmap como libre", i);

			marcarBloqueLibreBitmap(atoi(list_get(bloquesOcupados, i)));

			log_info(gameCard_logger, "La poscion del bitmap paso a: %d",
					bitarray_test_bit(bitarray, i));
		}

	}

	void marcarBloqueLibreBitmap(int numBloque) {

		pthread_mutex_lock(&semMutexBitmap);

		bitarray_clean_bit(bitarray, numBloque);

		msync(bitarray, sizeof(bitarray), MS_SYNC);

		pthread_mutex_unlock(&semMutexBitmap);

		log_info(gameCard_logger,
				"se ha marcado como libre en el bitmap el bloque %d",
				numBloque);

	}

	void capturarPokeEnPos(char* posicion) {

		log_info(gameCard_logger, "mostrame posicion : %s", posicion);

		if (string_starts_with(posicion, nuevaPos)) {

			log_info(gameCard_logger,
					"aca posicion coincide con elemento de nuevaPos");

			tamanioNuevaPos = string_length(nuevaPos);

			int cantEnPos = atoi(
					string_substring_from(posicion, tamanioNuevaPos));

			log_info(gameCard_logger, "aca pasame cantEnPos %d", cantEnPos);

			if (cantEnPos > 1) {

				log_info(gameCard_logger,
						"se decrementa en 1 unidad la cantidad del pokemon");
				log_info(gameCard_logger, "la posicion era: %s", posicion);

				cantEnPos = cantEnPos - 1;

				log_info(gameCard_logger, "aca mostrame cantidad: %d",
						cantEnPos);

				string_append(&posAcopiar, nuevaPos);
				string_append(&posAcopiar, string_itoa(cantEnPos));
				string_append(&posAcopiar, "\n");

				log_info(gameCard_logger, "la posicion es: %s", nuevaPos);

			}

			else {

				log_info(gameCard_logger, "se va a eliminar la posicion: %s",
						posicion);
			}
		}

		else {

			string_append(&posAcopiar, posicion);
			string_append(&posAcopiar, "\n");
		}
	}

	void vaciarBloques(t_list* bloquesMetadataPokemon) {

		for (int i = 0; i < list_size(bloquesMetadataPokemon); i++) {

			char* bloq = string_new();

			string_append(&bloq, "");

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

	}

	void eliminarMetadataPokemon(char* poke) {

		char* rutaPoke = string_new();
		string_append(&rutaPoke, rutas_fs->pathDirectorioFilesMetadata);
		string_append(&rutaPoke, "/");
		string_append(&rutaPoke, pokemon);
		string_append(&rutaPoke, "/Metadata.bin");

		int cerrateSiEstasAbierto = fclose(rutaPoke);

		if (cerrateSiEstasAbierto == 0 | cerrateSiEstasAbierto == EOF) {

			int estaBorrado = remove(rutaPoke);

			if (estaBorrado == 0) {
				log_info(gameCard_logger,
						" Se ha borrado la metadata del pokemon:%s", poke);

				char* directorioPoke = string_new();
				string_append(&directorioPoke,
						rutas_fs->pathDirectorioFilesMetadata);
				string_append(&directorioPoke, "/");
				string_append(&directorioPoke, pokemon);

				int seBorroDirectorio = rmdir(directorioPoke);

				if (seBorroDirectorio == 0) {

					log_info(gameCard_logger,
							"se ha borrado correctamente el pokemon: %s", poke);
				} else {
					log_error(gameCard_logger,
							"error al borrar el directorio: %s", poke);
				}

			}

		}

		else {
			log_error(gameCard_logger,
					"Error al intentar borrar metadata de %s", poke);
		}
	}

	t_list* obtenerPosicionesPokemon(char* pokemon) {

		pokemonEnMemoria = string_new();

		llenarListaBloquesPoke(pokemon);

		log_info(gameCard_logger, "CANT BLOQUES METADATA: %d",
				list_size(bloquesMetadataPokemon));

		copiarPokemonAmemoria(bloquesMetadataPokemon);

		log_info(gameCard_logger, "ACA poke en memoria %s", pokemonEnMemoria);

		char** posiciones = string_split(pokemonEnMemoria, "\n");

		string_iterate_lines(posiciones, agregarPosicionAlistaParaLocalized);

		log_info(gameCard_logger,
				" las posiciones y cantidades del pokemon %s son :", pokemon);
		for (int i = 0; i < list_size(pokemonesParaLocalized); i++) {

			log_info(gameCard_logger, "%s \n",
					list_get(pokemonesParaLocalized, i));
		}

		sleep(tiempo_retardo_operacion);
		cerrarArchivo(pokemon);

		return pokemonesParaLocalized;

	}

	void agregarPosicionAlistaParaLocalized(char* posicion) {

		pokemonesParaLocalized = list_create();

		list_add(pokemonesParaLocalized, posicion);
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
