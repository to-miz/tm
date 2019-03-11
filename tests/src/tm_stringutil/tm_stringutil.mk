tests.tm_stringutil.deps :=  ${unmerged.tm_stringutil} ${unmerged.tm_stringutil.c}
tests.tm_stringutil.deps += tests/src/tm_stringutil/*.cpp tm_stringutil.h

tests.tm_stringutil.default.out := ${build_dir}tests.tm_stringutil.default${exe_ext}
tests.tm_stringutil.c.out := ${build_dir}tests.tm_stringutil.c${exe_ext}
tests.tm_stringutil.string_view.out := ${build_dir}tests.tm_stringutil.string_view${exe_ext}
tests.tm_stringutil.signed_size_t.out := ${build_dir}tests.tm_stringutil.signed_size_t${exe_ext}

tests.tm_stringutil.all_config_deps := ${tests.tm_stringutil.default.out} \
                                       ${tests.tm_stringutil.c.out} \
                                       ${tests.tm_stringutil.string_view.out} \
                                       ${tests.tm_stringutil.signed_size_t.out}

${tests.tm_stringutil.default.out}: ${tests.tm_stringutil.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_stringutil/main.cpp, $@)

${tests.tm_stringutil.c.out}: ${tests.tm_stringutil.deps}
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_stringutil/main.c, $@)

${tests.tm_stringutil.string_view.out}: ${tests.tm_stringutil.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_stringutil/main.cpp, $@,, USE_STRING_VIEW)

${tests.tm_stringutil.signed_size_t.out}: ${tests.tm_stringutil.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tests/src/tm_stringutil/main.cpp, $@,, USE_STRING_VIEW USE_SIGNED_SIZE_T)

tests.tm_stringutil: ${tests.tm_stringutil.all_config_deps}

tests.tm_stringutil.default.run: ${tests.tm_stringutil.default.out}
	${hide}echo Running Test: ${tests.tm_stringutil.default.out}.
	${hide}${tests.tm_stringutil.default.out}

tests.tm_stringutil.run: ${tests.tm_stringutil.all_config_deps}
	${hide}echo Running Test: ${tests.tm_stringutil.default.out}.
	${hide}${tests.tm_stringutil.default.out}
	${hide}echo Running Test: ${tests.tm_stringutil.string_view.out}.
	${hide}${tests.tm_stringutil.string_view.out}
	${hide}echo Running Test: ${tests.tm_stringutil.signed_size_t.out}.
	${hide}${tests.tm_stringutil.signed_size_t.out}
