# Generated files:
generated.tm_unicode.flags   := case_info \
								category \
								grapheme_break \
								full_case_fold \
								simple_case_fold \
								prune_stage_one \
								prune_stage_two \
								handle_invalid_codepoints
generated.tm_unicode.options := --prefix=tmu_ \
--assert=TM_ASSERT \
--flags=$(subst ${space},${comma},${generated.tm_unicode.flags})

generated.tm_unicode.unicode_data_h := src/tm_unicode/generated/unicode_data.h
generated.tm_unicode.unicode_data_c := src/tm_unicode/generated/unicode_data.c

generated.tm_unicode := ${generated.tm_unicode.unicode_data_h} ${generated.tm_unicode.unicode_data_c}

${generated.tm_unicode}: ${unicode_gen.out} ${unicode_gen.data}
	${hide}echo Generating unicode_data.h and unicode_data.c
	${hide}${unicode_gen.out} dir ${unicode_gen.data_dir} ${generated.tm_unicode.options} \
		--no-header-guard \
		--output=${generated.tm_unicode.unicode_data_c} --header=${generated.tm_unicode.unicode_data_h}

unmerged.tm_unicode.out := ${build_dir}unmerged.tm_unicode${exe_ext}
unmerged.tm_unicode.c.out := ${build_dir}unmerged.tm_unicode.c${exe_ext}

${unmerged.tm_unicode.out}: CXX_OPTIONS.gcc += -Wno-error=unused-function
${unmerged.tm_unicode.out}: CXX_OPTIONS.clang += -Wno-error=unused-function
${unmerged.tm_unicode.out}: src/tm_unicode/*.cpp src/tm_unicode/*.h ${generated.tm_unicode}
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_unicode/test.cpp, $@, src/tm_unicode .)

${unmerged.tm_unicode.c.out}: C_OPTIONS.clang += -Wno-newline-eof
${unmerged.tm_unicode.c.out}: src/tm_unicode/*.cpp src/tm_unicode/*.c
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_unicode/test.c, $@, src/tm_unicode .)

unmerged.tm_unicode: ${unmerged.tm_unicode.out}

unmerged.tm_unicode.c: ${unmerged.tm_unicode.c.out}

tm_unicode.h: ${generated.tm_unicode} ${merge.out} ${unmerged.tm_unicode.out} ${unmerged.tm_unicode.c.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_unicode/main.cpp $@ src/tm_unicode -r

merge.tm_unicode: tm_unicode.h
