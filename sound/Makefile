.PHONY: all

OGG_FILES := $(shell find src -name *.ogg)
OGG_HEADERS := $(patsubst src/%.ogg,%_ogg.h,$(OGG_FILES))

all: $(OGG_HEADERS)

%_ogg.h: src/%.ogg
	xxd -i -n $(notdir $<) $< $@
