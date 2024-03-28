CXX := g++
LD := g++

TARGET_EXEC := main

SRC_DIR := src
EXT_DIR := ext

BIN_DIR := bin
BUILD_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.cc' -or -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXX_FLAGS := -Wall -Wpedantic -Werror -g -O3 -I$(SRC_DIR) -I$(EXT_DIR)/graphics/src -MMD -MP
CC_FLAGS := -O3 -I$(SRC_DIR) -MMD -MP
LD_FLAGS := -L$(EXT_DIR)/graphics/lib
LIB_FLAGS :=  -l:libgraphics.a -lglfw -lGL -lX11

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS) $(EXT_DIR)/graphics/lib/libgraphics.a
	@mkdir -p $(dir $@)
	$(LD) $(LD_FLAGS) -o $@ $(OBJS) $(LIB_FLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

$(EXT_DIR)/graphics/lib/libgraphics.a:
	$(MAKE) -C $(EXT_DIR)/graphics

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

deepclean: clean
	$(MAKE) -C $(EXT_DIR)/graphics clean

-include $(DEPS)
