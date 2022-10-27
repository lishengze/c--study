# !/bin/sh

cd build/
cmake ../
make -j4

rm -fr *.info
rm -fr build/report
rm -fr *.xml

cd ../

lcov -d build -z
lcov -d build -b . --no-external --initial -c -o testg.info

./build/testg


lcov -d build -b . --no-external -c -o testg.info
genhtml -o build/report testg.info