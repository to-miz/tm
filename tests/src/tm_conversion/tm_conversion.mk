tests.tm_conversion.out  := ${build_dir}tests.tm_conversion${exe_ext}
tests.tm_conversion.c.out := ${build_dir}tests.tm_conversion.c${exe_ext}

${tests.tm_conversion.out}: tests/src/tm_conversion/main.cpp tm_conversion.h
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_conversion/main.cpp, $@)

tests.tm_conversion: ${tests.tm_conversion.out} ${tests.tm_conversion.c.out}

tests.tm_conversion.out.fast := ${tests.tm_conversion.out} \
                                --test-case-exclude="Test float roundtrip,Test float accuracy"
ifeq (${os},windows)
	# cmd doesn't seem to like forward slashes on commands with arguments
	tests.tm_conversion.out.fast := $(subst /,\,${tests.tm_conversion.out.fast})
endif

tests.tm_conversion.run: ${tests.tm_conversion.out} ${tests.tm_conversion.c.out}
	${hide}echo Running Tests: $@.
	${hide}${tests.tm_conversion.out.fast}

tests.tm_conversion.slow.run: ${tests.tm_conversion.out} ${tests.tm_conversion.c.out}
	${hide}echo Running Tests: $@.
	${hide}${tests.tm_conversion.out}

# C compilation test.

${tests.tm_conversion.c.out}: tests/src/tm_conversion/main.c tm_conversion.h
	${hide}$(call c_compile_and_link, tests/src/tm_conversion/main.c, $@)
