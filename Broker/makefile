Broker : Broker.c Contexto_broker.c Herramientas_broker.c Memoria.c
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-eevee/Pokebola/Debug
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Contexto_broker.d" -MT"./Contexto_broker.o" -o "./Debug/Contexto_broker.o" "./Contexto_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Memoria.d" -MT"Memoria.o" -o "./Debug/Memoria.o" "./Memoria.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Herramientas_broker.d" -MT"./Herramientas_broker.o" -o "./Debug/Herramientas_broker.o" "./Herramientas_broker.c"
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Broker.d" -MT"./Broker.o" -o "./Debug/Broker.o" "./Broker.c"
	gcc -L"/home/utnso/tp-2020-1c-eevee/Pokebola/Debug" -o "./Dist/Broker" ./Debug/Memoria.o  ./Debug/Contexto_broker.o ./Debug/Herramientas_broker.o ./Debug/Broker.o   -lPokebola -lcommons

clean :
	rm ./Dist/*
	rm ./Debug/*