# Makefile that supports gcc, clang, cl and debug/release builds
# author: Tolga Mizrak 2018
# based on: https://stackoverflow.com/a/48793058

# some references for GNU make
# quick overview:           http://www2.ipp.mpg.de/~dpc/gmake/make_115.html
# variables:                https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
# functions for file names: http://www2.ipp.mpg.de/~dpc/gmake/make_75.html

# these can be changed from outside and this makefile will autoconfig

# build mode, either debug or release
BUILD := debug
# can be gcc, clang or cl
COMPILER := cl
FALLBACK_COMPILER := gcc

build_dir := build/${BUILD}
# output filename
exe       := main

# whether 32bit or 64bit
arch := 64

# autoconfig
ifeq (${OS}, Windows_NT)
	ext := .exe
	MKDIR_CMD = mkdir $(subst /,\,${1})
else
	ifeq (${COMPILER}, cl)
		COMPILER := ${FALLBACK_COMPILER}
	endif
	ext := .out
	MKDIR_CMD = mkdir -p ${1}
endif

# files
INCLUDE_DIRS :=

# tests

TESTS_INCLUDE_DIRS  := tests/external ./ tests/src/
TESTS_DOCTEST_DEP   := tests/external/doctest/doctest.h

# tm_conversion
TM_CONVERSION_SRC  := tests/src/tm_conversion/main.cpp
TM_CONVERSION_DEPS := ${build_dir} ${TM_CONVERSION_SRC} tm_conversion.h ${TESTS_DOCTEST_DEP}
TM_CONVERSION_DEPS += tests/src/assert_throws.h tests/src/assert_throws.cpp
TM_CONVERSION_OUT  := ${build_dir}/tm_conversion_tests${ext}

TM_CONVERSION_SRC_C  := tests/src/tm_conversion/main.c
TM_CONVERSION_DEPS_C := ${build_dir} ${TM_CONVERSION_SRC_C}
TM_CONVERSION_OUT_C  := ${build_dir}/tm_conversion_tests_c${ext}

# tools

gcc_version_suffix := -8
CXX.gcc := g++${gcc_version_suffix}
CC.gcc  := gcc${gcc_version_suffix}

clang_version_suffix := -7
CXX.clang := clang++${clang_version_suffix}
CC.clang  := clang${clang_version_suffix}

# my_cl is a batch file in PATH that just calls vcvarsall to set environment variables for the given architecture
# before calling cl with the supplied arguments
CXX.cl  := my_cl x${arch}
CC.cl   := my_cl x${arch}

# compiler
CXX := ${CXX.${COMPILER}}
CC  := ${CC.${COMPILER}}

# common preprocessor defines

DEFINES.debug := _DEBUG DEBUG
DEFINES.release := NDEBUG

# warnings for gcc and clang

GCC_WARNINGS := -Wall -Wextra -Werror -pedantic -pedantic-errors
GCC_WARNINGS += -Wno-unused-parameter

# gcc flags

