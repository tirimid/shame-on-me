.PHONY: all

TTF_FILES := $(shell find src -name *.ttf)
TTF_HEADERS := $(patsubst src/%.ttf,%_ttf.h,$(TTF_FILES))

all: $(TTF_HEADERS)

%_ttf.h: src/%.ttf
	xxd -i -n $(notdir $<) $< $@
