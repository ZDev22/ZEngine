CC = gcc
#CC = clang
CFLAGS = -march=native -mtune=native -Ofast -flto -fomit-frame-pointer -fprefetch-loop-arrays -fno-semantic-interposition -fno-plt -fipa-pta -fipa-icf -fira-loop-pressure -fira-region=all -fwhole-program -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -Wno-missing-field-initializers -fvect-cost-model=unlimited -fsimd-cost-model=unlimited -std=c99 -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100 -DTRACK_FPS
#CFLAGS = -O0 -g3 -Wall -Wextra -Wno-maybe-uninitialized -Wno-override-init -Wno-missing-field-initializers -std=c99 -fsanitize=address -DZENGINE_DEBUG -DZENGINE_ENABLE_VALIDATION_LAYERS -DTRACK_FPS -DZENGINE_MAX_SPRITES=10000 -DZENGINE_MAX_TEXTURES=100
LDFLAGS = -lm

BIN := bin/main
LDFLAGS += -lX11 -lXrandr -lXext -lXcursor -lXi -lvulkan -lpulse
MAKEFLAGS += -j$(shell nproc)

SRC := $(shell find . -name '*.c')
SRC += $(shell find . -name '*.cpp')
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
