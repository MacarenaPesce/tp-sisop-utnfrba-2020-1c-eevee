gameboy : Comunicacion_con_broker.c Comunicacion_con_gamecard.c Comunicacion_con_team.c Contexto_gameboy.c Herramientas_gameboy.c GameBoy.c 
	mkdir -p ./Debug ./Dist
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_broker.d" -MT"./Comunicacion_con_broker.o" -o "./Debug/Comunicacion_con_broker.o" "./Comunicacion_con_broker.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_gamecard.d" -MT"Comunicacion_con_gamecard.o" -o "./Debug/Comunicacion_con_gamecard.o" "./Comunicacion_con_gamecard.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_team.d" -MT"Comunicacion_con_team.o" -o "./Debug/Comunicacion_con_team.o" "./Comunicacion_con_team.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Contexto_gameboy.d" -MT"./Contexto_gameboy.o" -o "./Debug/Contexto_gameboy.o" "./Contexto_gameboy.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Herramientas_gameboy.d" -MT"Herramientas_gameboy.o" -o "./Debug/Herramientas_gameboy.o" "./Herramientas_gameboy.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Team.d" -MT"./GameBoy.o" -o "./Debug/GameBoy.o" "./GameBoy.c"
	gcc -L"../Pokebola/Debug" -o "./Dist/gameboy" ./Debug/Comunicacion_con_broker.o ./Debug/Comunicacion_con_gamecard.o ./Debug/Comunicacion_con_team.o ./Debug/Contexto_gameboy.o  ./Debug/Herramientas_gameboy.o ./Debug/GameBoy.o -lPokebola -lcommons -lpthread

clean :
	rm -f ./Dist/gameboy ./Dist/gameboy.log ./Debug/*

all : Comunicacion_con_broker.c Comunicacion_con_gamecard.c Comunicacion_con_team.c Contexto_gameboy.c Herramientas_gameboy.c GameBoy.c 
	mkdir -p ./Debug ./Dist
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_broker.d" -MT"./Comunicacion_con_broker.o" -o "./Debug/Comunicacion_con_broker.o" "./Comunicacion_con_broker.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_gamecard.d" -MT"Comunicacion_con_gamecard.o" -o "./Debug/Comunicacion_con_gamecard.o" "./Comunicacion_con_gamecard.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Comunicacion_con_team.d" -MT"Comunicacion_con_team.o" -o "./Debug/Comunicacion_con_team.o" "./Comunicacion_con_team.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Contexto_gameboy.d" -MT"./Contexto_gameboy.o" -o "./Debug/Contexto_gameboy.o" "./Contexto_gameboy.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Herramientas_gameboy.d" -MT"Herramientas_gameboy.o" -o "./Debug/Herramientas_gameboy.o" "./Herramientas_gameboy.c"
	gcc -I"../Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Debug/Team.d" -MT"./GameBoy.o" -o "./Debug/GameBoy.o" "./GameBoy.c"
	gcc -L"../Pokebola/Debug" -o "./Dist/gameboy" ./Debug/Comunicacion_con_broker.o ./Debug/Comunicacion_con_gamecard.o ./Debug/Comunicacion_con_team.o ./Debug/Contexto_gameboy.o  ./Debug/Herramientas_gameboy.o ./Debug/GameBoy.o -lPokebola -lcommons -lpthread