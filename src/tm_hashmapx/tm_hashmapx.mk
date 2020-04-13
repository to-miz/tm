unmerged.tm_hashmapx.out := ${build_dir}unmerged.tm_hashmapx${exe_ext}
${unmerged.tm_hashmapx.out}: src/tm_hashmapx/*.cpp src/tm_hashmapx/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_hashmapx/test.cpp, $@, src/tm_hashmapx .)

unmerged.tm_hashmapx: ${unmerged.tm_hashmapx.out}

unmerged.tm_hashmapx.c.out := ${build_dir}unmerged.tm_hashmapx.c${exe_ext}
${unmerged.tm_hashmapx.c.out}: src/tm_hashmapx/*.cpp src/tm_hashmapx/test.c src/tm_hashmapx/*.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_hashmapx/test.c, $@, src/tm_hashmapx .)

unmerged.tm_hashmapx.c: ${unmerged.tm_hashmapx.c.out}

tm_hashmapx.h: ${unmerged.tm_hashmapx.out} ${merge.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_hashmapx/main.cpp $@ src/tm_hashmapx -r

merge.tm_hashmapx: tm_hashmapx.h