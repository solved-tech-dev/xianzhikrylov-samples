# Shared build and run rules for each independent sample.
THIS_COMMON_MK := $(abspath $(lastword $(MAKEFILE_LIST)))
REPO_ROOT := $(patsubst %/,%,$(dir $(THIS_COMMON_MK)))
SAMPLE_NAME ?= $(notdir $(CURDIR))

CC ?= gcc
CFLAGS ?= -O3 -std=c11 -Wall -Wextra -Wno-enum-compare
GPU_RUNTIME_LIB_DIR ?=
CONFIG_FILE ?= config.json
TARGET ?= sample

# Use libraries/<package-name>/ when present. A samples directory placed inside
# an extracted package still defaults to that package root.
LIBRARY_PACKAGE := $(firstword $(patsubst %/include/XianzhiKrylovAPI.h,%,$(wildcard $(REPO_ROOT)/libraries/*/include/XianzhiKrylovAPI.h)))
LIBRARY_ROOT ?= $(if $(XIANZHIKRYLOV_ROOT),$(XIANZHIKRYLOV_ROOT),$(if $(LIBRARY_PACKAGE),$(LIBRARY_PACKAGE),$(abspath $(REPO_ROOT)/..)))

ifeq ($(strip $(MATRIX_FILE)),)
$(error Set MATRIX_FILE in the sample Makefile)
endif
RUN_ARGS ?= $(MATRIX_FILE) $(CONFIG_FILE)

CPPFLAGS += -I"$(LIBRARY_ROOT)/include" -I"$(REPO_ROOT)/common"
LDFLAGS += -L"$(LIBRARY_ROOT)/lib" \
    -Wl,-rpath,"$(LIBRARY_ROOT)/lib" \
    -Wl,--no-as-needed -Wl,--allow-shlib-undefined
LDLIBS += -lXianzhiKrylov -ldl -lm

.PHONY: all check run clean
all: $(TARGET)

check:
	@test -f "$(LIBRARY_ROOT)/include/XianzhiKrylovAPI.h" || { \
	    printf '%s\n' \
	      'XianzhiKrylov headers not found.' \
	      'Place an extracted package under libraries/<package-name>/,' \
	      'or run make with LIBRARY_ROOT=/path/to/package.' >&2; exit 1; }
	@test -f "$(LIBRARY_ROOT)/lib/libXianzhiKrylov.so" || { \
	    printf '%s\n' 'Missing library: $(LIBRARY_ROOT)/lib/libXianzhiKrylov.so' >&2; exit 1; }

$(TARGET): sample.c $(REPO_ROOT)/common/sample_common.h | check
	$(CC) $(CPPFLAGS) $(CFLAGS) sample.c -o $@ $(LDFLAGS) $(LDLIBS)

run: all
	@test -f "$(LIBRARY_ROOT)/license.key" || { \
	    printf '%s\n' \
	      'Missing license: $(LIBRARY_ROOT)/license.key' \
	      'Download the package and license from https://store.solved-tech.com/.' >&2; exit 1; }
	LD_LIBRARY_PATH="$(LIBRARY_ROOT)/lib$(if $(GPU_RUNTIME_LIB_DIR),:$(GPU_RUNTIME_LIB_DIR),)$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
	    ./$(TARGET) $(RUN_ARGS)

clean:
	$(RM) $(TARGET)
