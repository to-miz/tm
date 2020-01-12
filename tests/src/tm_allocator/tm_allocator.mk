tests.tm_allocator.deps :=  ${unmerged.tm_allocator}
tests.tm_allocator.deps += tests/src/tm_allocator/*.cpp tm_allocator.h

tests.tm_allocator.default.out := ${build_dir}tests.tm_allocator.default${exe_ext}
tests.tm_allocator.signed_size_t.out := ${build_dir}tests.tm_allocator.signed_size_t${exe_ext}

tests.tm_allocator.all_config_deps := ${tests.tm_allocator.default.out} \
                                       ${tests.tm_allocator.signed_size_t.out}

${tests.tm_allocator.default.out}: ${tests.tm_allocator.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_allocator/main.cpp, $@)

${tests.tm_allocator.signed_size_t.out}: ${tests.tm_allocator.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_allocator/main.cpp, $@,, USE_SIGNED_SIZE_T)

tests.tm_allocator: ${tests.tm_allocator.all_config_deps};

tests.tm_allocator.default.run: ${tests.tm_allocator.default.out}
	${hide}echo Running Test: ${tests.tm_allocator.default.out}.
	${hide}${tests.tm_allocator.default.out}

tests.tm_allocator.run: ${tests.tm_allocator.all_config_deps}
	${hide}echo Running Test: ${tests.tm_allocator.default.out}.
	${hide}${tests.tm_allocator.default.out}
	${hide}echo Running Test: ${tests.tm_allocator.signed_size_t.out}.
	${hide}${tests.tm_allocator.signed_size_t.out}

tests: tests.tm_allocator

tests.run: tests.tm_allocator.run