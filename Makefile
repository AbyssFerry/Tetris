# 项目根目录的Makefile

# 编译器配置
CXX          := g++
TARGET       := game.exe
BUILD_DIR    := obj
SRC_DIR      := Tetris
RES_DIR      := res
INCLUDE_DIRS := include
LIB_DIRS     := lib

# 编译选项
CXXFLAGS := -Wall -O2 -std=c++11 -g -I$(INCLUDE_DIRS)
LDFLAGS  := -L$(LIB_DIRS) -leasyx -lgdi32 -lole32 -luuid -lwinmm -static
LDFLAGS  += -Wl,--subsystem=windows

# 文件配置
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all run clean

all: checkdirs $(TARGET)
	@echo [INFO] Build successful! Execute 'make run' to start the game.

$(TARGET): $(OBJS)
	@echo [LD] $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

checkdirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(RES_DIR)" (echo [ERROR] Missing res directory! && exit 1)

run: $(TARGET)
	@echo [INFO] Starting game...
	@start $(TARGET)

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	@if exist "$(TARGET)" del /Q /F "$(TARGET)"
	@echo [INFO] Clean complete