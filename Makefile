PROJECT_NAME := app

ZLIB_PATH := $(shell pwd)/zlib
FREETYPE_PATH := $(shell pwd)/freetype
LIBPNG_PATH := $(shell pwd)/libpng
APP_PATH := $(shell pwd)/app
BUILD_PATH := $(shell pwd)/build

LDFLAGS := -Wl,-rpath,'\$$ORIGIN'

SRC_FILES := $(wildcard $(APP_PATH)/*.c)
OBJ_STATIC := $(patsubst $(APP_PATH)/%.c,$(BUILD_PATH)/static/obj/%.o,$(SRC_FILES))
OBJ_DYNAMIC := $(patsubst $(APP_PATH)/%.c,$(BUILD_PATH)/dynamic/obj/%.o,$(SRC_FILES))
OBJ_BLOB := $(patsubst $(APP_PATH)/%.c,$(BUILD_PATH)/blob/obj/%.o,$(SRC_FILES))

TARGET_STATIC := $(BUILD_PATH)/static/app-static
TARGET_DYNAMIC := $(BUILD_PATH)/dynamic/app-dynamic
TARGET_BLOB := $(BUILD_PATH)/blob/app-blob

OBJ_LIBS := $(FREETYPE_PATH)/objs/.libs/libfreetype.a \
    $(LIBPNG_PATH)/.libs/libpng16.a \
	$(ZLIB_PATH)/libz.a \
    -lm

LIBS_H := -I$(ZLIB_PATH) \
    -I$(FREETYPE_PATH)/include \
    -I$(LIBPNG_PATH)

DYN_LIBS := -ldl
DYN_LIBZ := $(ZLIB_PATH)/libz.so
DYN_LIBPNG := $(LIBPNG_PATH)/.libs/libpng16.so
DYN_FREETYPE := $(FREETYPE_PATH)/objs/.libs/libfreetype.so

CC := gcc
CFLAGS := -Wall -Werror -O2

.PHONY: all clean static dynamic libs

all: libs $(TARGET_STATIC) $(TARGET_DYNAMIC) $(TARGET_BLOB)

static: libs $(TARGET_STATIC)

dynamic: libs $(TARGET_DYNAMIC)

blob: libs $(TARGET_BLOB)

libs:
	$(MAKE) -C $(ZLIB_PATH)
	$(MAKE) -C $(FREETYPE_PATH)
	$(MAKE) -C $(LIBPNG_PATH)


$(TARGET_STATIC): $(OBJ_STATIC) $(OBJ_LIBS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ 

$(TARGET_DYNAMIC): $(OBJ_DYNAMIC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(DYN_LIBS)

$(TARGET_BLOB): $(OBJ_BLOB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -nostdlib -pie -fPIE -o $@ $(OBJ_BLOB) $(DYN_LIBS)

$(BUILD_PATH)/static/obj/%.o: $(APP_PATH)/%.c
	$(MAKE) -C $(APP_PATH) LIBS_H="$(LIBS_H)" BUILD_TYPE="static" OBJ_DIR="$(BUILD_PATH)/static/obj" static

$(BUILD_PATH)/dynamic/obj/%.o: $(APP_PATH)/%.c
	$(MAKE) -C $(APP_PATH) LIBS_H="$(LIBS_H)" BUILD_TYPE="dynamic" DYN_LIBZ="$(DYN_LIBZ)" DYN_LIBPNG="$(DYN_LIBPNG)" DYN_FREETYPE="$(DYN_FREETYPE)" OBJ_DIR="$(BUILD_PATH)/dynamic/obj" dynamic

$(BUILD_PATH)/blob/obj/%.o: $(APP_PATH)/%.c
	$(MAKE) -C $(APP_PATH) LIBS_H="$(LIBS_H)" BUILD_TYPE="blob" OBJ_DIR="$(BUILD_PATH)/blob/obj" blob

clean:
	$(MAKE) -C $(APP_PATH) clean
	$(MAKE) -C $(FREETYPE_PATH) clean
	$(MAKE) -C $(ZLIB_PATH) clean
	$(MAKE) -C $(LIBPNG_PATH) clean
	rm -rf $(BUILD_PATH)/static $(BUILD_PATH)/dynamic $(BUILD_PATH)/blob



	
