# !/bin/bash

gprof_dir="gprof_data"
work_dir="build"
exe_name="perf_ana"

cd ${work_dir}/

cmake ../

make

./${exe_name}

cd ..

gprof  ${work_dir}/${exe_name} ./${work_dir}/gmon.out -b > ${gprof_dir}/gprof_report.txt

gprof  ${work_dir}/${exe_name} ./${work_dir}/gmon.out |gprof2dot |dot -Tpng -o ${gprof_dir}/gprof_show.png