all:
	cd ./Pokebola/Debug && $(MAKE)
	cd ./Broker && $(MAKE) 	
	cd ./GameBoy && $(MAKE) 	
	cd ./GameCard && $(MAKE) 
	cd ./TEAM && $(MAKE)
	mkdir -p ./Dist/bin
	cp ./Broker/Dist/broker ./Dist/bin/broker
	cp ./GameBoy/Dist/gameboy ./Dist/bin/gameboy 
	cp ./GameCard/Dist/gamecard ./Dist/bin/gamecard 
	cp ./TEAM/Dist/team ./Dist/bin/team 

clean:
	cd ./Pokebola/Debug && $(MAKE) clean
	cd ./Broker && $(MAKE) clean
	cd ./GameBoy && $(MAKE) clean
	cd ./GameCard && $(MAKE) clean
	cd ./TEAM && $(MAKE) clean
	rm -Rf ./Dist/bin

cleanfs:

	rm -Rf /home/utnso/desktop/tall-grass/