tests.tm_uint128.deps := tm_uint128.h \
						 tests/src/tm_uint128/main.cpp \
						 tests/src/tm_uint128/generated_tests.cpp \
						 tests/src/tm_uint128/test_gen.py
tests.tm_uint128.default.out := ${build_dir}tests.tm_uint128.default${exe_ext}

tests/src/tm_uint128/generated_tests.cpp: tests/src/tm_uint128/test_gen.py
	${hide}echo Generating $@.
	${hide}python $< > $@

# Default
${tests.tm_uint128.default.out}: ${tests.tm_uint128.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_uint128/main.cpp, $@,, TMI_BACKEND_UINT64)

tests.tm_uint128.default: ${tests.tm_uint128.default.out}

tests.tm_uint128.default.run: ${tests.tm_uint128.default.out}
	${hide}echo TESTING: tm_uint128 default config
	${hide}${tests.tm_uint128.default.out}

tests.tm_uint128.run: tests.tm_uint128.default.run

# Intrinsics
tests.tm_uint128.intrinsics.out := ${build_dir}tests.tm_uint128.intrinsics${exe_ext}

${tests.tm_uint128.intrinsics.out}: ${tests.tm_uint128.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_uint128/main.cpp, $@,, TMI_BACKEND_UINT64 TMI_USE_INTRINSICS)

tests.tm_uint128.intrinsics: ${tests.tm_uint128.intrinsics.out}

tests.tm_uint128.intrinsics.run: ${tests.tm_uint128.intrinsics.out}
	${hide}echo TESTING: tm_uint128 default config
	${hide}${tests.tm_uint128.intrinsics.out}

tests.tm_uint128.run: tests.tm_uint128.intrinsics.run

# Gcc __int128
tests.tm_uint128.gcc_int128.out := ${build_dir}tests.tm_uint128.gcc_int128${exe_ext}

${tests.tm_uint128.gcc_int128.out}: ${tests.tm_uint128.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_uint128/main.cpp, $@,, TMI_BACKEND_GCC_UINT128)

tests.tm_uint128.gcc_int128: ${tests.tm_uint128.gcc_int128.out}

tests.tm_uint128.gcc_int128.run: ${tests.tm_uint128.gcc_int128.out}
	${hide}echo TESTING: tm_uint128 default config
	${hide}${tests.tm_uint128.gcc_int128.out}

tests.tm_uint128.run: tests.tm_uint128.gcc_int128.run