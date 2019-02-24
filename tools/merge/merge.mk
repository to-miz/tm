MERGE_BUILD ?= release

merge_build_dir := ${BUILD_DIR}/${MERGE_BUILD}/

merge.out := ${merge_build_dir}merge${exe_ext}

${merge.out}: override BUILD := ${MERGE_BUILD}
${merge.out}: tools/merge/src/main.cpp
	${hide}echo Compiling $@.
	${hide}$(call cpp_compile_and_link, $^, $@)

merge: ${merge.out}