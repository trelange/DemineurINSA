CC = gcc
# Ajout explicite de sdl2 et SDL2_mixer dans pkg-config
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_mixer) -Wall -g
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_mixer)

TARGET = demineur
SRC_DIR = src
OBJ_DIR = obj

# Liste des fichiers sources
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/logique/demineur.c \
          $(SRC_DIR)/interface/gui.c

# Conversion des .c en .o
OBJECTS = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/demineur.o \
          $(OBJ_DIR)/gui.o

all: directories $(TARGET)

# Création du dossier obj si inexistant
directories:
	mkdir -p $(OBJ_DIR)

# Édition de liens
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compilation des fichiers objets
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/demineur.o: $(SRC_DIR)/logique/demineur.c $(SRC_DIR)/logique/demineur.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/logique/demineur.c -o $(OBJ_DIR)/demineur.o

$(OBJ_DIR)/gui.o: $(SRC_DIR)/interface/gui.c $(SRC_DIR)/interface/gui.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/interface/gui.c -o $(OBJ_DIR)/gui.o

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Raccourcis pour lancer le jeu
run: $(TARGET)
	./$(TARGET)

# Raccourci pour lancer sans paramètres (ouvre le menu)
menu: $(TARGET)
	./$(TARGET)

.PHONY: all directories clean run menu