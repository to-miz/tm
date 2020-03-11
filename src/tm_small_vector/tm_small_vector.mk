unmerged.tm_small_vector.out := ${build_dir}unmerged.tm_small_vector${exe_ext}

# ${unmerged.tm_small_vector.out}: private override BUILD := release
${unmerged.tm_small_vector.out}: private options.cl.exception := -EHs
${unmerged.tm_small_vector.out}: src/tm_small_vector/*.cpp
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_small_vector/test.cpp, $@, src/tm_small_vector .)

unmerged.tm_small_vector: ${unmerged.tm_small_vector.out};

tm_small_vector.h: ${merge.out} ${unmerged.tm_small_vector.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_small_vector/main.cpp $@ src/tm_small_vector -r

merge.tm_small_vector: tm_small_vector.h;

merge.all: tm_small_vector.h