# !/bin/bash

perf_dir="perf_data"
work_dir="build"
exe_name="perf_ana"

stack_type="dwarf" #fp,dwarf

cd ${work_dir}/

cmake ../

make

cd ..

perf record -a --call-graph ${stack_type} ./${work_dir}/${exe_name}


perf script -i ${work_dir}/perf.data  > ${perf_dir}/perf.script

stackcollapse-perf.pl ${perf_dir}/perf.script > ${perf_dir}/result.floded

flamegraph.pl ${perf_dir}/result.floded > ${perf_dir}/result_${stack_type}.svg