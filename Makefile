utils:= src/DirectedGraph.hpp src/utilities.hpp
out_dir := exec

default: ${out_dir}/sequential ${out_dir}/cilk ${out_dir}/cilk_cs ${out_dir}/cilk_race ${out_dir}/cilk_cs-bench

${out_dir}/sequential: src/main.cpp ${utils} | ${out_dir}
	g++ -Wl,-z,stack-size=67108864 src/main.cpp  -O3 -o $@

${out_dir}/cilk: src/cilk.cpp ${utils} | ${out_dir}
	/opt/opencilk/bin/clang++ -fopencilk -O3 src/cilk.cpp -o $@

${out_dir}/cilk_cs: src/cilk.cpp ${utils} | ${out_dir}
	/opt/opencilk/bin/clang++ -fopencilk -fcilktool=cilkscale -O3 src/cilk.cpp -o $@

${out_dir}/cilk_cs-bench: src/cilk.cpp ${utils} | ${out_dir}
	/opt/opencilk/bin/clang++ -fopencilk -fcilktool=cilkscale-benchmark -O3 src/cilk.cpp -o $@

${out_dir}/cilk_race: src/cilk.cpp ${utils} | ${out_dir}
	/opt/opencilk/bin/clang++ -fopencilk -fsanitize=cilk -Og -g src/cilk.cpp -o $@

${out_dir}:
	mkdir ${out_dir}