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
	MKDIR_CMD := cmd /c setlocal enableextensions && mkdir
else
	ifeq (${COMPILER}, cl)
		COMPILER := ${FALLBACK_COMPILER}
	endif
	ext := .out
	MKDIR_CMD := @mkdir -p
endif

# files
EXTERNAL  := external ../ src/

# tm_conversion
TM_CONVERSION_SRC := src/tm_conversion/main.cpp
TM_CONVERSION_DEPS := ${TM_CONVERSION_SRC} external/doctest/doctest.h
TM_CONVERSION_OUT := ${build_dir}/tm_conversion_tests${ext}

TM_CONVERSION_SRC_C := src/tm_conversion/main.c
TM_CONVERSION_DEPS_C := ${TM_CONVERSION_SRC_C}
TM_CONVERSION_OUT_C := ${build_dir}/tm_conversion_tests_c${ext}

exe_file.C   := ${build_dir}/${exe}_c${ext}
exe_file.CXX := ${build_dir}/${exe}${ext}

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
FLAGS.gcc.debug   := -Og -fstack-protector-all -g -ggdb -fsanitize=address -fno-omit-frame-pointer
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

INCLUDEFLAGS.gcc    := $(addprefix -I,${EXTERNAL})
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

INCLUDEFLAGS.clang    := $(addprefix -I,${EXTERNAL})
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

INCLUDEFLAGS.cl    := $(addprefix /I,${EXTERNAL})
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

# compile command
OUT_FILE.gcc = -o
OUT_FILE.clang = -o
OUT_FILE.cl = /link /OUT:


# targets
all: directories

tm_conversion-c: ${TM_CONVERSION_DEPS_C}
	@${CC} ${CFLAGS} ${CINCLUDEFLAGS} ${TM_CONVERSION_SRC_C} ${OUT_FILE.${COMPILER}}${TM_CONVERSION_OUT_C}${LDFLAGS} ${LDLIBS}

tm_conversion: ${TM_CONVERSION_DEPS}
	@${CXX} ${CXXFLAGS} ${CXXINCLUDEFLAGS} ${TM_CONVERSION_SRC} ${OUT_FILE.${COMPILER}}${TM_CONVERSION_OUT} ${LDFLAGS} ${LDLIBS}

tm_conversion-tests: tm_conversion
	${TM_CONVERSION_OUT}

tm_conversion-fast-tests: tm_conversion
	${TM_CONVERSION_OUT} --test-case-exclude="Test float roundtrip,Test float accuracy"

tm_conversion-check: tm_conversion
	clang-tidy-8 ../tm_conversion.h ${clang-tidy-flags} -checks=$(subst ${space},${empty},${clang-tidy-checks}),-llvm-header-guard -- ${CXXFLAGS.clang} ${CXXINCLUDEFLAGS} ${TM_CONVERSION_SRC} ${OUT_FILE.clang}${TM_CONVERSION_OUT} ${LDFLAGS} ${LDLIBS}

directories: ${build_dir}

${build_dir}:
	${MKDIR_CMD} $@

clean:
	rm -rf ${build_dir}

check-all: ${CXX_SRC}
	clang-tidy-8 $^ ${clang-tidy-flags} -checks=$(subst ${space},${empty},${clang-tidy-checks}) -- ${CXXFLAGS.clang} ${CXXINCLUDEFLAGS} $^ ${OUT_FILE.clang} ${exe_file.CXX} ${LDFLAGS} ${LDLIBS}

run: all
	./${exe_file.CXX}
	./${exe_file.C}

.PHONY: clean all run tm_conversion-tests tm_conversion-fast-tests tm_conversion-check