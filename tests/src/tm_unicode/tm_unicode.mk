# Generated files:
tests.generated.tm_unicode.flags := case_info \
									category \
									grapheme_break \
									width \
									canonical \
									compatibility \
									full_case \
									full_case_fold \
									full_case_toggle \
									simple_case \
									simple_case_fold \
									simple_case_toggle \
									handle_invalid_codepoints
tests.generated.tm_unicode.options := --prefix=tmu_ \
--assert=TM_ASSERT \
--flags=$(subst ${space},${comma},${tests.generated.tm_unicode.flags})

tests.generated.tm_unicode   := tests/src/tm_unicode/generated/tests_unicode_data.h \
								tests/src/tm_unicode/generated/tests_unicode_data.c

${tests.generated.tm_unicode}: ${unicode_gen.out} ${unicode_gen.data}
	${hide}echo Generating tests_unicode_data.h and tests_unicode_data.c
	${hide}${unicode_gen.out} dir ${unicode_gen.data_dir} ${tests.generated.tm_unicode.options} \
		--no-header-guard --output=tests/src/tm_unicode/generated/tests_unicode_data.c \
		--header=tests/src/tm_unicode/generated/tests_unicode_data.h

tests.tm_unicode.deps := tm_unicode.h tests/src/tm_unicode/*.cpp tests/src/tm_unicode/*.c tests/src/tm_unicode/*.h ${tests.generated.tm_unicode}

tests.tm_unicode.default.c.out := ${build_dir}tests.tm_unicode.default.c${exe_ext}
tests.tm_unicode.windows.c.out := ${build_dir}tests.tm_unicode.windows.c${exe_ext}

tests.tm_unicode.default.out := ${build_dir}tests.tm_unicode.default${exe_ext}
tests.tm_unicode.signed_size_t.out := ${build_dir}tests.tm_unicode.signed_size_t${exe_ext}
tests.tm_unicode.msvc_crt.out := ${build_dir}tests.tm_unicode.msvc_crt${exe_ext}
tests.tm_unicode.msvc_crt.signed_size_t.out := ${build_dir}tests.tm_unicode.msvc_crt.signed_size_t${exe_ext}
tests.tm_unicode.windows.out := ${build_dir}tests.tm_unicode.windows${exe_ext}
tests.tm_unicode.windows.signed_size_t.out := ${build_dir}tests.tm_unicode.windows.signed_size_t${exe_ext}

tests.tm_unicode.all_config_deps := ${tests.tm_unicode.default.c.out} \
									${tests.tm_unicode.default.out} \
									${tests.tm_unicode.signed_size_t.out} \
									${tests.tm_unicode.msvc_crt.out} \
									${tests.tm_unicode.msvc_crt.signed_size_t.out} \
									${tests.tm_unicode.windows.out} \
									${tests.tm_unicode.windows.signed_size_t.out}

ifeq (${os},windows)
tests.tm_unicode.all_config_deps += ${tests.tm_unicode.windows.c.out}
endif

${tests.tm_unicode.default.c.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_unicode/main.c, $@, , USE_TESTS_UCD)

${tests.tm_unicode.windows.c.out}: LINK_LIBS.cl := Shell32.lib
${tests.tm_unicode.windows.c.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_unicode/main.c, $@,, USE_WINDOWS_H)

${tests.tm_unicode.default.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@)

${tests.tm_unicode.signed_size_t.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@,, USE_SIGNED_SIZE_T)

${tests.tm_unicode.msvc_crt.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@,, USE_MSVC_CRT)

${tests.tm_unicode.msvc_crt.signed_size_t.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@,, USE_MSVC_CRT USE_SIGNED_SIZE_T)

${tests.tm_unicode.windows.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@,, USE_WINDOWS_H)

${tests.tm_unicode.windows.signed_size_t.out}: ${tests.tm_unicode.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_unicode/main.cpp, $@,, USE_WINDOWS_H USE_SIGNED_SIZE_T)

tests.tm_unicode: ${tests.tm_unicode.all_config_deps}

tests.tm_unicode.c: ${tests.tm_unicode.default.c.out}

tests.tm_unicode.run: ${tests.tm_unicode.all_config_deps}
	${hide}echo Running Test: ${tests.tm_unicode.default.out}
	${hide}${tests.tm_unicode.default.out}
	${hide}echo Running Test: ${tests.tm_unicode.signed_size_t.out}
	${hide}${tests.tm_unicode.signed_size_t.out}
	${hide}echo Running Test: ${tests.tm_unicode.msvc_crt.out}
	${hide}${tests.tm_unicode.msvc_crt.out}
	${hide}echo Running Test: ${tests.tm_unicode.msvc_crt.signed_size_t.out}
	${hide}${tests.tm_unicode.msvc_crt.signed_size_t.out}
	${hide}echo Running Test: ${tests.tm_unicode.windows.out}
	${hide}${tests.tm_unicode.windows.out}
	${hide}echo Running Test: ${tests.tm_unicode.windows.signed_size_t.out}
	${hide}${tests.tm_unicode.windows.signed_size_t.out}

tests.tm_unicode.default.run: ${tests.tm_unicode.default.out}
	${hide}echo Running Test: ${tests.tm_unicode.default.out}
	${hide}${tests.tm_unicode.default.out}

tests.tm_unicode.signed_size_t.run: ${tests.tm_unicode.signed_size_t.out}
	${hide}echo Running Test: ${tests.tm_unicode.signed_size_t.out}
	${hide}${tests.tm_unicode.signed_size_t.out}

tests.tm_unicode.windows.run: ${tests.tm_unicode.windows.out}
	${hide}echo Running Test: ${tests.tm_unicode.windows.out}
	${hide}${tests.tm_unicode.windows.out}

tests.tm_unicode.msvc_crt.run: ${tests.tm_unicode.msvc_crt.out}
	${hide}echo Running Test: ${tests.tm_unicode.msvc_crt.out}
	${hide}${tests.tm_unicode.msvc_crt.out}