MERGE_BUILD ?= release

merge_build_dir := ${BUILD_DIR}${path_sep}${MERGE_BUILD}${path_sep}

merge.out := ${merge_build_dir}merge${exe_ext}

${merge.out}: private override BUILD := ${MERGE_BUILD}
${merge.out}: tools/merge/src/main.cpp
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, tools/merge/src/main.cpp, $@, .)

merge: ${merge.out}