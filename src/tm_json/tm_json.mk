unmerged.tm_json.out := ${build_dir}unmerged.tm_json${exe_ext}
unmerged.tm_json.c.out := ${build_dir}unmerged.tm_json.c${exe_ext}

${unmerged.tm_json.out}: src/tm_json/*.cpp
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, src/tm_json/test.cpp, $@, src/tm_json .)

${unmerged.tm_json.c.out}: src/tm_json/*.c
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_json/test.c, $@, src/tm_json .)

unmerged.tm_json: ${unmerged.tm_json.out}

unmerged.tm_json.c: ${unmerged.tm_json.c.out}

tm_json.h: src/tm_json/*.cpp
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_json/main.cpp $@ src/tm_json -r

merge.tm_json: tm_json.h