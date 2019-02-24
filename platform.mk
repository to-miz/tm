# Disable the built-in rules.
.SUFFIXES:

# Set up project and platform defaults. These can be user supplied in the commandline.

BUILD     := debug
COMPILER  := default
BUILD_DIR := build
ARCH      := 64
VERBOSE   := false

# Include config that may change any of the variables here.

-include config.mk

# Set up declarations used for all supported platforms and compilers.

# Extensions depending on platform.
ifeq (${OS},Windows_NT)
	os := windows
	exe_ext := .exe
	lib_ext := .lib
	dll_ext := .dll
	obj_ext := .obj

	ifeq ("${COMPILER}", "default")
		override COMPILER := cl
	endif

	mkdir_cmd = mkdir $(subst /,\,${1}) 2>nul
else
	os := linux
	exe_ext := .out
	lib_ext := .a
	dll_ext := .so
	obj_ext := .o

	ifeq ("${COMPILER}", "default")
		override COMPILER := gcc
	endif

	mkdir_cmd = mkdir -p ${1}
endif

# Directories

build_dir   := ${BUILD_DIR}/${BUILD}/
debug_dir   := ${BUILD_DIR}/debug/
release_dir := ${BUILD_DIR}/release/

# Create build dirs.
$(shell $(call mkdir_cmd, ${debug_dir}))
$(shell $(call mkdir_cmd, ${release_dir}))

# Verbosity of the build rules.
ifeq ("${VERBOSE}","false")
	hide := @
else
	hide :=
endif

# Compiler detection.
maybe_gcc := $(findstring gcc,${COMPILER}) $(findstring g++,${COMPILER})

ifneq ($(findstring clang,${COMPILER}),)
	compiler_selector := clang
else
ifneq ($(findstring gcc,${maybe_gcc}),)
	compiler_selector := gcc
else
ifneq ($(findstring .bat,${COMPILER}),)
	# batch file wrapper around cl.exe where you can specify the target architecture.
	compiler_selector := cl
	cpp_compiler.cl := ${COMPILER} x${ARCH}
	c_compiler.cl := ${COMPILER} x${ARCH}
else
ifneq ($(findstring cl,${COMPILER}),)
	# cl
	compiler_selector := cl
	cpp_compiler.cl := ${COMPILER}
	c_compiler.cl := ${COMPILER}
else
$(error Unsupported compiler "${COMPILER}")
endif # cl
endif # cl.bat
endif # gcc
endif # clang

# Common defines.

DEFINES.debug := _DEBUG
DEFINES.release := NDEBUG

# clang
version_suffix.clang := $(patsubst clang%,%,$(patsubst clang++%,%,${COMPILER}))
cpp_compiler.clang := clang++${version_suffix.clang}
c_compiler.clang := clang${version_suffix.clang}

warnings.clang := -Wall -Wextra -Werror -pedantic -pedantic-errors
sanitize.clang := -fsanitize=address

