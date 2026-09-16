# Projeto 1 - Computacao Visual (UPM FCI CC) - 2026.2

SRC_DIR   := src
BUILD_DIR := build
INC_DIR   := include
LIB_DIR   := lib
BIN_DIR   := bin

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CC      := gcc
CFLAGS  := -std=c23 -Wall -Wextra -Wpedantic -Werror -MMD -MP -I$(INC_DIR)
LDFLAGS :=
LDLIBS  :=

# make DEBUG=1 para build com simbolos e sanitizers.
ifeq ($(DEBUG),1)
CFLAGS  += -g -O0 -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined
else
CFLAGS  += -O2
endif

ifeq ($(OS),Windows_NT)
TARGET  := projeto-1-cv.exe
LDLIBS  += $(LIB_DIR)/SDL3.dll $(LIB_DIR)/SDL3_image.dll $(LIB_DIR)/SDL3_ttf.dll

# Forca cmd.exe para as receitas abaixo (sintaxe "if not exist"/"del"/"rmdir"),
# independente de haver um sh.exe em algum PATH (ex.: Git for Windows instalado
# a parte) que o make pegaria por padrao em vez do cmd.
SHELL := cmd.exe

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@if not exist SDL3.dll copy /Y $(BIN_DIR)\SDL3.dll . >nul
	@if not exist SDL3_image.dll copy /Y $(BIN_DIR)\SDL3_image.dll . >nul
	@if not exist SDL3_ttf.dll copy /Y $(BIN_DIR)\SDL3_ttf.dll . >nul

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

run: $(TARGET)
	$(TARGET)

clean:
	@if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	@if exist $(TARGET) del /Q /F $(TARGET)
	@if exist SDL3.dll del /Q /F SDL3.dll
	@if exist SDL3_image.dll del /Q /F SDL3_image.dll
	@if exist SDL3_ttf.dll del /Q /F SDL3_ttf.dll

else
TARGET  := projeto-1-cv

# SDL3 + SDL_image + SDL_ttf via pkg-config (fallback para as bibliotecas
# vendorizadas em lib/ quando pkg-config nao encontra os modulos; os
# headers vendorizados em include/ ja estao no CFLAGS).
SDL_CFLAGS := $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf 2>/dev/null)
SDL_LIBS   := $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf 2>/dev/null)
ifeq ($(strip $(SDL_LIBS)),)
SDL_LIBS := -L$(LIB_DIR) -lSDL3 -lSDL3_image -lSDL3_ttf
endif

CFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS) -lm

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

endif

-include $(DEPS)
