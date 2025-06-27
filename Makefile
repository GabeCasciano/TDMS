# CXX = g++
# CXXFLAGS = -std=c++20 -O2 -Wall -Iinclude
#
# BUILD_DIR = build
# BIN_DIR = bin
#
# TARGET = $(BIN_DIR)/tdms_writer_demo
# OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
#
# all: $(TARGET)
#
# $(TARGET): $(OBJ)
# 	@mkdir -p $(BIN_DIR)
# 	$(CXX) $(CXXFLAGS) -o $@ $^
#
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@mkdir -p $(BUILD_DIR)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
#
# bear:
# 	bear -- make clean all
#
# clean:
# 	rm -rf $(BUILD_DIR) $(BIN_DIR)
#

CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Iinclude

SRC_DIRS = src src/TDMS
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/tdms_writer_demo

# All .cpp files
SRC = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))

# Map source files to build/*.o paths
OBJ = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC))

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp into its matching .o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Print debug info
info:
	@echo "SRC = $(SRC)"
	@echo "OBJ = $(OBJ)"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# For clangd
bear:
	bear -- make clean all
