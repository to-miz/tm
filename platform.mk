# Disable the built-in rules.
.SUFFIXES:

# Default rule.
all:

# Set up project and platform defaults. These can be user supplied in the commandline.

BUILD     := debug
CXX       ?= g++
CC        ?= gcc
BUILD_DIR := build
ARCH      := 64
VERBOSE   := false

# Include config that may change any of the variables here.

ifneq (${CONFIG_FILE},)
-include ${CONFIG_FILE}
else
-include config.mk
endif

# Set up declarations used for all supported platforms and compilers.

# Directories

build_dir      := ${BUILD_DIR}/${BUILD}/
debug_dir      := ${BUILD_DIR}/debug/
release_dir    := ${BUILD_DIR}/release/
build_dir_root := ${BUILD_DIR}/
path_sep       := /

# Extensions depending on platform.
ifeq (${OS},Windows_NT)
	os := windows
	exe_ext := .exe
	lib_ext := .lib
	dll_ext := .dll
	obj_ext := .obj

	mkdir_cmd = mkdir $(subst /,\,${1}) 2>nul

	# Turn paths seperators to Windows path seperators.
	build_dir      := $(subst /,\,${build_dir})
	debug_dir      := $(subst /,\,${debug_dir})
	release_dir    := $(subst /,\,${release_dir})
	build_dir_root := $(subst /,\,${build_dir_root})
	path_sep       := $(subst /,\,${path_sep})

	make_quoted_string = ""${1}""

	clean_build_dir := del /f/q/s \
	                   ${build_dir_root}*.exe ${build_dir_root}*.lib ${build_dir_root}*.dll \
	                   ${build_dir_root}*.obj ${build_dir_root}*.pdb ${build_dir_root}*.exp >nul 2>nul

	shell_random := cmd /c "echo %RANDOM%"
	shell_delete = del /f/q/s "${1}${path_sep}${2}" >nul 2>nul

	windows_defines := NOMINMAX UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS
else
	os := linux
	exe_ext := .out
	lib_ext := .a
	dll_ext := .so
	obj_ext := .o

	mkdir_cmd = mkdir -p ${1}

	make_quoted_string = \"${1}\"

	clean_build_dir := find ${build_dir_root} -type f \
	                   \( -name "*.out" -o -name "*.a" -o -name "*.so" -o -name "*.o" \) \
	                   -delete

	shell_random := /bin/bash -c "echo $$RANDOM"
	shell_delete = find /f/q/s "${1}" -name "${2}" -type f -delete
endif

# Helper functions.
make_build_dir = ${BUILD_DIR}${path_sep}${1}${path_sep}

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
cxx_maybe_gcc := $(findstring gcc,${CXX})$(findstring g++,${CXX})

ifneq ($(findstring clang-cl,${CXX}),)
	cxx_compiler_selector := cl
	cxx_compiler.cl := ${CXX}
	clang_cl_warnings := -Qunused-arguments
else
ifneq ($(findstring clang,${CXX}),)
	cxx_compiler_selector := clang
	cxx_compiler.clang := ${CXX}
else
ifneq (${cxx_maybe_gcc},)
	cxx_compiler_selector := gcc
	cxx_compiler.gcc := ${CXX}
else
ifneq ($(findstring .bat,${CXX}),)
	# batch file wrapper around cl.exe where you can specify the target architecture.
	cxx_compiler_selector := cl
	cxx_compiler.cl := ${CXX} x${ARCH}
else
ifneq ($(findstring cl,${CXX}),)
	# cl
	cxx_compiler_selector := cl
	cxx_compiler.cl := ${CXX}
else
	cxx_compiler_selector := other
	cxx_compiler.other := ${CXX}
endif # cl
endif # cl.bat
endif # gcc
endif # clang
endif # clang-cl

c_maybe_gcc := $(findstring gcc,${CC})$(findstring g++,${CC})

