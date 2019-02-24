unmerged.tm_cli.out := ${build_dir}unmerged.tm_cli${exe_ext}
unmerged.tm_cli.c.out := ${build_dir}unmerged.tm_cli.c${exe_ext}

${unmerged.tm_cli.out}: src/tm_cli/*.cpp
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, src/tm_cli/test.cpp, $@, src/tm_cli .)

${unmerged.tm_cli.c.out}: C_OPTIONS.clang += -Wno-newline-eof
${unmerged.tm_cli.c.out}: src/tm_cli/*.cpp src/tm_cli/*.c
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_cli/test.c, $@, src/tm_cli .)

unmerged.tm_cli: ${unmerged.tm_cli.out}

unmerged.tm_cli.c: ${unmerged.tm_cli.c.out}

tm_cli.h: ${merge.out} ${unmerged.tm_cli.out} ${unmerged.tm_cli.c.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_cli/main.cpp $@ src/tm_cli -r

merge.tm_cli: tm_cli.h