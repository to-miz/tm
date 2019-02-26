UNICODE_GEN_BUILD ?= release
# UNICODE_GEN_BUILD ?= debug

unicode_gen_build_dir := ${BUILD_DIR}/${UNICODE_GEN_BUILD}/

unicode_gen.out := ${unicode_gen_build_dir}unicode_gen${exe_ext}
unicode_gen.data_dir := tools/unicode_gen/data/
unicode_gen.data := $(wildcard ${unicode_gen.data_dir}*)

${unicode_gen.out}: override BUILD := ${UNICODE_GEN_BUILD}
${unicode_gen.out}: CPP_OPTIONS.gcc += -Wno-missing-field-initializers
${unicode_gen.out}: CPP_OPTIONS.gcc += -Wno-error=unused-parameter
${unicode_gen.out}: CPP_OPTIONS.clang += -Wno-missing-field-initializers
${unicode_gen.out}: CPP_OPTIONS.clang += -Wno-error=unused-parameter
${unicode_gen.out}: override cpp_options.cl := $(filter-out -EH%, ${cpp_options.cl}) -EHa
${unicode_gen.out}: tools/unicode_gen/src/*.cpp tm_cli.h
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, tools/unicode_gen/src/main.cpp, $@, .)

unicode_gen.flags    := case_info \
						category \
						grapheme_break \
						width \
						full_case_fold \
						simple_case_fold \
						prune_stage_one \
						prune_stage_two \
						handle_invalid_codepoints
unicode_gen.options := --prefix=tmu_ \
--assert=TM_ASSERT \
--flags=$(subst ${space},${comma},${unicode_gen.flags})

${unicode_gen_build_dir}test.h ${unicode_gen_build_dir}test.cpp: ${unicode_gen.out}
	${hide}echo Generating test unicode files.
	${hide}${unicode_gen.out} dir tools/unicode_gen/data ${unicode_gen.options} -o${unicode_gen_build_dir}test.cpp --header=${unicode_gen_build_dir}test.h

unicode_gen: ${unicode_gen.out}

unicode_gen.test: ${unicode_gen_build_dir}test.cpp ${unicode_gen_build_dir}test.h
