# Requires GNU Make Version at least 3.82.
include platform.mk

# Automatically add dependencies and include_dirs to test rules.
tests.include_dirs  := tests/external . tests/src/
tests.dependencies  := tests/external/doctest/doctest.h tests/src/assert_throws.h tests/src/assert_throws.cpp

${build_dir}tests.%: ${tests.dependencies}
${build_dir}tests.%: private CXX_INCLUDES += ${tests.include_dirs}
${build_dir}tests.%: private C_INCLUDES   += ${tests.include_dirs}
# Testing needs functions marked as extern C to be able to throw C++ exceptions, so EHsc can't be used, otherwise UB.
${build_dir}tests.%: private options.cl.exception := -EHs

# Subprojects

include tools/merge/merge.mk # Defines target merge and output file merge.out.
include tools/unicode_gen/unicode_gen.mk # Defines target unicode_gen and output file unicode_gen.out.

include src/tm_conversion/tm_conversion.mk
include tests/src/tm_conversion/tm_conversion.mk

include src/tm_print/tm_print.mk
include tests/src/tm_print/tm_print.mk

include src/tm_json/tm_json.mk
include tests/src/tm_json/tm_json.mk

include src/tm_cli/tm_cli.mk
include tests/src/tm_cli/tm_cli.mk

include src/tm_stringutil/tm_stringutil.mk
include tests/src/tm_stringutil/tm_stringutil.mk

include tests/src/tm_bin_packing/tm_bin_packing.mk

include tests/src/tm_polygon/tm_polygon.mk

include tests/src/tm_bezier/tm_bezier.mk

include src/tm_unicode/tm_unicode.mk
include tests/src/tm_unicode/tm_unicode.mk

# Combined

merge.all.deps := tm_conversion.h tm_print.h tm_json.h tm_cli.h
merge.all.deps += tm_stringutil.h tm_bin_packing.h tm_polygon.h tm_bezier.h tm_unicode.h

merge.all: ${merge.all.deps}

tests.deps := tests.tm_conversion tests.tm_print tests.tm_json tests.tm_cli
tests.deps += tests.tm_stringutil tests.tm_bin_packing tests.tm_polygon tests.tm_bezier tests.tm_unicode

tests: ${tests.deps}

tests.run.deps := tests.tm_conversion.run tests.tm_print.run tests.tm_json.run tests.tm_cli.run
tests.run.deps += tests.tm_stringutil.run tests.tm_bin_packing.run tests.tm_polygon.run
tests.run.deps += tests.tm_bezier.run tests.tm_unicode.run

tests.run: ${tests.run.deps}

clean:
	${hide}echo Cleaning folder ${BUILD_DIR}${path_sep}.
	${hide}${clean_build_dir}