# BUILD_HW= rxlamp soptunna 5strip
SHELL= /bin/bash
# ifndef BUILD_HW
# $(error You have to specify what hardware to build!)
# endif

BUILD_DIR= build/

PORT?= /dev/ttyUSB0

SOURCES= main.c systick.c pwm.c uart.c protocol.c
OBJECTS= $(SOURCES:%.c=$(BUILD_DIR)%.o)
# BUILD_OBJECTS= $(OBJECTS) $(BUILD_HW:%=$(BUILD_DIR)hwprofile_%.o)
BUILD_OBJECTS= $(OBJECTS)

BUILD_BIN= $(BUILD_HW:%=$(BUILD_DIR)%.bin)
BUILD_ELF= $(BUILD_HW:%=$(BUILD_DIR)%.elf)

LIBS= libopencm3_stm32f1.a
LIB_DIRS= lib/libopencm3/lib/stm32/f1 lib/libopencm3/lib

INCLUDE_PATHS+= -Ilib/libopencm3/include -Iinc

CPU_FLAGS= -mthumb -mcpu=cortex-m3 -msoft-float -DSTM32F1
COMPILE_FLAGS= -c -std=gnu99 -Os -Wall -fno-common -ffunction-sections -fdata-sections

UPLOAD_COMMANDS= $(BUILD_HW:%=%_upload)

LINK_SCRIPT= stm32f100x6.ld
LINK_FLAGS= $(CPU_FLAGS)
LINK_FLAGS+= $(LIB_DIRS:%=-L%)
LINK_FLAGS+= --static -nostartfiles
LINK_FLAGS+= -T$(LINK_SCRIPT)
LINK_FLAGS+= -Wl,--start-group $(LIBS:lib%.a=-l%)  -lc -lgcc -lnosys -Wl,--end-group

CFLAGS= $(COMPILE_FLAGS)
CFLAGS+= $(CPU_FLAGS)
CFLAGS+= $(INCLUDE_PATHS)

TOOLCHAIN = arm-none-eabi

CC = $(TOOLCHAIN)-gcc
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump
SIZE = $(TOOLCHAIN)-size



vpath %.c src
vpath %.a lib/libopencm3/lib
vpath %.ld $(LIB_DIRS)

all: $(BUILD_HW)

$(BUILD_HW): %: $(BUILD_DIR)%.bin
	@echo $@ complete!

$(BUILD_BIN): %.bin: %.elf
	$(OBJCOPY) -O binary $^ $@
	$(SIZE) --format=berkeley $^
	
libopencm3: libopencm3_stm32f1.a

libopencm3_stm32f1.a: lib/libopencm3/.git
	cd lib/libopencm3 && $(MAKE) lib/stm32/f1

lib/libopencm3/.git:
	cd lib/libopencm3 && git submodule init
	cd lib/libopencm3 && git submodule update

$(BUILD_ELF): $(BUILD_OBJECTS) $(LIBS) $(BUILD_HW:%=$(BUILD_DIR)hwprofile_%.o)
	$(CC) $(OBJECTS) $(@:$(BUILD_DIR)%.elf=$(BUILD_DIR)hwprofile_%.o) $(LINK_FLAGS) -o $@ 2> >(./cleanup.py)

$(BUILD_HW:%=$(BUILD_DIR)hwprofile_%.o): $(BUILD_HW:%=hwprofile_%.c)
	$(CC) $(CFLAGS) $^ -o $@ -DLAMP_ID=$(LAMP_ID) 2> >(./cleanup.py)


$(BUILD_OBJECTS): $(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ 2> >(./cleanup.py)



upload: $(BUILD_BIN)
	stm32flash $(PORT) -w $^ -b 1000000 -R cs.rts -B cs.dtr

clean:
	rm -f $(BUILD_ELF) $(BUILD_BIN) $(OBJECTS)

testrigg: test_rxlamp test_5strip

test_rxlamp:
	$(MAKE) LAMP_ID=0x1 BUILD_HW=rxlamp PORT=/dev/ttyUSB1 upload

test_5strip:
	$(MAKE) LAMP_ID=0x2 BUILD_HW=5strip PORT=/dev/ttyUSB0 upload

.PHONY: testrigg test_rxlamp test_5strip rxlamp soptunna 5strip libopencm3 $(BUILD_HW:%=$(BUILD_DIR)hwprofile_%.o)
