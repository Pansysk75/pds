opencilk_clang++ := /opt/opencilk/bin/clang++

out_dir := exec
src_files :=  src/scc_algorithms/tarjan.cpp src/scc_algorithms/testing.cpp
headers:= $(wildcard *.hpp)


default: ${out_dir}/sequential ${out_dir}/omp ${out_dir}/cilk   ${out_dir}/pthreads
all: default ${out_dir}/cilk_cs ${out_dir}/cilk_cs-bench ${out_dir}/cilk_race

${out_dir}/sequential: src/main.cpp src/scc_algorithms/coloring_sequential.cpp ${headers} | ${out_dir}
	g++ src/main.cpp src/scc_algorithms/coloring_sequential.cpp -O3 -o $@

${out_dir}/cilk: src/main.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} src/main.cpp src/scc_algorithms/coloring_cilk.cpp -O3 -o $@ -fopencilk 


${out_dir}/omp: src/main.cpp src/scc_algorithms/coloring_omp.cpp ${headers} | ${out_dir}
	g++ src/main.cpp src/scc_algorithms/coloring_omp.cpp -O3 -o $@  -fopenmp

${out_dir}/pthreads: src/main.cpp src/scc_algorithms/coloring_pthreads.cpp ${headers} | ${out_dir}
	g++ src/main.cpp src/scc_algorithms/coloring_pthreads.cpp -O3 -o $@


#Additional optional OpenCilk builds:

${out_dir}/cilk_cs: src/main.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fcilktool=cilkscale -O3 src/main.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/cilk_cs-bench: src/main.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fcilktool=cilkscale-benchmark -O3 src/main.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/cilk_race: src/main.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fsanitize=cilk -Og -g src/main.cpp src/scc_algorithms/coloring_cilk.cpp -o $@



${out_dir}:
	mkdir -p ${out_dir}