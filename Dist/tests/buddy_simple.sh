../bin/gameboy BROKER CAUGHT_POKEMON 1 OK
sleep 1
../bin/gameboy BROKER CAUGHT_POKEMON 2 FAIL
sleep 1
../bin/gameboy BROKER NEW_POKEMON Pikachu 2 3 1
sleep 1
../bin/gameboy BROKER CATCH_POKEMON Onyx 4 5
sleep 1
../bin/gameboy SUSCRIPTOR NEW_POKEMON 2

../bin/gameboy BROKER CATCH_POKEMON Charmander 4 5