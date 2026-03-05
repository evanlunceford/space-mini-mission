CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g
TARGET   = mission_sim
BUILD    = build

SRC_DIRS = src src/core src/communication src/rover src/sensors src/launch src/telemetry
SRCS     = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS     = $(patsubst %.cpp,$(BUILD)/%.o,$(SRCS))

.PHONY: all clean run

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD)/%.o: %.cpp
	if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -Isrc -c $< -o $@

run: all
	$(BUILD)/$(TARGET)

clean:
	if exist $(BUILD) rmdir /s /q $(BUILD)
