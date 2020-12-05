tm_resource_ptr.h: ${unmerged_deps} src/tm_resource_ptr/*.cpp ${merge.out}
	${hide}echo Merging $@.
	${hide}${merge.out} src/tm_resource_ptr/main.cpp $@ src/tm_resource_ptr -r

merge.all: tm_resource_ptr.h