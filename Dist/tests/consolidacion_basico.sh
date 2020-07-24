../bin/gameboy BROKER CAUGHT_POKEMON 1 OK
../bin/gameboy BROKER CAUGHT_POKEMON 2 FAIL

../bin/gameboy BROKER CATCH_POKEMON Pikachu 2 3
../bin/gameboy BROKER CATCH_POKEMON Squirtle 5 2

../bin/gameboy BROKER CATCH_POKEMON Onyx 4 5

valgrind ../bin/gameboy SUSCRIPTOR CAUGHT_POKEMON 10

../bin/gameboy BROKER CATCH_POKEMON Charmander 4 5