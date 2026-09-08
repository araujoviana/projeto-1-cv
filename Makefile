# Projeto 1 - Computacao Visual (UPM FCI CC) - 2026.2

TARGET  := projeto-1-cv
SRC_DIR := src
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CC      := gcc
CFLAGS  := -std=c23 -Wall -Wextra -Wpedantic -MMD -MP
LDFLAGS :=
LDLIBS  :=

# SDL3 via pkg-config (fallback para -lSDL3 caso pkg-config nao encontre).
SDL_CFLAGS := $(shell pkg-config --cflags sdl3 2>/dev/null)
SDL_LIBS   := $(shell pkg-config --libs sdl3 2>/dev/null)
ifeq ($(strip $(SDL_LIBS)),)
SDL_LIBS := -lSDL3
endif

CFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS)

# make DEBUG=1 para build com simbolos e sanitizers.
ifeq ($(DEBUG),1)
CFLAGS  += -g -O0 -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined
else
CFLAGS  += -O2
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
