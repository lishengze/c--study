# !bin/bash
rm -f ./obj/*.o
rm -f ./bin/*

make

printf "\n\nResult: \n"
./bin/main