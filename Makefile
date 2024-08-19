# default toolchain for windows
GCC = aarch64-none-elf-gcc
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

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

# build final image
$(BUILD_DIR)/kernel8.img: $(BUILD_DIR)/boot.o $(LIB_OBJ) $(UTIL_OBJ) $(CLI_OBJ) $(OBJ)
	$(LD) -nostdlib $(BUILD_DIR)/boot.o $(LIB_OBJ) $(UTIL_OBJ) $(CLI_OBJ) $(OBJ) -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel8.elf kernel8.img
	@echo build successfully for $(if $(filter 1,$(RPI4)),RPI4,RPI3) $(if $(filter 1,$(UART1)),UART1,UART0)

clean:
	-$(RM) $(BUILD_DIR)\kernel8.elf
	-$(RM) $(BUILD_DIR)\*.o
	-$(RM) *.img

run:
	qemu-system-aarch64 -M $(if $(filter 1,$(RPI4)),raspi4b,raspi3b) -kernel kernel8.img $(if $(filter 1,$(UART1)),-serial null -serial stdio,-serial stdio)
