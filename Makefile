CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

TARGET = demineur
SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/logique/demineur.c \
          $(SRC_DIR)/interface/gui.c

OBJECTS = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/demineur.o \
          $(OBJ_DIR)/gui.o

all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/demineur.o: $(SRC_DIR)/logique/demineur.c $(SRC_DIR)/logique/demineur.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/logique/demineur.c -o $(OBJ_DIR)/demineur.o

$(OBJ_DIR)/gui.o: $(SRC_DIR)/interface/gui.c $(SRC_DIR)/interface/gui.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/interface/gui.c -o $(OBJ_DIR)/gui.o

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Différentes tailles de grille
facile: $(TARGET)
	./$(TARGET) 8 8 10

moyen: $(TARGET)
	./$(TARGET) 16 16 40

difficile: $(TARGET)
	./$(TARGET) 16 30 99

.PHONY: all directories clean run facile moyen difficile
