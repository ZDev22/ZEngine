CC = gcc
# CC = musl-gcc
CFLAGS = -march=native -O3 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -std=c99 -ffast-math -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100
#CFLAGS = -O0 -g3 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -std=c99 -fsanitize=address -DZENGINE_DEBUG -DZENGINE_MAX_SPRITES=1000 -DZENGINE_MAX_TEXTURES=10
LDFLAGS = -lm

BIN := bin/main

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS += /usr/lib/libX11.so /usr/lib/libXrandr.so /usr/lib/libvulkan.so /usr/lib/libpulse.so
else ifeq ($(UNAME_S),Darwin)
	VULKANSDK ?= $(shell ls -d $(HOME)/VulkanSDK/*/macOS)
	CFLAGS += -I$(VULKANSDK)/include
	LDFLAGS += -L$(VULKANSDK)/lib -Wl,-rpath,$(VULKANSDK)/lib -lMoltenVK -lc++ -framework Cocoa -framework Metal -ObjC -framework CoreAudio -framework AudioToolbox
endif

MAKEFLAGS += -j$(shell nproc)

SRC := $(shell find . -name '*.c')
OBJ := $(patsubst %.c,bin/obj/%.o,$(SRC))

VERT_SHADERS := $(wildcard src/shaders/*.vert)
FRAG_SHADERS := $(wildcard src/shaders/*.frag)

VERT_SPV := $(patsubst src/shaders/%.vert,bin/shaders/%.vert.spv,$(VERT_SHADERS))
FRAG_SPV := $(patsubst src/shaders/%.frag,bin/shaders/%.frag.spv,$(FRAG_SHADERS))

SPV_FILES := $(VERT_SPV) $(FRAG_SPV)


all: $(BIN) $(SPV_FILES) copyAssets


$(BIN): $(OBJ)
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

bin/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bin/shaders/%.vert.spv: src/shaders/%.vert
	@mkdir -p bin/shaders
	glslc -O --target-env=vulkan1.2 $< -o $@
bin/shaders/%.frag.spv: src/shaders/%.frag
	@mkdir -p bin/shaders
	glslc -O --target-env=vulkan1.2 $< -o $@

copyAssets:
	@mkdir -p bin/assets
	cp -r src/assets/. bin/assets/

