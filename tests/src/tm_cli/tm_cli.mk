tests.tm_cli.c.out := ${build_dir}tests.tm_cli.c${exe_ext}

${tests.tm_cli.c.out}: tm_cli.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, tests/src/tm_cli/main.c, $@)

tests.tm_cli: ${tests.tm_cli.c.out}

tests.tm_cli.run: