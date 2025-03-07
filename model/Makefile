.PHONY: all

OBJ_FILES := $(shell find src -name *.obj)
MTL_FILES := $(shell find src -name *.mtl)
OBJ_HEADERS := $(patsubst src/%.obj,%_obj.h,$(OBJ_FILES))
MTL_HEADERS := $(patsubst src/%.mtl,%_mtl.h,$(MTL_FILES))

all: $(OBJ_HEADERS) $(MTL_HEADERS)

%_obj.h: src/%.obj
	bin2header -o $@ $<

%_mtl.h: src/%.mtl
	bin2header -o $@ $<
