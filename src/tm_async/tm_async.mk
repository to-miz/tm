unmerged.tm_async.out := ${build_dir}unmerged.tm_async${exe_ext}
unmerged.tm_async.c.out := ${build_dir}unmerged.tm_async.c${exe_ext}

# ${unmerged.tm_async.out}: private override BUILD := release
${unmerged.tm_async.out}: private options.cl.exception := -EHs
${unmerged.tm_async.out}: src/tm_async/*.cpp src/tm_async/*.c src/tm_async/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_async/test.cpp, $@, src/tm_async .)

${unmerged.tm_async.c.out}: C_OPTIONS.clang += -Wno-newline-eof
${unmerged.tm_async.c.out}: src/tm_async/*.cpp src/tm_async/*.c src/tm_async/*.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_async/test.c, $@, src/tm_async .)

unmerged.tm_async: ${unmerged.tm_async.out}

unmerged.tm_async.c: ${unmerged.tm_async.c.out}

tm_async.h: ${merge.out} ${unmerged.tm_async.out} ${unmerged.tm_async.c.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_async/main.cpp $@ src/tm_async -r

merge.tm_async: tm_async.h