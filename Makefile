opencilk_clang++ := /opt/opencilk/bin/clang++

out_dir := exec
src_files := src/main.cpp src/scc_algorithms/tarjan.cpp src/scc_algorithms/testing.cpp
headers:= $(wildcard *.hpp)


default: ${out_dir}/main # ${out_dir}/cilk ${out_dir}/cilk_cs ${out_dir}/cilk_race ${out_dir}/cilk_cs-bench

${out_dir}/main: ${src_files} ${headers} | ${out_dir}
	${opencilk_clang++} -Wl,-z,stack-size=67108864 ${src_files}  -O3 -o $@

# ${out_dir}/cilk: src/cilk.cpp ${utils} | ${out_dir}
# 	${opencilk_clang++} -fopencilk -O3 src/cilk.cpp -o $@

# ${out_dir}/cilk_cs: src/cilk.cpp ${utils} | ${out_dir}
# 	${opencilk_clang++} -fopencilk -fcilktool=cilkscale -O3 src/cilk.cpp -o $@

# ${out_dir}/cilk_cs-bench: src/cilk.cpp ${utils} | ${out_dir}
# 	${opencilk_clang++} -fopencilk -fcilktool=cilkscale-benchmark -O3 src/cilk.cpp -o $@

# ${out_dir}/cilk_race: src/cilk.cpp ${utils} | ${out_dir}
# 	${opencilk_clang++} -fopencilk -fsanitize=cilk -Og -g src/cilk.cpp -o $@

# ${out_dir}:
# 	mkdir ${out_dir}