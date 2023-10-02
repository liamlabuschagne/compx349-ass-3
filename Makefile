###############################################################################
#
# Application name
#
###############################################################################

APP_NAME = blinky


###############################################################################
#
# Application sources
#
###############################################################################

SRCS_C =  \
    src/blinky.c

SRCS_H =


###############################################################################
#
# 'all' rule has to be the first rule so that it is run by default
#
###############################################################################

BUILD_DIR ?= build

TARGET = $(BUILD_DIR)/$(APP_NAME)
.PHONY: all
all: $(TARGET).elf $(TARGET).hex
	$(SIZE) $<

###############################################################################
#
# Vendor specific code
#
###############################################################################

SRCS_C +=  \
    src/vendor/mdk/system_nrf52833.c \
    src/vendor/mdk/gcc_startup_nrf52833.S

SRCS_H +=  \
    src/vendor/mdk/

LINKER_SCRIPTS +=  \
    src/vendor/mdk/nrf52833_xxaa.ld

INCLUDE_PATH +=  \
    src/vendor/mdk/ \
    src/vendor/cmsis/include

LDFLAGS += \
    -L src/vendor/mdk

BUILD_DEFINES += -DNRF52833_XXAA
BUILD_DEFINES += -D__LINT__=0

###############################################################################
#
# Utilities
#
###############################################################################

comma:=,

# When building on Cygwin/Windows we're generally faced with
# toolchains which require native paths (e.g., C:\... rather than
# /cygdrive/c/...). We here define a function to perform the mapping
# on Windows, but leave the paths alone otherwise.
ifdef COMSPEC
native-path = $(shell cygpath -m $1)
else
native-path = $1
endif


###############################################################################
#
# Build environment
#
###############################################################################

CFLAGS += -mcpu=cortex-m4 -mthumb -W -g3 -Os -fno-common -ffunction-sections -fdata-sections -Wno-unused-parameter

CFLAGS_WARN_ERROR += -Wall -Wundef -Wpointer-arith -Werror -Wfatal-errors

LDFLAGS += -Wl,--gc-sections -Wl,--start-group -lgcc -lc -Wl,--end-group $(addprefix -Wl$(comma)-T$(comma), $(call native-path,$(LINKER_SCRIPTS)))

INCLUDE_PATH_FLAGS=$(addprefix -I, $(call native-path,$(INCLUDE_PATH)))


###############################################################################
#
# Toolchain configuration
#
###############################################################################
TOOLCHAIN_BASE ?= arm-none-eabi-
CC  = $(TOOLCHAIN_BASE)gcc
CPP = $(TOOLCHAIN_BASE)g++
AS  = $(TOOLCHAIN_BASE)as
LD  = $(TOOLCHAIN_BASE)ld
OBJCOPY = $(TOOLCHAIN_BASE)objcopy
SIZE = $(TOOLCHAIN_BASE)size
MKDIR ?= mkdir


###############################################################################
#
# Rules
#
###############################################################################

OBJS_ = $(patsubst src/%, $(BUILD_DIR)/%, $(SRCS_C) $(SRCS_S))

OBJS =  $(patsubst %.c, %.o,   \
        $(patsubst %.cc, %.o,  \
        $(patsubst %.cpp, %.o, \
        $(patsubst %.s, %.o,   \
        $(patsubst %.S, %.o,   \
        $(OBJS_))))))


$(BUILD_DIR)/%.o : src/%.c $(SRCS_H)
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_WARN_ERROR) $(BUILD_DEFINES) $(INCLUDE_PATH_FLAGS) -c -o $(call native-path,$@ $<)

$(BUILD_DIR)/%.o : src/%.cc $(SRCS_H)
	$(MKDIR) -p $(dir $@)
	$(CPP) $(CFLAGS) $(CFLAGS_WARN_ERROR) $(BUILD_DEFINES) $(INCLUDE_PATH_FLAGS) -c -o $(call native-path,$@ $<)

$(BUILD_DIR)/%.o : src/%.cpp $(SRCS_H)
	$(MKDIR) -p $(dir $@)
	$(CPP) $(CFLAGS) $(CFLAGS_WARN_ERROR) $(BUILD_DEFINES) $(INCLUDE_PATH_FLAGS) -c -o $(call native-path,$@ $<)

$(BUILD_DIR)/%.o : src/%.s $(SRCS_H)
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_WARN_ERROR) $(BUILD_DEFINES) $(INCLUDE_PATH_FLAGS) -c -o $(call native-path,$@ $<)

$(BUILD_DIR)/%.o : src/%.S $(SRCS_H)
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_WARN_ERROR) $(BUILD_DEFINES) $(INCLUDE_PATH_FLAGS) -c -o $(call native-path,$@ $<)

$(TARGET).elf : $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(call native-path,$(OBJS) $(LIBS)) -o $(call native-path,$@)

%.bin : %.elf
	$(OBJCOPY) -O binary $(call native-path,$< $@)

%.hex : %.elf
	$(OBJCOPY) -O ihex $(call native-path,$< $@)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