ifneq ($(findstring clang-cl,${CC}),)
	c_compiler_selector := cl
	c_compiler.cl := ${CC}
else
ifneq ($(findstring clang,${CC}),)
	c_compiler_selector := clang
	c_compiler.clang := ${CC}
else
ifneq (${c_maybe_gcc},)
	c_compiler_selector := gcc
	c_compiler.gcc := ${CC}
else
ifneq ($(findstring .bat,${CC}),)
	# batch file wrapper around cl.exe where you can specify the target architecture.
	c_compiler_selector := cl
	c_compiler.cl := ${CC} x${ARCH}
else
ifneq ($(findstring cl,${CC}),)
	# cl
	c_compiler_selector := cl
	c_compiler.cl := ${CC}
else
	c_compiler_selector := other
	c_compiler.other := ${CC}
	output_directive.other := -o
endif # cl
endif # cl.bat
endif # gcc
endif # clang
endif # clang-cl

# Common defines.

DEFINES.debug := _DEBUG ${windows_defines}
DEFINES.release := NDEBUG ${windows_defines}

# clang

warnings.clang := -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-gnu-zero-variadic-macro-arguments -Wno-newline-eof
sanitize.clang ?= -fsanitize=address

options.clang.debug   := -fstack-protector-all -g -ggdb -fno-omit-frame-pointer ${sanitize.clang}
options.clang.release := -O3 -march=native
options.clang = ${warnings.clang} -m${ARCH} ${options.clang.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

cxx_options.clang.debug :=
cxx_options.clang.release :=
cxx_options.clang = -std=c++17 ${cxx_options.clang.${BUILD}} ${options.clang}

c_options.clang.debug :=
c_options.clang.release :=
c_options.clang = -std=c99 -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE ${c_options.clang.${BUILD}} ${options.clang}

link_options.clang.debug   :=
link_options.clang.release :=
link_options.clang         = -m${ARCH} ${link_options.clang.${BUILD}}

link_libs.clang.debug    :=
link_libs.clang.release  :=
link_libs.clang          = ${link_libs.clang.${BUILD}}

output_directive.clang   = -o${1}

# gcc

warnings.gcc := -Wall -Wextra -Werror -pedantic -pedantic-errors
sanitize.gcc ?= -fsanitize=address

options.gcc.debug   := -fstack-protector-all -g -ggdb -fno-omit-frame-pointer ${sanitize.gcc}
options.gcc.release := -O3 -march=native
# ifeq (${ARCH},64)
# options.gcc.debug += -municode
# options.gcc.release += -municode
# endif
options.gcc = ${warnings.gcc} -m${ARCH} ${options.gcc.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}})

cxx_options.gcc.debug :=
cxx_options.gcc.release :=
cxx_options.gcc = -std=c++17 ${cxx_options.gcc.${BUILD}} ${options.gcc}

c_options.gcc.debug :=
c_options.gcc.release :=
c_options.gcc = -std=c99 -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE ${c_options.gcc.${BUILD}} ${options.gcc}

link_options.gcc.debug   :=
link_options.gcc.release :=
link_options.gcc         = -m${ARCH} ${link_options.gcc.${BUILD}}

link_libs.gcc.debug    :=
link_libs.gcc.release  :=
link_libs.gcc          = ${link_libs.gcc.${BUILD}}

output_directive.gcc   = -o${1}

output_directive.other = -o${1}

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
warnings.cl  += -w44746 # volatile access of '<expression>' is subject to /volatile:[iso|ms] setting
warnings.cl += ${clang_cl_warnings}

sanitize.cl := -RTCsu -GS -sdl
# sanitize.cl += -analyze

