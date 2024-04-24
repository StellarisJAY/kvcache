DEBUG?=no
TEST?=no
BENCH?=no

SRC_DIR=./src
SRCS=$(wildcard $(SRC_DIR)/*.c)

INCLUDE_DIR=./include
INCLUDES=$(wildcard $(INCLUDE_DIR)/*.c)
DEPS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(INCLUDES)))

TEST_DIR=./test
TESTS=$(wildcard $(TEST_DIR)/*.c)
TEST_OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(TESTS)))

BUILD_DIR=./build
OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
TARGET=kvcache-server

CC=gcc
CFLAGS=-I$(SRC_DIR) -I$(INCLUDE_DIR) -O3

ifeq ($(DEBUG), yes)
CFLAGS+=-g -D DEBUG
endif

ifeq ($(TEST), yes)
CFLAGS+=-I$(TEST_DIR) -D TEST
endif

ifeq ($(BENCH), yes)
CFLAGS+=-D BENCH
endif

$(BUILD_DIR)/$(TARGET): $(OBJS) $(DEPS) $(TEST_OBJS)
	@echo CC -o $@ $^
	@$(CC) -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC -c $<
	@$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: $(INCLUDE_DIR)/%.c
	@echo CC -c $<
	@$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@echo CC -c $<
	@$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)