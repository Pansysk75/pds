opencilk_clang++ := /opt/opencilk/bin/clang++

out_dir := exec
src_files :=  src/scc_algorithms/tarjan.cpp src/scc_algorithms/testing.cpp
headers:= $(wildcard *.hpp)


default: ${out_dir}/main ${out_dir}/omp ${out_dir}/cilk  ${out_dir}/cilk_cs ${out_dir}/cilk_cs-bench ${out_dir}/cilk_race ${out_dir}/pthreads

${out_dir}/main: src/main.cpp src/scc_algorithms/coloring.cpp ${headers} | ${out_dir}
	g++ -O3 src/main.cpp src/scc_algorithms/coloring.cpp -o $@

${out_dir}/cilk: src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -O3 src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/cilk_cs: src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fcilktool=cilkscale -O3 src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/cilk_cs-bench: src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fcilktool=cilkscale-benchmark -O3 src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/cilk_race: src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp  ${headers} | ${out_dir}
	${opencilk_clang++} -fopencilk -fsanitize=cilk -Og -g src/cilk.cpp src/scc_algorithms/coloring_cilk.cpp -o $@

${out_dir}/omp: src/omp.cpp src/scc_algorithms/coloring_omp.cpp ${headers} | ${out_dir}
	g++ -fopenmp -O3 src/omp.cpp src/scc_algorithms/coloring_omp.cpp -o $@

${out_dir}/pthreads: src/pthreads.cpp src/scc_algorithms/coloring_pthreads.cpp ${headers} | ${out_dir}
	g++ -O3 src/pthreads.cpp src/scc_algorithms/coloring_pthreads.cpp -o $@

# ${out_dir}:
# 	mkdir ${out_dir}