# common for both C and C++
# -Og
FLAGS.gcc.debug   := -fstack-protector-all -g -ggdb -fsanitize=address -fno-omit-frame-pointer
FLAGS.gcc.release := -O3 -march=native -DNDEBUG
FLAGS.gcc         := ${GCC_WARNINGS} ${FLAGS.gcc.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

CXXFLAGS.gcc.debug   :=
CXXFLAGS.gcc.release :=
CXXFLAGS.gcc         := -m${arch} -std=c++17 ${CXXFLAGS.gcc.${BUILD}} ${FLAGS.gcc}

CFLAGS.gcc.debug   :=
CFLAGS.gcc.release :=
CFLAGS.gcc         := -m${arch} -std=c99 ${CFLAGS.gcc.${BUILD}} ${FLAGS.gcc}

LDFLAGS.gcc.debug   :=
LDFLAGS.gcc.release :=
LDFLAGS.gcc         := -m${arch} ${LDFLAGS.gcc.${BUILD}}

LDLIBS.gcc.debug    :=
LDLIBS.gcc.release  :=
LDLIBS.gcc          := ${LDLIBS.gcc.${BUILD}}

INCLUDECOMMAND.gcc  = $(addprefix -I,${1})
DEFINECOMMAND.gcc   = $(addprefix -D,${1})

INCLUDEFLAGS.gcc    := $(addprefix -I,${INCLUDE_DIRS})
CXXINCLUDEFLAGS.gcc := ${INCLUDEFLAGS.gcc}
CINCLUDEFLAGS.gcc   := ${INCLUDEFLAGS.gcc}

# clang flags

FLAGS.clang.debug   := -O0 -fstack-protector-all -g -ggdb -fsanitize=address -fno-omit-frame-pointer
FLAGS.clang.release := -O3 -march=native -DNDEBUG
FLAGS.clang         := ${GCC_WARNINGS} ${FLAGS.clang.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

CXXFLAGS.clang.debug   :=
CXXFLAGS.clang.release :=
CXXFLAGS.clang         := -m${arch} -std=c++17 ${CXXFLAGS.clang.${BUILD}} ${FLAGS.clang}

CFLAGS.clang.debug   :=
CFLAGS.clang.release :=
CFLAGS.clang         := -m${arch} -std=c99 ${CFLAGS.clang.${BUILD}} ${FLAGS.clang}

LDFLAGS.clang.debug   :=
LDFLAGS.clang.release :=
LDFLAGS.clang         := -m${arch} ${LDFLAGS.clang.${BUILD}}

LDLIBS.clang.debug    :=
LDLIBS.clang.release  :=
LDLIBS.clang          := ${LDLIBS.clang.${BUILD}}

INCLUDECOMMAND.clang  = $(addprefix -I,${1})
DEFINECOMMAND.clang   = $(addprefix -D,${1})

INCLUDEFLAGS.clang    := $(addprefix -I,${INCLUDE_DIRS})
CXXINCLUDEFLAGS.clang := ${INCLUDEFLAGS.clang}
CINCLUDEFLAGS.clang   := ${INCLUDEFLAGS.clang}

# cl flags

FLAGS.cl.debug   := /Od /Zi /MDd /Fd${build_dir}/
FLAGS.cl.release := /DNDEBUG /MD /GS- /Gy /fp:fast /Ox /Oy- /GL /Oi /O2
FLAGS.cl         := /EHsc /W4 /Oi /DNOMINMAX /DUNICODE  \
					/D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS \
					${FLAGS.cl.${BUILD}} $(addprefix /D, ${DEFINES.${BUILD}}) \
					/Fo${build_dir}/ /FC

CXXFLAGS.cl.debug   :=
CXXFLAGS.cl.release :=
CXXFLAGS.cl         := /std:c++latest ${CXXFLAGS.cl.${BUILD}} ${FLAGS.cl}

CFLAGS.cl.debug   :=
CFLAGS.cl.release :=
CFLAGS.cl         := ${CFLAGS.cl.${BUILD}} ${FLAGS.cl}

LDFLAGS.cl.debug   :=
LDFLAGS.cl.release := /LTCG
# subsystem is either CONSOLE or WINDOWS most of the time
LDFLAGS.cl         := /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /nologo ${LDFLAGS.cl.${BUILD}}

LDLIBS.cl.debug    :=
LDLIBS.cl.release  :=
LDLIBS.cl          := ${LDLIBS.cl.${BUILD}}

INCLUDECOMMAND.cl  = $(addprefix /I,${1})
DEFINECOMMAND.cl   = $(addprefix /D,${1})

INCLUDEFLAGS.cl    := $(addprefix /I,${INCLUDE_DIRS})
CXXINCLUDEFLAGS.cl := ${INCLUDEFLAGS.cl}
CINCLUDEFLAGS.cl   := ${INCLUDEFLAGS.cl}

# flags

CXXFLAGS        := ${CXXFLAGS.${COMPILER}}
CFLAGS          := ${CFLAGS.${COMPILER}}
LDFLAGS         := ${LDFLAGS.${COMPILER}}
CXXINCLUDEFLAGS := ${CXXINCLUDEFLAGS.${COMPILER}}
CINCLUDEFLAGS   := ${CINCLUDEFLAGS.${COMPILER}}

# clang-tidy

clang-tidy-checks := *
# allow c-style varargs functions (printf)
clang-tidy-checks += ,-cppcoreguidelines-pro-type-vararg,-hicpp-vararg
# disable some warnings because they trigger on C code
clang-tidy-checks += ,-modernize-use-using,-modernize-deprecated-headers,-hicpp-deprecated-headers


# helpers to remove all spaces from clang-tidy-checks
empty :=
space := ${empty} ${empty}

clang-tidy-flags := -header-filter=.*

# cxx_compile output parameter
OUT_FILE.gcc = -o
OUT_FILE.clang = -o
OUT_FILE.cl = /link /OUT:

# compile command
# ${1} is the source
# ${2} is output filename
# ${3} are include directories
# ${4} are additional definitions
override cxx_compile = ${CXX} ${CXXFLAGS} ${call INCLUDECOMMAND.${COMPILER},${3}} $(call DEFINECOMMAND.${COMPILER},${4}) ${1} ${OUT_FILE.${COMPILER}}${2} ${LDFLAGS} ${LDLIBS}
override c_compile = ${CC} ${CFLAGS} ${call INCLUDECOMMAND.${COMPILER},${3}} $(call DEFINECOMMAND.${COMPILER},${4}) ${1} ${OUT_FILE.${COMPILER}}${2} ${LDFLAGS} ${LDLIBS}

# targets
all: directories

directories: ${build_dir}

${build_dir}:
	@$(call MKDIR_CMD,$@)

clean:
	rm -rf ${build_dir}

check-all: ${CXX_SRC}
	clang-tidy-8 $^ ${clang-tidy-flags} -checks=$(subst ${space},${empty},${clang-tidy-checks}) \
	-- ${CXXFLAGS.clang} ${CXXINCLUDEFLAGS} $^ ${OUT_FILE.clang} ${exe_file.CXX} ${LDFLAGS} ${LDLIBS}

.PHONY: clean all check-all tm_conversion-tests tm_conversion-fast-tests tm_conversion-check

# merge

MERGE_INCLUDE_DIRS := tools/merge/src
MERGE_SRC          := tools/merge/src/main.cpp
MERGE_DEPS         := ${build_dir} tools/merge/src/main.cpp
MERGE_OUT          := ${build_dir}/merge${ext}

${MERGE_OUT}: ${MERGE_DEPS}
	@echo building merge
	@${CXX} ${CXXFLAGS} \
		$(call INCLUDECOMMAND.${COMPILER}, ${MERGE_INCLUDE_DIRS}) \
		${MERGE_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

merge: ${MERGE_OUT}

# tm_conversion
tm_conversion-c: ${TM_CONVERSION_DEPS_C}
	@${CC} ${CFLAGS} \
		$(call INCLUDECOMMAND.cl, ${TESTS_INCLUDE_DIRS}) \
		${TM_CONVERSION_SRC_C} \
		${OUT_FILE.${COMPILER}}${TM_CONVERSION_OUT_C} \
		${LDFLAGS} ${LDLIBS}

${TM_CONVERSION_OUT}: ${TM_CONVERSION_DEPS}
	@${CXX} ${CXXFLAGS} \
		$(call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}) \
		${TM_CONVERSION_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

tm_conversion-tests: ${TM_CONVERSION_OUT}

tm_conversion-run-tests: ${TM_CONVERSION_OUT}
	@echo TESTING: all tests
	@${TM_CONVERSION_OUT}

TM_CONVERSION_OUT_FAST := ${TM_CONVERSION_OUT} --test-case-exclude="Test float roundtrip,Test float accuracy"
ifeq (${OS}, Windows_NT)
	# cmd doesn't seem to like forward slashes on commands with arguments
	TM_CONVERSION_OUT_FAST := $(subst /,\,${TM_CONVERSION_OUT_FAST})
endif
tm_conversion-run-tests-fast: ${TM_CONVERSION_OUT}
	@echo TESTING: fast tests
	@${TM_CONVERSION_OUT_FAST}

tm_conversion-check: tm_conversion
	clang-tidy-8 tm_conversion.h ${clang-tidy-flags} \
		-checks=$(subst ${space},${empty},${clang-tidy-checks}),-llvm-header-guard \
		-- ${CXXFLAGS.clang} ${CXXINCLUDEFLAGS} ${TM_CONVERSION_SRC} \
		${OUT_FILE.clang}${TM_CONVERSION_OUT} ${LDFLAGS} ${LDLIBS}

# tm_print

TM_PRINT_UNMERGED := ${build_dir}/tm_print-unmerged${ext}

TM_PRINT_SRC  := tests/src/tm_print/main.cpp
TM_PRINT_DEPS := ${build_dir} ${TM_PRINT_SRC} ./tm_print.h tm_conversion.h ${TESTS_DOCTEST_DEP}
TM_PRINT_DEPS += tests/src/assert_throws.h tests/src/assert_throws.cpp
TM_PRINT_OUT  := ${build_dir}/tm_print_tests${ext}

TM_PRINT_TESTS_DEFAULT := ${build_dir}/tm_print_default${ext}
TM_PRINT_TESTS_CRT := ${build_dir}/tm_print_crt${ext}
TM_PRINT_TESTS_CRT_SIGNED_SIZE_T := ${build_dir}/tm_print_crt_signed_size_t${ext}
TM_PRINT_TESTS_TM_CONVERSION := ${build_dir}/tm_print_tm_conversion${ext}
TM_PRINT_TESTS_TM_CONVERSION_SIGNED_SIZE_T := ${build_dir}/tm_print_tm_conversion_signed_size_t${ext}
TM_PRINT_TESTS_CHARCONV := ${build_dir}/tm_print_charconv${ext}
TM_PRINT_TESTS_CHARCONV_SIGNED_SIZE_T := ${build_dir}/tm_print_charconv_signed_size_t${ext}

TM_PRINT_ALL_CONFIGS_DEPS := ${TM_PRINT_TESTS_DEFAULT}
TM_PRINT_ALL_CONFIGS_DEPS += ${TM_PRINT_TESTS_CRT} ${TM_PRINT_TESTS_CRT_SIGNED_SIZE_T}
TM_PRINT_ALL_CONFIGS_DEPS += ${TM_PRINT_TESTS_TM_CONVERSION}
TM_PRINT_ALL_CONFIGS_DEPS += ${TM_PRINT_TESTS_TM_CONVERSION_SIGNED_SIZE_T}
TM_PRINT_ALL_CONFIGS_DEPS += ${TM_PRINT_TESTS_CHARCONV} ${TM_PRINT_TESTS_CHARCONV_SIGNED_SIZE_T}

tm_print.h: ${TM_PRINT_UNMERGED} ${MERGE_OUT} src/tm_print/*.cpp src/tm_print/*.h
	@echo merging tm_print.h
	@${MERGE_OUT} src/tm_print/main.cpp $@ src/tm_print -r

tm_print-merge: tm_print.h

${TM_PRINT_UNMERGED}: src/tm_print/*.cpp src/tm_print/*.h
	@${CXX} ${CXXFLAGS} \
		$(call INCLUDECOMMAND.${COMPILER},src/tm_print ./) \
		src/tm_print/test.cpp \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

tm_print-unmerged: ${TM_PRINT_UNMERGED}

${TM_PRINT_TESTS_DEFAULT}: ${TM_PRINT_DEPS}
	@echo default config
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_CRT}: ${TM_PRINT_DEPS}
	@echo crt backend
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_CRT TMP_FLOAT_BACKEND_CRT) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_TM_CONVERSION}: ${TM_PRINT_DEPS}
	@echo tm_conversion backend
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_TM_CONVERSION TMP_FLOAT_BACKEND_TM_CONVERSION) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_CHARCONV}: ${TM_PRINT_DEPS}
	@echo charconv backend (only int, since float not implemented yet in any stl)
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_CHARCONV) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_CRT_SIGNED_SIZE_T}: ${TM_PRINT_DEPS}
	@echo crt backend with signed size_t
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_CRT TMP_FLOAT_BACKEND_CRT) \
		$(call DEFINECOMMAND.${COMPILER},SIGNED_SIZE_T) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_TM_CONVERSION_SIGNED_SIZE_T}: ${TM_PRINT_DEPS}
	@echo tm_conversion backend with signed size_t
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_TM_CONVERSION TMP_FLOAT_BACKEND_TM_CONVERSION) \
		$(call DEFINECOMMAND.${COMPILER},SIGNED_SIZE_T) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

${TM_PRINT_TESTS_CHARCONV_SIGNED_SIZE_T}: ${TM_PRINT_DEPS}
	@echo charconv backend (only int, since float not implemented yet in any stl) with signed size_t
	@${CXX} ${CXXFLAGS} \
		${call INCLUDECOMMAND.${COMPILER}, ${TESTS_INCLUDE_DIRS}} \
		$(call DEFINECOMMAND.${COMPILER},TMP_INT_BACKEND_CHARCONV) \
		$(call DEFINECOMMAND.${COMPILER},SIGNED_SIZE_T) \
		${TM_PRINT_SRC} \
		${OUT_FILE.${COMPILER}}$@ \
		${LDFLAGS} ${LDLIBS}

tm_print-tests: ${TM_PRINT_ALL_CONFIGS_DEPS}

tm_print-run-tests-default: ${TM_PRINT_TESTS_DEFAULT}
	@echo TESTING: default config
	@${TM_PRINT_TESTS_DEFAULT}

tm_print-run-tests: tm_print-tests
	@echo TESTING: default config
	@${TM_PRINT_TESTS_DEFAULT}
	@echo TESTING: crt backend
	@${TM_PRINT_TESTS_CRT}
	@echo TESTING: crt backend with signed size_t
	@${TM_PRINT_TESTS_CRT_SIGNED_SIZE_T}
	@echo TESTING: tm_conversion backend
	@${TM_PRINT_TESTS_TM_CONVERSION}
	@echo TESTING: tm_conversion backend with signed size_t
	@${TM_PRINT_TESTS_TM_CONVERSION_SIGNED_SIZE_T}
	@echo TESTING: charconv backend (only int, since float not implemented yet in any stl)
	@${TM_PRINT_TESTS_CHARCONV}
	@echo TESTING: charconv backend (only int, since float not implemented yet in any stl) with signed size_t
	@${TM_PRINT_TESTS_CHARCONV_SIGNED_SIZE_T}

# tm_json.h

TM_JSON_DEPS := ${build_dir} ${TESTS_DOCTEST_DEP} tm_json.h tests/src/tm_json/main.cpp
TM_JSON_SRC := tests/src/tm_json/main.cpp
TM_JSON_TESTS_OUT := ${build_dir}/tm_json_tests${ext}

${TM_JSON_TESTS_OUT}: ${TM_JSON_DEPS}
	@$(call cxx_compile,${TM_JSON_SRC},$@,${TESTS_INCLUDE_DIRS},)

tm_json-tests: ${TM_JSON_TESTS_OUT}

# tm_bin_packing.h

TM_BIN_PACKING_DEPS := ${build_dir} ${TESTS_DOCTEST_DEP} tm_bin_packing.h
TM_BIN_PACKING_SRC_C := tests/src/tm_bin_packing/main.c
TM_BIN_PACKING_TESTS_OUT_C := ${build_dir}/tm_bin_packing_tests${ext}

${TM_BIN_PACKING_TESTS_OUT_C}: ${TM_BIN_PACKING_DEPS} tests/src/tm_bin_packing/main.c
	@$(call c_compile,${TM_BIN_PACKING_SRC_C},$@,${TESTS_INCLUDE_DIRS},)

tm_bin_packing-c-build: ${TM_BIN_PACKING_TESTS_OUT_C}