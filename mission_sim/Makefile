CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = mission_sim
BUILD   = build

SRC_DIRS = src src/core src/communication src/rover src/sensors src/launch src/telemetry
SRCS     = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS     = $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

.PHONY: all clean run

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Isrc -c $< -o $@

run: all
	./$(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)
