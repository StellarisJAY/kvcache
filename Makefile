DEBUG?=no

SRC_DIR=./src
SRCS=$(wildcard $(SRC_DIR)/*.c)

INCLUDE_DIR=./include
INCLUDES=$(wildcard $(INCLUDE_DIR)/*.c)
DEPS=$(patsubst %.c, %.o, $(INCLUDES))

BUILD_DIR=./build
OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
TARGET=main

CC=gcc
CFLAGS=-I. -I$(INCLUDE_DIR)

ifeq ($(DEBUG), yes)
CFLAGS+=-g
endif

$(BUILD_DIR)/$(TARGET): $(OBJS) $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(INCLUDE_DIR)/%.o: $(INCLUDE_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(INCLUDE_DIR)/*.o