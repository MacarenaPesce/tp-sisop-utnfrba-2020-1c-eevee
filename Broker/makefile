Broker : Contexto_broker.c Herramientas_broker.c Memoria.c ColaMensajes.c Conexion.c Broker.c
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-eevee/Pokebola/Debug
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Contexto_broker.d" -MT"./Contexto_broker.o" -o "./Debug/Contexto_broker.o" "./Contexto_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Memoria.d" -MT"Memoria.o" -o "./Debug/Memoria.o" "./Memoria.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Buddy.d" -MT"Buddy.o" -o "./Debug/Buddy.o" "./Buddy.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/ColaMensajes.d" -MT"./ColaMensajes.o" -o "./Debug/ColaMensajes.o" "./ColaMensajes.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Conexion.d" -MT"Conexion.o" -o "./Debug/Conexion.o" "./Conexion.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Herramientas_broker.d" -MT"./Herramientas_broker.o" -o "./Debug/Herramientas_broker.o" "./Herramientas_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/utilsMemoria.d" -MT"./utilsMemoria.o" -o "./Debug/utilsMemoria.o" "./utilsMemoria.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Broker.d" -MT"./Broker.o" -o "./Debug/Broker.o" "./Broker.c"
	gcc -L"/home/utnso/tp-2020-1c-eevee/Pokebola/Debug" -o "./Dist/Broker" ./Debug/Memoria.o ./Debug/Buddy.o  ./Debug/Contexto_broker.o ./Debug/Herramientas_broker.o ./Debug/utilsMemoria.o ./Debug/ColaMensajes.o ./Debug/Conexion.o ./Debug/Broker.o   -lPokebola -lcommons -lpthread

Cliente : Cliente.c
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-eevee/Pokebola/Debug
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Cliente.d" -MT"./Cliente.o" -o "./Debug/Cliente.o" "./Cliente.c"
	gcc -L"/home/utnso/tp-2020-1c-eevee/Pokebola/Debug" -o "./Dist/Cliente"  ./Debug/Cliente.o   -lPokebola -lcommons


clean :
	rm ./Dist/*
	rm ./Debug/*


all:  Contexto_broker.c Herramientas_broker.c Memoria.c Cliente.c ColaMensajes.c Conexion.c Broker.c
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-eevee/Pokebola/Debug
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Contexto_broker.d" -MT"./Contexto_broker.o" -o "./Debug/Contexto_broker.o" "./Contexto_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Memoria.d" -MT"Memoria.o" -o "./Debug/Memoria.o" "./Memoria.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/ColaMensajes.d" -MT"./ColaMensajes.o" -o "./Debug/ColaMensajes.o" "./ColaMensajes.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Conexion.d" -MT"Conexion.o" -o "./Debug/Conexion.o" "./Conexion.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Herramientas_broker.d" -MT"./Herramientas_broker.o" -o "./Debug/Herramientas_broker.o" "./Herramientas_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/utils.d" -MT"./utils.o" -o "./Debug/utils.o" "./utils.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Broker.d" -MT"./Broker.o" -o "./Debug/Broker.o" "./Broker.c"
	gcc -L"/home/utnso/tp-2020-1c-eevee/Pokebola/Debug" -o "./Dist/Broker" ./Debug/Memoria.o  ./Debug/Contexto_broker.o ./Debug/Herramientas_broker.o ./Debug/ColaMensajes.o ./Debug/Conexion.o ./Debug/Broker.o   -lPokebola -lcommons -lpthread
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Cliente.d" -MT"./Cliente.o" -o "./Debug/Cliente.o" "./Cliente.c" 
	gcc -L"/home/utnso/tp-2020-1c-eevee/Pokebola/Debug" -o "./Dist/Cliente"  ./Debug/Cliente.o   -lPokebola -lcommons -lpthread

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-eevee/Pokebola/Debug