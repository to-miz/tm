unicode_gen_build ?= release

unicode_gen_build_dir := ${BUILD_DIR}${path_sep}${unicode_gen_build}${path_sep}

unicode_gen.data_dir := tools/unicode_gen/data/
unicode_gen.data := $(wildcard ${unicode_gen.data_dir}*)

include tools/unicode_gen/rules.mk

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
	${hide}${unicode_gen.out} dir tools/unicode_gen/data ${unicode_gen.options} \
		-o${unicode_gen_build_dir}test.cpp --header=${unicode_gen_build_dir}test.h

unicode_gen: ${unicode_gen.out}

unicode_gen.test: ${unicode_gen_build_dir}test.cpp ${unicode_gen_build_dir}test.h
