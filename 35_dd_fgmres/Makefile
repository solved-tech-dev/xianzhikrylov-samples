# Build this sample against an existing XianzhiKrylov package.
# XK_ROOT defaults to the package containing samples/.
CC ?= gcc
CFLAGS ?= -O3 -std=c11 -Wall -Wextra
XK_ROOT ?= $(if $(XIANZHIKRYLOV_ROOT),$(XIANZHIKRYLOV_ROOT),$(abspath $(CURDIR)/../..))
XK_BACKEND ?= 1
GPU_RUNTIME_LIB_DIR ?=
RUN_ARGS ?= data/diffusion_1m.csrpat solver.json
TARGET := sample

CPPFLAGS += -I"$(XK_ROOT)/include" -DXK_SAMPLE_BACKEND=$(XK_BACKEND)
LDFLAGS += -L"$(XK_ROOT)/lib" \
    -Wl,-rpath,'$$ORIGIN/../../lib' -Wl,-rpath,"$(XK_ROOT)/lib" \
    -Wl,--no-as-needed -Wl,--allow-shlib-undefined
LDLIBS += -lXianzhiKrylov -ldl -lm

.PHONY: all check run clean
all: $(TARGET)

check:
	@test -f "$(XK_ROOT)/include/XianzhiKrylovAPI.h" || { \
	    printf '%s\n' 'Set XK_ROOT to the extracted XianzhiKrylov package.' >&2; exit 1; }
	@test -f "$(XK_ROOT)/lib/libXianzhiKrylov.so" || { \
	    printf '%s\n' 'Missing library: lib/libXianzhiKrylov.so' >&2; exit 1; }

$(TARGET): sample.c sample_common.h | check
	$(CC) $(CPPFLAGS) $(CFLAGS) sample.c -o $@ $(LDFLAGS) $(LDLIBS)

run: all
	@! grep -q '/absolute/path/to/' solver.json || { \
	    printf '%s\n' 'Edit solver.json: set LinglongAMG library_path and config_path.' >&2; exit 1; }
	LD_LIBRARY_PATH="$(XK_ROOT)/lib$(if $(GPU_RUNTIME_LIB_DIR),:$(GPU_RUNTIME_LIB_DIR),)$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
	    ./$(TARGET) $(RUN_ARGS)

clean:
	$(RM) $(TARGET)
