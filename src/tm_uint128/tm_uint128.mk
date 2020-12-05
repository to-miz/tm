unmerged.tm_uint128.out := ${build_dir}unmerged.tm_uint128${exe_ext}
unmerged.tm_uint128.c.out := ${build_dir}unmerged.tm_uint128.c${exe_ext}

${unmerged.tm_uint128.out}: private link_libs.gcc += -lm
${unmerged.tm_uint128.out}: ${unmerged_deps} src/tm_uint128/*.cpp src/tm_uint128/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_uint128/test.cpp, $@, src/tm_uint128 .)

${unmerged.tm_uint128.c.out}: private link_libs.gcc += -lm
${unmerged.tm_uint128.c.out}: ${unmerged_deps} src/tm_uint128/test.c src/tm_uint128/*.cpp src/tm_uint128/*.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_uint128/test.c, $@, src/tm_uint128 .)

unmerged.tm_uint128: ${unmerged.tm_uint128.out}
unmerged.tm_uint128.c: ${unmerged.tm_uint128.c.out}

tm_uint128.h: ${merge.out} ${unmerged.tm_uint128.out} ${unmerged.tm_uint128.c.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_uint128/main.cpp $@ src/tm_uint128 -r

merge.tm_uint128: tm_uint128.h

merge.all: tm_uint128.h