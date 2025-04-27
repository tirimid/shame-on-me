.PHONY: all

SHADER_FILES := $(shell find src -name *.glsl)
SHADER_HEADERS := $(patsubst src/%.glsl,%_glsl.h,$(SHADER_FILES))

all: $(SHADER_HEADERS)

%_glsl.h: src/%.glsl
	xxd -i -n $(notdir $<) $< $@
