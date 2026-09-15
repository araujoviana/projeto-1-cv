# Compilador e flags de compilação
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O2
# Flags de linkagem para utilizar as DLLs presentes na pasta lib/
LDFLAGS = -Llib -lSDL3 -lSDL3_image -lSDL3_ttf

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Nome do executável final
TARGET = $(BIN_DIR)/main.exe

# Lista de arquivos fontes (.c) e objetos (.o)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Regra padrão
all: build_dirs $(TARGET)

build_dirs:
	-@mkdir $(OBJ_DIR) 2>nul || true
	-@mkdir $(BIN_DIR) 2>nul || true

# Linkagem do executável
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compilação dos arquivos fonte para objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados pela compilação
clean:
	-@rmdir /s /q $(subst /,\\,$(OBJ_DIR)) 2>nul || true
	-@del /q /f $(subst /,\\,$(TARGET)) 2>nul || true

# Regra para compilar e executar rapidamente o projeto
run: all
	./$(TARGET)

.PHONY: all clean run build_dirs
