tests.tm_bin_packing.c.out := ${build_dir}tests.tm_bin_packing.c${exe_ext}
tests.tm_bin_packing.default.out := ${build_dir}tests.tm_bin_packing.default${exe_ext}

${tests.tm_bin_packing.c.out}: tests/src/tm_bin_packing/main.c tm_bin_packing.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_bin_packing/main.c, $@)

${tests.tm_bin_packing.default.out}: tests/src/tm_bin_packing/main.cpp tm_bin_packing.h
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_bin_packing/main.cpp, $@)

tests.tm_bin_packing.c: ${tests.tm_bin_packing.c.out}

tests.tm_bin_packing: ${tests.tm_bin_packing.c.out} ${tests.tm_bin_packing.default.out}

tests.tm_bin_packing.run: