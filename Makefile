# default toolchain for windows
GCC = aarch64-none-elf-gcc
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

SRC = $(wildcard $(SRC_DIR)/*.c)
LIB = $(wildcard $(LIB_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
LIB_OBJ = $(LIB:$(LIB_DIR)/%.c=$(BUILD_DIR)/%.o)

GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude

# nix-shell cross-compile toolchain check
ifneq ($(IN_NIX_SHELL),undefined)
    $(info Using nix-shell cross-compile toolchain)
    GCC = aarch64-unknown-none-elf-gcc                   
    LD = aarch64-unknown-none-elf-ld
    OBJCOPY = aarch64-unknown-none-elf-objcopy
endif

#UNAME_S := $(shell uname -s)
#ifeq ($(UNAME_S),Linux)
#    CCFLAGS += -D LINUX
#endif
#ifeq ($(UNAME_S),Darwin)
#    CCFLAGS += -D OSX
#endif

# default RPI3
all: clean $(BUILD_DIR)/kernel8.img

target_rpi3: GCCFLAGS += -DRPI3
target_rpi4: GCCFLAGS += -DRPI4

target_rpi3: all
target_rpi4: all

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	aarch64-unknown-none-elf-gcc $(GCCFLAGS) -c $(SRC_DIR)/boot.S -o $(BUILD_DIR)/boot.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	aarch64-unknown-none-elf-gcc $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	aarch64-unknown-none-elf-gcc $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel8.img: $(BUILD_DIR)/boot.o $(LIB_OBJ) $(OBJ)
	aarch64-unknown-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(LIB_OBJ) $(OBJ) -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-unknown-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

clean:
	rm -rf $(BUILD_DIR) 
	mkdir $(BUILD_DIR) 

