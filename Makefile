# default toolchain for windows
GCC = aarch64-none-elf-gcc
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

SRC_DIR = src
BUILD_DIR = build
BUILD_DIR_CACHE = build/cache
LIB_DIR = lib
ASSETS_DIR = assets

# library
LIB = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ = $(LIB:$(LIB_DIR)/%.c=$(BUILD_DIR)/%.o)

# main kernel
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

UTIL = $(wildcard $(SRC_DIR)/util/*c)
UTIL_OBJ = $(UTIL:$(SRC_DIR)/util/%.c=$(BUILD_DIR)/%.o)

CLI = $(wildcard $(SRC_DIR)/cli/*c)
CLI_OBJ = $(CLI:$(SRC_DIR)/cli/%.c=$(BUILD_DIR)/%.o)

VIDEO_PLAYER = $(wildcard $(SRC_DIR)/video-player/*c)
VIDEO_PLAYER_OBJ = $(VIDEO_PLAYER:$(SRC_DIR)/video-player/%.c=$(BUILD_DIR)/%.o)

GAME = $(wildcard $(SRC_DIR)/game/*c)
GAME_OBJ = $(GAME:$(SRC_DIR)/game/%.c=$(BUILD_DIR)/%.o)

ASSETS_C = $(wildcard $(ASSETS_DIR)/*c)
ASSETS_OBJ = $(ASSETS_C:$(ASSETS_DIR)/%.c=$(BUILD_DIR_CACHE)/%.o)

GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude

# nix-shell cross-compile toolchain check
# ifneq ($(IN_NIX_SHELL),undefined)
#     $(info Using nix-shell cross-compile toolchain)
#     GCC = aarch64-unknown-none-elf-gcc                   
#     LD = aarch64-unknown-none-elf-ld
#     OBJCOPY = aarch64-unknown-none-elf-objcopy
# endif

# default clean for windows
RM = del

UNAME_S := $(shell uname -s)
# linux clean
ifeq ($(UNAME_S),Linux)
	RM = rm
endif
# macOS clean
ifeq ($(UNAME_S),Darwin)
	RM = rm
endif

# uart target
UART1 = 1

# raspberry target
RPI4 = 1

# default compiles to RPI4 and UART1
all: clean $(BUILD_DIR)/kernel8.img run

use_uart0:
	$(eval UART1 = 0)
use_uart0:
	$(eval UART1 = 0)
use_rpi3:
	$(eval RPI4 = 0)
use_rpi4:
	$(eval RPI4 = 0)

r3_u0: GCCFLAGS += -DRPI3 -DUART0
r3_u1: GCCFLAGS += -DRPI3 -DUART1
r4_u0: GCCFLAGS += -DRPI4 -DUART0
r4_u1: GCCFLAGS += -DRPI4 -DUART1

r3_u0: use_uart0 use_rpi3 all
r3_u1: use_rpi3 all
r4_u0: use_uart0 all
r4_u1: all

# compile to boot.S
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	$(GCC) $(GCCFLAGS) -c $(SRC_DIR)/boot.S -o $(BUILD_DIR)/boot.o

# compile src dir
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# compile lib dir
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# compile util
$(BUILD_DIR)/%.o: $(SRC_DIR)/util/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# compile project cli
$(BUILD_DIR)/%.o: $(SRC_DIR)/cli/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# compile video player
$(BUILD_DIR)/%.o: $(SRC_DIR)/video-player/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# compile game
$(BUILD_DIR)/%.o: $(SRC_DIR)/game/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

# assets - declare each target for caching
$(BUILD_DIR_CACHE)/img-background.o: $(ASSETS_DIR)/img-background.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/video.o: $(ASSETS_DIR)/video.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/block.o: $(ASSETS_DIR)/block.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/gameOverPopUp.o: $(ASSETS_DIR)/gameOverPopUp.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/gameWelcomeScreen.o: $(ASSETS_DIR)/gameWelcomeScreen.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/helpScreen.o: $(ASSETS_DIR)/helpScreen.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR_CACHE)/playScreen.o: $(ASSETS_DIR)/playScreen.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
all_assets_target:$(BUILD_DIR_CACHE)/img-background.o $(BUILD_DIR_CACHE)/video.o $(BUILD_DIR_CACHE)/block.o $(BUILD_DIR_CACHE)/gameOverPopUp.o $(BUILD_DIR_CACHE)/gameWelcomeScreen.o $(BUILD_DIR_CACHE)/helpScreen.o $(BUILD_DIR_CACHE)/playScreen.o

# build final image
$(BUILD_DIR)/kernel8.img: $(BUILD_DIR)/boot.o $(LIB_OBJ) $(UTIL_OBJ) $(CLI_OBJ) $(VIDEO_PLAYER_OBJ) $(GAME_OBJ) $(OBJ) all_assets_target
	$(LD) -nostdlib $(BUILD_DIR)/boot.o $(LIB_OBJ) $(UTIL_OBJ) $(CLI_OBJ) $(VIDEO_PLAYER_OBJ) $(GAME_OBJ) $(OBJ) $(ASSETS_OBJ) -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel8.elf kernel8.img
	@echo build successfully for $(if $(filter 1,$(RPI4)),RPI4,RPI3) $(if $(filter 1,$(UART1)),UART1,UART0)

clean:
	-$(RM) $(BUILD_DIR)\kernel8.elf
	-$(RM) $(BUILD_DIR)\*.o
	-$(RM) *.img

run:
	qemu-system-aarch64 -M $(if $(filter 1,$(RPI4)),raspi4b,raspi3b) -kernel kernel8.img $(if $(filter 1,$(UART1)),-serial null -serial stdio,-serial stdio)
