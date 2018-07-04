
include subMakefile.mk

CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

TOP_DIR     := $(shell pwd)
BIN_DIR     := $(TOP_DIR)/BIN
OBJ_DIR     := $(TOP_DIR)/OBJ
INCLUDEDIR 	:= $(TOP_DIR)/INCLUDE
TEMP_DIR 	:= $(TOP_DIR)/TEMP
BIN         := AllModuleTestsofS5PV210.bin
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

LDS         := AllModuleTestsofS5PV210.lds
ELF         := AllModuleTestsofS5PV210.elf
DIS         := AllModuleTestsofS5PV210.dis

LDFLASG := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

export CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS AR LDFLASG hardware_obj_c
export TOP_DIR BIN_DIR OBJ_DIR BIN TEMP_DIR LDS ELF DIS objs_S objs_c  SUB_DIR


all: CHECKDIR $(SUB_DIR) BIN_PROCESS

CHECKDIR:
	mkdir -p  $(BIN_DIR)  $(TEMP_DIR)

$(SUB_DIR):ECHO
	make -C $@
	
ECHO:
	@echo $(OBJ_DIR)
	@echo begin compile ...
	
BIN_PROCESS:
	make -C $(OBJ_DIR)
	@echo program successfully!
	
.PHONY :clean
	
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR) $(TEMP_DIR)
	@echo remove successfully!
	