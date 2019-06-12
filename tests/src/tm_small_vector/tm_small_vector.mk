tests.tm_small_vector.deps :=  ${unmerged.tm_small_vector}
tests.tm_small_vector.deps += tests/src/tm_small_vector/*.cpp tm_small_vector.h

tests.tm_small_vector.default.out := ${build_dir}tests.tm_small_vector.default${exe_ext}
tests.tm_small_vector.signed_size_t.out := ${build_dir}tests.tm_small_vector.signed_size_t${exe_ext}

tests.tm_small_vector.all_config_deps := ${tests.tm_small_vector.default.out} \
                                       ${tests.tm_small_vector.signed_size_t.out}

${tests.tm_small_vector.default.out}: ${tests.tm_small_vector.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_small_vector/main.cpp, $@)

${tests.tm_small_vector.signed_size_t.out}: ${tests.tm_small_vector.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_small_vector/main.cpp, $@,, USE_SIGNED_SIZE_T)

tests.tm_small_vector: ${tests.tm_small_vector.all_config_deps};

tests.tm_small_vector.default.run: ${tests.tm_small_vector.default.out}
	${hide}echo Running Test: ${tests.tm_small_vector.default.out}.
	${hide}${tests.tm_small_vector.default.out}

tests.tm_small_vector.run: ${tests.tm_small_vector.all_config_deps}
	${hide}echo Running Test: ${tests.tm_small_vector.default.out}.
	${hide}${tests.tm_small_vector.default.out}
	${hide}echo Running Test: ${tests.tm_small_vector.signed_size_t.out}.
	${hide}${tests.tm_small_vector.signed_size_t.out}

tests: test.tm_small_vector

tests.run: tests.tm_small_vector.run