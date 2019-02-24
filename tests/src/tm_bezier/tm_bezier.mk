tests.tm_bezier.c.out := ${build_dir}tests.tm_bezier.c${exe_ext}

${tests.tm_bezier.c.out}: tests/src/tm_bezier/main.c tm_bezier.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_bezier/main.c, $@)

tests.tm_bezier.c: ${tests.tm_bezier.c.out}

tests.tm_bezier: ${tests.tm_bezier.c.out}

tests.tm_bezier.run: