# Build this sample against an existing XianzhiKrylov package.
# XK_ROOT defaults to the package containing samples/.
CC ?= gcc
CFLAGS ?= -O3 -std=c11 -Wall -Wextra
XK_ROOT ?= $(if $(XIANZHIKRYLOV_ROOT),$(XIANZHIKRYLOV_ROOT),$(abspath $(CURDIR)/../..))
CUDA_ROOT ?= /usr/local/cuda
CUDA_LIB_DIR ?= $(if $(wildcard $(CUDA_ROOT)/lib64),$(CUDA_ROOT)/lib64,$(CUDA_ROOT)/lib)
RUN_ARGS ?= data/diffusion_1m.csrpat solver.json
TARGET := sample

CPPFLAGS += -I"$(XK_ROOT)/include" -I"$(CUDA_ROOT)/include"
LDFLAGS += -L"$(XK_ROOT)/lib" -L"$(CUDA_LIB_DIR)" \
    -Wl,-rpath,'$$ORIGIN/../../lib' -Wl,-rpath,"$(XK_ROOT)/lib" \
    -Wl,-rpath,"$(CUDA_LIB_DIR)" \
    -Wl,--no-as-needed -Wl,--allow-shlib-undefined
LDLIBS += -lXianzhiKrylov -lcudart -ldl -lm

.PHONY: all check run clean
all: $(TARGET)

check:
	@test -f "$(XK_ROOT)/include/XianzhiKrylovAPI.h" || { \
	    printf '%s\n' 'Set XK_ROOT to the extracted XianzhiKrylov package.' >&2; exit 1; }
	@test -f "$(XK_ROOT)/lib/libXianzhiKrylov.so" || { \
	    printf '%s\n' 'Missing library: lib/libXianzhiKrylov.so' >&2; exit 1; }
	@test -f "$(CUDA_ROOT)/include/cuda_runtime.h" || { \
	    printf '%s\n' 'Set CUDA_ROOT to a compatible CUDA development toolkit.' >&2; exit 1; }
	@test -f "$(CUDA_LIB_DIR)/libcudart.so" || { \
	    printf '%s\n' 'Set CUDA_LIB_DIR to the directory containing libcudart.so.' >&2; exit 1; }

$(TARGET): sample.c sample_common.h | check
	$(CC) $(CPPFLAGS) $(CFLAGS) sample.c -o $@ $(LDFLAGS) $(LDLIBS)

run: all
	LD_LIBRARY_PATH="$(XK_ROOT)/lib:$(CUDA_LIB_DIR)$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
	    ./$(TARGET) $(RUN_ARGS)

clean:
	$(RM) $(TARGET)
