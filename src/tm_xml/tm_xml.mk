unmerged.tm_xml.out := ${build_dir}unmerged.tm_xml${exe_ext}
${unmerged.tm_xml.out}: ${unmerged_deps} src/tm_xml/*.cpp src/tm_xml/*.h
	${hide}echo Compiling $@.
	${hide}$(call cxx_compile_and_link, src/tm_xml/test.cpp, $@, src/tm_xml .)

unmerged.tm_xml: ${unmerged.tm_xml.out}

unmerged.tm_xml.c.out := ${build_dir}unmerged.tm_xml.c${exe_ext}
${unmerged.tm_xml.c.out}: ${unmerged_deps} src/tm_xml/*.cpp src/tm_xml/test.c src/tm_xml/*.h
	${hide}echo Compiling $@.
	${hide}$(call c_compile_and_link, src/tm_xml/test.c, $@, src/tm_xml .)

unmerged.tm_xml.c: ${unmerged.tm_xml.c.out}

tm_xml.h: ${unmerged.tm_xml.out} ${unmerged.tm_xml.c.out} ${merge.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_xml/main.h $@ src/tm_xml -r

merge.tm_xml: tm_xml.h

merge.all: tm_xml.h