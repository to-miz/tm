unmerged.tm_print.out := ${build_dir}unmerged.tm_print${exe_ext}
${unmerged.tm_print.out}: ${unmerged_deps} src/tm_print/*.cpp src/tm_print/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_print/test.cpp, $@, src/tm_print .)

unmerged.tm_print: ${unmerged.tm_print.out}

tm_print.h: ${unmerged.tm_print.out} ${merge.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_print/main.cpp $@ src/tm_print -r

merge.all: tm_print.h