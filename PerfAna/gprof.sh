# !/bin/bash

cd build/

cmake ../

make

./perf_ana

cd ..

gprof ./build/perf_ana ./build/gmon.out -b >gprof_report.txt

gprof  ./build/perf_ana ./build/gmon.out |gprof2dot |dot -Tpng -o gprof_show.png