options.clang.debug   := -fstack-protector-all -g -ggdb -fno-omit-frame-pointer ${sanitize.clang}
options.clang.release := -O3 -march=native
options.clang = ${warnings.clang} -m${ARCH} ${options.clang.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

cpp_options.clang.debug :=
cpp_options.clang.release :=
cpp_options.clang = -std=c++17 ${cpp_options.clang.${BUILD}} ${options.clang}

c_options.clang.debug :=
c_options.clang.release :=
c_options.clang = -std=c99 ${c_options.clang.${BUILD}} ${options.clang}

link_options.clang.debug   :=
link_options.clang.release :=
link_options.clang         = -m${ARCH} ${link_options.clang.${BUILD}}

link_libs.clang.debug    :=
link_libs.clang.release  :=
link_libs.clang          = ${link_libs.clang.${BUILD}}

output_directive.clang   := -o

# gcc
version_suffix.gcc := $(patsubst gcc%,%,$(patsubst g++%,%,${COMPILER}))
cpp_compiler.gcc := g++${version_suffix.gcc}
c_compiler.gcc := gcc${version_suffix.gcc}

warnings.gcc := -Wall -Wextra -Werror -pedantic -pedantic-errors
sanitize.gcc := -fsanitize=address

options.gcc.debug   := -fstack-protector-all -g -ggdb -fno-omit-frame-pointer ${sanitize.gcc}
options.gcc.release := -O3 -march=native
options.gcc = ${warnings.gcc} -m${ARCH} ${options.gcc.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

cpp_options.gcc.debug :=
cpp_options.gcc.release :=
cpp_options.gcc = -std=c++17 ${cpp_options.gcc.${BUILD}} ${options.gcc}

c_options.gcc.debug :=
c_options.gcc.release :=
c_options.gcc = -std=c99 ${c_options.gcc.${BUILD}} ${options.gcc}

link_options.gcc.debug   :=
link_options.gcc.release :=
link_options.gcc         = -m${ARCH} ${link_options.gcc.${BUILD}}

link_libs.gcc.debug    :=
link_libs.gcc.release  :=
link_libs.gcc          = ${link_libs.gcc.${BUILD}}

output_directive.gcc   := -o

# cl

warnings.cl  := -W4 -WX
warnings.cl  += -w44062 # enumerator 'identifier' in a switch of enum 'enumeration' is not handled
warnings.cl  += -w44242 # 'identifier': conversion from 'type1' to 'type2', possible loss of data
warnings.cl  += -w44254 # 'operator': conversion from 'type1' to 'type2', possible loss of data
warnings.cl  += -w44287 # 'operator': unsigned/negative constant mismatch
warnings.cl  += -w44388 # signed/unsigned mismatch
warnings.cl  += -w44302 # 'conversion': truncation from 'type1' to 'type2'
warnings.cl  += -w44826 # Conversion from 'type1' to 'type2' is sign-extended.
warnings.cl  += -w44905 # wide string literal cast to 'LPSTR'
warnings.cl  += -w44906 # string literal cast to 'LPWSTR'
warnings.cl  += -w45039 # 'function': pointer or reference to potentially throwing function passed
						# to extern C function under -EHc.
						# Undefined behavior may occur if this function throws an exception.

sanitize.cl := -RTCsu -GS -sdl

options.cl.debug   := -Od -Zi -MDd -Fd${build_dir} ${sanitize.cl}
options.cl.release := -DNDEBUG -MD -GS- -Gy -fp:fast -Ox -Oy- -GL -Gw -Oi -O2
options.cl        = ${warnings.cl} -EHsc -Oi -permissive- -utf-8 -volatile:iso -DNOMINMAX -DUNICODE  \
					-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS \
					${options.cl.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}}) \
					-Fo${build_dir} -FC -bigobj

cpp_options.cl.debug   :=
cpp_options.cl.release :=
cpp_options.cl         = -std:c++latest ${cpp_options.cl.${BUILD}} ${options.cl}

c_options.cl.debug   :=
c_options.cl.release :=
c_options.cl         = ${c_options.cl.${BUILD}} ${options.cl}

link_options.cl.debug   :=
link_options.cl.release := -ltcg
# Subsystem is either CONSOLE or WINDOWS most of the time.
link_options.cl         = -subsystem:CONSOLE -incremental:NO -nologo ${link_options.cl.${BUILD}}

link_libs.cl.debug    :=
link_libs.cl.release  :=
link_libs.cl          = ${link_libs.cl.${BUILD}}

output_directive.cl   := -link -out:

# Generic

cpp_options = ${cpp_options.${compiler_selector}}
c_options = ${c_options.${compiler_selector}}
link_options = ${link_options.${compiler_selector}}
link_libs = ${link_libs.${compiler_selector}}

empty :=
space := ${empty} ${empty}
comma := ,

# Compilation commands
# ${1} Source files
# ${2} Output filename
# ${3} Additional includes
# ${4} Additional defines
override cpp_compile_and_link = ${hide}${cpp_compiler.${compiler_selector}} \
								${cpp_options} \
								${CPP_OPTIONS.${compiler_selector}} \
								$(addprefix -I,${CPP_INCLUDES}) \
								$(addprefix -I,${CPP_INCLUDES.${compiler_selector}}) \
								$(addprefix -I,${3}) \
								$(addprefix -D,${CPP_DEFINES}) \
								$(addprefix -D,${CPP_DEFINES.${compiler_selector}}) \
								$(addprefix -D,${4}) \
								${1} \
								${output_directive.${compiler_selector}}$(subst ${space},${empty},${2}) \
								${link_options} \
								${LINK_OPTIONS.${compiler_selector}} \
								${link_libs} \
								${LINK_LIBS.${compiler_selector}}
override c_compile_and_link   = ${hide}${c_compiler.${compiler_selector}} \
								${c_options} \
								${C_OPTIONS.${compiler_selector}} \
								$(addprefix -I,${C_INCLUDES}) \
								$(addprefix -I,${C_INCLUDES.${compiler_selector}}) \
								$(addprefix -I,${3}) \
								$(addprefix -D,${C_DEFINES}) \
								$(addprefix -D,${C_DEFINES.${compiler_selector}}) \
								$(addprefix -D,${4}) \
								${1} \
								${output_directive.${compiler_selector}}$(subst ${space},${empty},${2}) \
								${link_options} \
								${LINK_OPTIONS.${compiler_selector}} \
								${link_libs} \
								${LINK_LIBS.${compiler_selector}}
