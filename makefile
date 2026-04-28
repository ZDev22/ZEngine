CC = gcc
GLSLC = glslc

#CFLAGS = -march=westmere -O3 -Weverything -Wno-maybe-uninitialized -Wno-declaration-after-statement -Wno-poison-system-directories -Wno-padded -Wno-missing-noreturn -Wno-bad-function-cast -Wno-float-conversion -Wno-double-promotion -Wno-pedantic -std=c99
CFLAGS = -march=westmere -O3 -Wall -Wextra -Wno-maybe-uninitialized -std=c99 -flto
LDFLAGS = -lvulkan -lm -lpthread

BIN := build/main

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lX11 -lXrandr
endif

ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -lCocoa -lMetal
endif

MAKEFLAGS += -j$(shell nproc)

SRC := $(shell find . -name '*.c')
OBJ := $(patsubst %.c,build/obj/%.o,$(SRC))

VERT_SHADERS := $(wildcard src/shaders/*.vert)
FRAG_SHADERS := $(wildcard src/shaders/*.frag)

VERT_SPV := $(patsubst src/shaders/%.vert,build/shaders/%.vert.spv,$(VERT_SHADERS))
FRAG_SPV := $(patsubst src/shaders/%.frag,build/shaders/%.frag.spv,$(FRAG_SHADERS))

SPV_FILES := $(VERT_SPV) $(FRAG_SPV)


# NEW FUNCTIONS GO HERE ----
all: $(BIN) $(SPV_FILES) copyAssets


$(BIN): $(OBJ)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/shaders/%.vert.spv: src/shaders/%.vert
	@mkdir -p build/shaders
	$(GLSLC) $< -o $@
build/shaders/%.frag.spv: src/shaders/%.frag
	@mkdir -p build/shaders
	$(GLSLC) $< -o $@

copyAssets:
	@mkdir -p build/assets
	cp -r src/assets/. build/assets/

