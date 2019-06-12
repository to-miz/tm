tests.tm_json.deps := tm_json.h tests/src/tm_json/main.cpp ${unmerged.tm_json.out} ${unmerged.tm_json.c.out}

tests.tm_json.c.out := ${build_dir}tests.tm_json.c${exe_ext}
tests.tm_json.default.out := ${build_dir}tests.tm_json.default${exe_ext}
tests.tm_json.tm_conversion.out := ${build_dir}tests.tm_json.tm_conversion${exe_ext}
tests.tm_json.string_view.out := ${build_dir}tests.tm_json.string_view${exe_ext}
tests.tm_json.charconv.out := ${build_dir}tests.tm_json.charconv${exe_ext}
tests.tm_json.signed_size_t.out := ${build_dir}tests.tm_json.signed_size_t${exe_ext}

tests.tm_json.all_config_deps := ${tests.tm_json.default.out} ${tests.tm_json.tm_conversion.out}
tests.tm_json.all_config_deps += ${tests.tm_json.c.out}
tests.tm_json.all_config_deps += ${tests.tm_json.string_view.out}
tests.tm_json.all_config_deps += ${tests.tm_json.signed_size_t.out}

tests.tm_json.signed_size_t.defines := USE_STRING_VIEW SIGNED_SIZE_T

ifeq ("${HAS_CHARCONV}", "true")
tests.tm_json.all_config_deps += ${tests.tm_json.charconv.out}
tests.tm_json.signed_size_t.defines += USE_CHARCONV
tests.tm_json.run:
	${hide}echo Running Test: ${tests.tm_json.charconv.out}.
	${hide}${tests.tm_json.charconv.out}
endif

${tests.tm_json.c.out}: ${tests.tm_json.deps}
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_json/main.c, $@)

${tests.tm_json.default.out}: ${tests.tm_json.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_json/main.cpp, $@)

${tests.tm_json.tm_conversion.out}: ${tests.tm_json.deps} tm_conversion.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_json/main.cpp, $@,,USE_TM_CONVERSION)

${tests.tm_json.string_view.out}: ${tests.tm_json.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_json/main.cpp, $@,,USE_STRING_VIEW)

${tests.tm_json.charconv.out}: ${tests.tm_json.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_json/main.cpp, $@,,USE_CHARCONV)

${tests.tm_json.signed_size_t.out}: ${tests.tm_json.deps}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tests/src/tm_json/main.cpp, $@,,${tests.tm_json.signed_size_t.defines})

tests.tm_json: ${tests.tm_json.all_config_deps}

tests.tm_json.run: ${tests.tm_json.all_config_deps}
	${hide}echo Running Test: ${tests.tm_json.default.out}.
	${hide}${tests.tm_json.default.out}
	${hide}echo Running Test: ${tests.tm_json.tm_conversion.out}.
	${hide}${tests.tm_json.tm_conversion.out}
	${hide}echo Running Test: ${tests.tm_json.string_view.out}.
	${hide}${tests.tm_json.string_view.out}
	${hide}echo Running Test: ${tests.tm_json.signed_size_t.out}.
	${hide}${tests.tm_json.signed_size_t.out}

tests.tm_json.default: ${tests.tm_json.default.out}

tests.tm_json.signed_size_t: ${tests.tm_json.signed_size_t.out}

tests.tm_json.tm_conversion: ${tests.tm_json.tm_conversion.out}

tests.tm_json.default.run: ${tests.tm_json.default.out}
	${hide}echo Running Test: ${tests.tm_json.default.out}.
	${hide}${tests.tm_json.default.out}