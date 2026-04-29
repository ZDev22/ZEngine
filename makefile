CC = gcc
#CFLAGS = -Og -Weverything -Wno-maybe-uninitialized -Wno-declaration-after-statement -Wno-poison-system-directories -Wno-padded -Wno-missing-noreturn -Wno-bad-function-cast -Wno-float-conversion -Wno-double-promotion -Wno-pedantic -std=c99
CFLAGS = -march=westmere -O3 -Wall -Wextra -Wno-maybe-uninitialized -std=c99 -flto
LDFLAGS = -lm -lpthread

BIN := build/main

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Windows_NT)
	LDFLAGS += -lvulkan
endif

ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lX11 -lXrandr -lvulkan
endif

ifeq ($(UNAME_S),Darwin)
	VULKANSDK ?= $(HOME)/VulkanSDK/current/macOS
	export VK_ICD_FILENAMES := $(VULKANSDK)/share/vulkan/icd.d/MoltenVK_icd.json
	CFLAGS += -I$(VULKANSDK)/include
	LDFLAGS += -L$(VULKANSDK)/lib -Wl,-rpath,$(VULKANSDK)/lib -lMoltenVK -lc++ -framework Cocoa -framework Metal -ObjC
endif

MAKEFLAGS += -j$(shell nproc)

SRC := $(shell find . -name '*.c')
OBJ := $(patsubst %.c,build/obj/%.o,$(SRC))

VERT_SHADERS := $(wildcard src/shaders/*.vert)
FRAG_SHADERS := $(wildcard src/shaders/*.frag)

VERT_SPV := $(patsubst src/shaders/%.vert,build/shaders/%.vert.spv,$(VERT_SHADERS))
FRAG_SPV := $(patsubst src/shaders/%.frag,build/shaders/%.frag.spv,$(FRAG_SHADERS))

SPV_FILES := $(VERT_SPV) $(FRAG_SPV)


all: $(BIN) $(SPV_FILES) copyAssets


$(BIN): $(OBJ)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/shaders/%.vert.spv: src/shaders/%.vert
	@mkdir -p build/shaders
	glslc $< -o $@
build/shaders/%.frag.spv: src/shaders/%.frag
	@mkdir -p build/shaders
	glslc $< -o $@

copyAssets:
	@mkdir -p build/assets
	cp -r src/assets/. build/assets/

