# Build or clean every sample through the shared common.mk rules.
SAMPLE_DIRS := $(sort $(notdir $(wildcard [0-9][0-9]_*)))
.PHONY: all clean $(SAMPLE_DIRS)
all: $(SAMPLE_DIRS)
$(SAMPLE_DIRS):
	$(MAKE) -C $@ all
clean:
	@set -e; for dir in $(SAMPLE_DIRS); do $(MAKE) -C "$$dir" clean; done
