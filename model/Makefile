.PHONY: all

OBJ_FILES := $(shell find src -name *.obj)
OBJ_HEADERS := $(patsubst src/%.obj,%_obj.h,$(OBJ_FILES))

all: $(OBJ_HEADERS)

%_obj.h: src/%.obj
	python3 gen-model.py -m $< $@