options.cl.debug   := -Od -Zi -MDd ${sanitize.cl}
options.cl.release := -DNDEBUG -MD -GS- -Gy -fp:fast -Ox -Oy- -GL -Gw -Oi -O2
# options.cl.exception := -EHsc
options.cl.exception := -EHs
options.cl        = ${warnings.cl} ${options.cl.exception} -Oi -permissive- -utf-8 -volatile:iso \
					${options.cl.${BUILD}} $(addprefix -D, ${DEFINES.${BUILD}}) -FS -FC -bigobj

cxx_options.cl.debug   :=
cxx_options.cl.release :=
cxx_options.cl         = -std:c++latest ${cxx_options.cl.${BUILD}} ${options.cl}

c_options.cl.debug   :=
c_options.cl.release :=
c_options.cl         = ${c_options.cl.${BUILD}} ${options.cl}

link_options.cl.debug   :=
link_options.cl.release := -ltcg
# Subsystem is either CONSOLE or WINDOWS most of the time.
link_options.cl.subsystem := CONSOLE
link_options.cl         = -subsystem:${link_options.cl.subsystem} -incremental:NO -nologo ${link_options.cl.${BUILD}}

link_libs.cl.debug    :=
link_libs.cl.release  :=
link_libs.cl          = ${link_libs.cl.${BUILD}}

pdb_suffix.cl :=
output_directive.cl = -Fd"${1}${pdb_suffix.cl}.pdb" -Fe"${1}" -Fo"${build_dir}/" \
                      -link -out:${1} -pdb:${1}${pdb_suffix.cl}.pdb

# Generic

cxx_options = ${CPPFLAGS} ${CXXFLAGS} ${cxx_options.${cxx_compiler_selector}}
c_options = ${CPPFLAGS} ${CFLAGS} ${c_options.${c_compiler_selector}}
cxx_link_options = ${LDFLAGS} ${link_options.${cxx_compiler_selector}}
c_link_options = ${LDFLAGS} ${link_options.${c_compiler_selector}}
cxx_link_libs = ${LDLIBS} ${link_libs.${cxx_compiler_selector}}
c_link_libs = ${LDLIBS} ${link_libs.${c_compiler_selector}}

empty :=
space := ${empty} ${empty}
comma := ,
define newline :=


endef

# Compilation commands
# ${1} Source files
# ${2} Output filename
# ${3} Additional includes
# ${4} Additional defines
override cxx_compile_and_link = ${hide}${cxx_compiler.${cxx_compiler_selector}} \
								${cxx_options} \
								${CXX_OPTIONS.${cxx_compiler_selector}} \
								$(addprefix -I,${CXX_INCLUDES}) \
								$(addprefix -I,${CXX_INCLUDES.${cxx_compiler_selector}}) \
								$(addprefix -I,${3}) \
								$(addprefix -D,${CXX_DEFINES}) \
								$(addprefix -D,${CXX_DEFINES.${cxx_compiler_selector}}) \
								$(addprefix -D,${4}) \
								${1} \
								$(call output_directive.${cxx_compiler_selector},$(subst ${space},${empty},${2})) \
								${cxx_link_options} \
								${CXX_LINK_OPTIONS.${cxx_compiler_selector}} \
								${cxx_link_libs} \
								${CXX_LINK_LIBS.${cxx_compiler_selector}}

override c_compile_and_link   = ${hide}${c_compiler.${c_compiler_selector}} \
								${c_options} \
								${C_OPTIONS.${c_compiler_selector}} \
								$(addprefix -I,${C_INCLUDES}) \
								$(addprefix -I,${C_INCLUDES.${c_compiler_selector}}) \
								$(addprefix -I,${3}) \
								$(addprefix -D,${C_DEFINES}) \
								$(addprefix -D,${C_DEFINES.${c_compiler_selector}}) \
								$(addprefix -D,${4}) \
								${1} \
								$(call output_directive.${c_compiler_selector},$(subst ${space},${empty},${2})) \
								${c_link_options} \
								${C_LINK_OPTIONS.${c_compiler_selector}} \
								${c_link_libs} \
								${C_LINK_LIBS.${c_compiler_selector}}
