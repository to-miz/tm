tests.tm_polygon.c.out := ${build_dir}tests.tm_polygon.c${exe_ext}

${tests.tm_polygon.c.out}: tests/src/tm_polygon/main.c tm_polygon.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_polygon/main.c, $@)

tests.tm_polygon.c: ${tests.tm_polygon.c.out}

tests.tm_polygon: ${tests.tm_polygon.c.out}

tests.tm_polygon.run: