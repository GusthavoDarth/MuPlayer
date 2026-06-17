CC       = gcc
CFLAGS   = -Wall -Wextra -O2 -I./src/include
LDFLAGS  = -lm
SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:.c=.o)
TARGET   = MuPlayer

# Detecção automática do sistema operacional
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(UNAME_S),Windows)
    # Windows (MinGW)
    LDFLAGS += -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET   = MuPlayer.exe
else ifeq ($(UNAME_S),Darwin)
    # macOS
    LDFLAGS += -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
else
    # Linux (e outros Unix)
    LDFLAGS += -lraylib -lGL -lpthread -ldl -lX11
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
