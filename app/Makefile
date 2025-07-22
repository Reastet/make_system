APP_SRC := $(wildcard *.c)
APP_OBJ := $(patsubst %.c, $(OBJ_DIR)/%.o, $(APP_SRC))

CC := gcc
CFLAGS ?= -Wall -Werror -O2

LIBS_H ?=
BUILD_TYPE ?=
DYN_LIBPNG ?=
DYN_FREETYPE ?=
DYN_LIBZ ?=
FFLAGS =

OBJ_DIR ?= ./obj

ifneq ($(MAKECMDGOALS),clean)
ifeq ($(BUILD_TYPE), static)
CFLAGS += -DSTATIC_BUILD
$(info STATIC_BUILD!!!!!!!!!!!!!!)
else ifeq ($(BUILD_TYPE), dynamic)
CFLAGS += -DDYNAMIC_BUILD
FFLAGS += -DDYN_FREETYPE=\"$(DYN_FREETYPE)\"
FFLAGS += -DDYN_LIBPNG=\"$(DYN_LIBPNG)\"
FFLAGS += -DDYN_LIBZ=\"$(DYN_LIBZ)\"
$(info DYNAMIC BUILD!!!!!!!!!!!!!!)
else ifeq ($(BUILD_TYPE), blob)
CFLAGS += -DBLOB_BUILD -nostdlib -pie -fPIE
$(info BLOB BUILD!!!!!!!!!!!!!!)
else
$(info ANOTHER BUILD!!!!!!!!!!!!!!)
endif
endif


$(OBJ_DIR)/%.o: %.c
	@echo "LIBS_H: $(LIBS_H)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(FFLAGS) $(LIBS_H) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o

.PHONY: all clean static dynamic

all: static dynamic

static: $(APP_OBJ)

dynamic: $(APP_OBJ)

blob: $(APP_OBJ)
