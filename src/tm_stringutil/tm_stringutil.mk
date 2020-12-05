unmerged.tm_stringutil.out := ${build_dir}unmerged.tm_stringutil${exe_ext}

${unmerged.tm_stringutil.out}: ${unmerged_deps} src/tm_stringutil/*.cpp
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_stringutil/test.cpp, $@, src/tm_stringutil)

unmerged.tm_stringutil: ${unmerged.tm_stringutil.out}

tm_stringutil.h: ${merge.out} ${unmerged.tm_stringutil.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_stringutil/main.cpp $@ src/tm_stringutil -r

merge.all: tm_stringutil.h