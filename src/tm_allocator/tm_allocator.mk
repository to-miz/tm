unmerged.tm_allocator.out := ${build_dir}unmerged.tm_allocator${exe_ext}

# ${unmerged.tm_allocator.out}: private override BUILD := release
${unmerged.tm_allocator.out}: private options.cl.exception := -EHs
${unmerged.tm_allocator.out}: src/tm_allocator/*.cpp src/tm_allocator/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_allocator/test.cpp, $@, src/tm_allocator .)

unmerged.tm_allocator: ${unmerged.tm_allocator.out}

tm_allocator.h: ${merge.out} ${unmerged.tm_allocator.out} ${unmerged.tm_allocator.c.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_allocator/main.cpp $@ src/tm_allocator -r

merge.tm_allocator: tm_allocator.h

merge.all: tm_allocator.h