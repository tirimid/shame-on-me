.PHONY: all

PNG_FILES := $(shell find src -name *.png)
PNG_HEADERS := $(patsubst src/%.png,%_png.h,$(PNG_FILES))

all: $(PNG_HEADERS)

%_png.h: src/%.png
	bin2header -o $@ $<
