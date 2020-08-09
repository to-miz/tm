# Configurable summodule makefile for use with platform.mk.

# Configurable variables.
unicode_gen_build ?= ${BUILD}
unicode_gen_src ?= tools/unicode_gen/src/
unicode_gen_tm_root ?= .

# Executable
unicode_gen.out := ${build_dir_root}${unicode_gen_build}${path_sep}unicode_gen${exe_ext}

${unicode_gen.out}: private override BUILD := ${unicode_gen_build}
${unicode_gen.out}: private CXX_OPTIONS.gcc += -Wno-missing-field-initializers
${unicode_gen.out}: private CXX_OPTIONS.gcc += -Wno-error=unused-parameter
ifeq (${os}${ARCH},windows64)
${unicode_gen.out}: private CXX_OPTIONS.gcc += -municode
endif
${unicode_gen.out}: private CXX_OPTIONS.clang += -Wno-missing-field-initializers
${unicode_gen.out}: private CXX_OPTIONS.clang += -Wno-error=unused-parameter
${unicode_gen.out}: private options.cl.exception := -EHa
${unicode_gen.out}: ${unicode_gen_src}*.cpp ${unicode_gen_tm_root}${path_sep}tm_cli.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, ${unicode_gen_src}main.cpp, $@, ${unicode_gen_tm_root})