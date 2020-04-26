Broker : Broker.c Contexto_broker.c Herramientas_broker.c
	gcc -I"/home/utnso/workspace/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"Contexto_broker.d" -MT"Contexto_broker.o" -o "Contexto_broker.o" "./Contexto_broker.c"
	gcc -I"/home/utnso/workspace/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"Herramientas_broker.d" -MT"Herramientas_broker.o" -o "Herramientas_broker.o" "./Herramientas_broker.c"
	gcc -I"/home/utnso/workspace/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"Broker.d" -MT"Broker.o" -o "Broker.o" "./Broker.c"
	gcc -L"/home/utnso/workspace/tp-2020-1c-eevee/Pokebola/Debug" -o "Broker"  ./Contexto_broker.o ./Herramientas_broker.o ./Broker.o   -lPokebola -lcommons
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/workspace/tp-2020-1c-eevee/Pokebola/Debug