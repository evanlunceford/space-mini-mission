CXX      = g++
VCPKG_ROOT ?= C:/vcpkg
VCPKG_INSTALLED = $(CURDIR)/../vcpkg_installed/x64-mingw-static
CXXFLAGS = -Wall -Wextra -std=c++17 -g -I$(VCPKG_INSTALLED)/include
LDFLAGS  = -L$(VCPKG_INSTALLED)/lib -lSDL2main -lSDL2 -lmingw32 -mwindows
TARGET   = mission_sim
BUILD    = build

SRC_DIRS = src src/core src/communication src/rover src/sensors src/launch src/telemetry
SRCS     = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS     = $(patsubst %.cpp,$(BUILD)/%.o,$(SRCS))

.PHONY: all clean run setup

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Isrc -c $< -o $@

run: all
	$(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)

setup:
	cd .. && VCPKG_DEFAULT_HOST_TRIPLET=x64-mingw-static $(VCPKG_ROOT)/vcpkg.exe install --triplet x64-mingw-static
