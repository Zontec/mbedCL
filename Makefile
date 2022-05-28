.PHONY: docs clean menuconfig tests shared static clean_objs cicd

#=========== PROJECT NAME ===========#
PROJECT := mbcrypt

BUILD_ARG_LIST = PROJECT=$(PROJECT)

BUILD_MAKEFILE := Makefile.build

all: tests shared static cicd

shared:
	@${MAKE} -f $(BUILD_MAKEFILE) shared BUILD_TYPE=SHARED $(BUILD_ARG_LIST)

static:
	@${MAKE} -f $(BUILD_MAKEFILE) static BUILD_TYPE=STATIC $(BUILD_ARG_LIST)

tests:
	@${MAKE} -f $(BUILD_MAKEFILE) $(PROJECT)_tests \
		CONFIG_BUILD_MODE_DEBUG=y BUILD_TYPE=EXECUTABLE $(BUILD_ARG_LIST)

cicd:
	@${MAKE} -f $(BUILD_MAKEFILE) cicd BUILD_TYPE=EXECUTABLE KCONFIG_CONFIG=test.config $(BUILD_ARG_LIST)

run_tests:
	./mbedcrypto_tests

clean_objs:
	@rm -rf build/

clean: clean_objs
	@rm -f $(PROJECT)*

docs:
	@doxygen docs/doxygen.cfg

menuconfig:
	@kconfig-mconf KConfig

config:
	@kconfig-conf KConfig