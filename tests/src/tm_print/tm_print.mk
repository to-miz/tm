tests.tm_print.src  := tests/src/tm_print/main.cpp
tests.tm_print.deps := ${tests.tm_print.src} tm_print.h tm_conversion.h

tests.tm_print.default.out := ${build_dir}tests.tm_print.default${exe_ext}
tests.tm_print.crt.out := ${build_dir}tests.tm_print.crt${exe_ext}
tests.tm_print.crt_signed_size_t.out := ${build_dir}tests.tm_print.crt_signed_size_t${exe_ext}
tests.tm_print.tm_conversion.out := ${build_dir}tests.tm_print.tm_conversion${exe_ext}
tests.tm_print.tm_conversion_signed_size_t.out := ${build_dir}tests.tm_print.tm_conversion_signed_size_t${exe_ext}
tests.tm_print.charconv.out := ${build_dir}tests.tm_print.charconv${exe_ext}
tests.tm_print.charconv_signed_size_t.out := ${build_dir}tests.tm_print.charconv_signed_size_t${exe_ext}

tests.tm_print.all_configs_deps := ${tests.tm_print.default.out}
tests.tm_print.all_configs_deps += ${tests.tm_print.crt.out} ${tests.tm_print.crt_signed_size_t.out}
tests.tm_print.all_configs_deps += ${tests.tm_print.tm_conversion.out}
tests.tm_print.all_configs_deps += ${tests.tm_print.tm_conversion_signed_size_t.out}
tests.tm_print.all_configs_deps += ${tests.tm_print.charconv.out} ${tests.tm_print.charconv_signed_size_t.out}

${tests.tm_print.default.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@)

${tests.tm_print.crt.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_CRT TMP_FLOAT_BACKEND_CRT)

${tests.tm_print.crt_signed_size_t.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_CRT TMP_FLOAT_BACKEND_CRT SIGNED_SIZE_T)

${tests.tm_print.tm_conversion.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_TM_CONVERSION TMP_FLOAT_BACKEND_TM_CONVERSION)

${tests.tm_print.tm_conversion_signed_size_t.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_TM_CONVERSION TMP_FLOAT_BACKEND_TM_CONVERSION SIGNED_SIZE_T)

ifeq (${HAS_CHARCONV_FLOAT},true)
tm_print.charconv_float := TMP_FLOAT_BACKEND_CHARCONV
else
tm_print.charconv_without_float := ${space}without float backend
endif

${tests.tm_print.charconv.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_CHARCONV ${tm_print.charconv_float})

${tests.tm_print.charconv_signed_size_t.out}: ${tests.tm_print.deps}
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, ${tests.tm_print.src}, $@,, \
	TMP_INT_BACKEND_CHARCONV ${tm_print.charconv_float} SIGNED_SIZE_T)

tests.tm_print: ${tests.tm_print.all_configs_deps}

tests.tm_print.default.run: ${tests.tm_print.default.out}
	${hide}echo TESTING: default config
	${hide}${tests.tm_print.default.out}

tests.tm_print.run: tests.tm_print
	${hide}echo ---
	${hide}echo TESTING: default config
	${hide}${tests.tm_print.default.out}
	${hide}echo ---
	${hide}echo TESTING: crt backend
	${hide}${tests.tm_print.crt.out}
	${hide}echo ---
	${hide}echo TESTING: crt backend with signed size_t
	${hide}${tests.tm_print.crt_signed_size_t.out}
	${hide}echo ---
	${hide}echo TESTING: tm_conversion backend
	${hide}${tests.tm_print.tm_conversion.out}
	${hide}echo ---
	${hide}echo TESTING: tm_conversion backend with signed size_t
	${hide}${tests.tm_print.tm_conversion_signed_size_t.out}
	${hide}echo ---
	${hide}echo TESTING: charconv backend${tm_print.charconv_without_float}
	${hide}${tests.tm_print.charconv.out}
	${hide}echo ---
	${hide}echo TESTING: charconv backend${tm_print.charconv_without_float} with signed size_t
	${hide}${tests.tm_print.charconv_signed_size_t.out}