CC = gcc
CFLAGS = -march=native -O3 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -std=c99 -ffast-math
#CFLAGS = -O0 -g -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -std=c99 -fsanitize=address -DZENGINE_DEBUG
LDFLAGS = -lm -lpulse

BIN := bin/main

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lX11 -lXrandr -lvulkan
else ifeq ($(UNAME_S),Darwin)
	VULKANSDK ?= $(HOME)/VulkanSDK/current/macOS
	export VK_ICD_FILENAMES := $(VULKANSDK)/share/vulkan/icd.d/MoltenVK_icd.json
	CFLAGS += -I$(VULKANSDK)/include
	LDFLAGS += -L$(VULKANSDK)/lib -Wl,-rpath,$(VULKANSDK)/lib -lMoltenVK -lc++ -framework Cocoa -framework Metal -ObjC
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

