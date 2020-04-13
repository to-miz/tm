tests.tm_hashmapx.deps := tm_hashmapx.h tests/src/tm_hashmapx/*.cpp ${unmerged.tm_hashmapx.out} ${unmerged.tm_hashmapx.c.out}

tests.tm_hashmapx.c.out := ${build_dir}tests.tm_hashmapx.c${exe_ext}
tests.tm_hashmapx.default.out := ${build_dir}tests.tm_hashmapx.default${exe_ext}
tests.tm_hashmapx.signed_size_t.out := ${build_dir}tests.tm_hashmapx.signed_size_t${exe_ext}

tests.tm_hashmapx.all_config_deps := ${tests.tm_hashmapx.default.out}
tests.tm_hashmapx.all_config_deps += ${tests.tm_hashmapx.c.out}
tests.tm_hashmapx.all_config_deps += ${tests.tm_hashmapx.signed_size_t.out}

tests.tm_hashmapx.signed_size_t.defines := SIGNED_SIZE_T

${tests.tm_hashmapx.c.out}: ${tests.tm_hashmapx.deps}
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_hashmapx/main.c, $@)

${tests.tm_hashmapx.default.out}: ${tests.tm_hashmapx.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_hashmapx/main.cpp, $@)

${tests.tm_hashmapx.signed_size_t.out}: ${tests.tm_hashmapx.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_hashmapx/main.cpp, $@,,${tests.tm_hashmapx.signed_size_t.defines})

tests.tm_hashmapx: ${tests.tm_hashmapx.all_config_deps}

tests.tm_hashmapx.run: ${tests.tm_hashmapx.all_config_deps}
	${hide}echo Running Test: ${tests.tm_hashmapx.default.out}.
	${hide}${tests.tm_hashmapx.default.out}
	${hide}echo Running Test: ${tests.tm_hashmapx.signed_size_t.out}.
	${hide}${tests.tm_hashmapx.signed_size_t.out}

tests.tm_hashmapx.default: ${tests.tm_hashmapx.default.out}

tests.tm_hashmapx.signed_size_t: ${tests.tm_hashmapx.signed_size_t.out}

tests.tm_hashmapx.default.run: ${tests.tm_hashmapx.default.out}
	${hide}echo Running Test: ${tests.tm_hashmapx.default.out}.
	${hide}${tests.tm_hashmapx.default.out}

tests: tests.tm_hashmapx

tests.run: tests.tm_hashmapx.run