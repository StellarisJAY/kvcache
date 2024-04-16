DEBUG?=no
TEST?=no

SRC_DIR=./src
SRCS=$(wildcard $(SRC_DIR)/*.c)

INCLUDE_DIR=./include
INCLUDES=$(wildcard $(INCLUDE_DIR)/*.c)
DEPS=$(patsubst %.c, %.o, $(INCLUDES))

TEST_DIR=./test
TESTS=$(wildcard $(TEST_DIR)/*.c)
TEST_OBJS=$(patsubst %.c, %.o, $(TESTS))

BUILD_DIR=./build
OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
TARGET=main

CC=gcc
CFLAGS=-I$(SRC_DIR) -I$(INCLUDE_DIR)

ifeq ($(DEBUG), yes)
CFLAGS+=-g
endif

ifeq ($(TEST), yes)
CFLAGS+=-I$(TEST_DIR)
endif

$(BUILD_DIR)/$(TARGET): $(OBJS) $(DEPS) $(TESTS)
	$(CC) -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(INCLUDE_DIR)/%.o: $(INCLUDE_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(INCLUDE_DIR)/*.o
	rm -f $(TEST_DIR)/*.o