# Guide d'installation détaillé

## Installation de GTK+3

### 💻 Windows

**Option 1 : MSYS2 (recommandé)**

1. Télécharger et installer MSYS2 : https://www.msys2.org/

2. Ouvrir MSYS2 MinGW 64-bit

3. Installer les paquets :
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-pkg-config
pacman -S make
```

4. Accéder au dossier du projet :
```bash
cd c:
cd Users/xnom01/...../demineur/
```

**Option 2 : WSL (Windows Subsystem for Linux)**

Suivre les instructions Linux une fois WSL installé.

### 🐧 Linux

#### Ubuntu / Debian / Linux Mint
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libgtk-3-dev
sudo apt-get install pkg-config
```

#### Fedora / RHEL / CentOS
```bash
sudo dnf install gcc make
sudo dnf install gtk3-devel
sudo dnf install pkg-config
```

#### Arch Linux
```bash
sudo pacman -S base-devel
sudo pacman -S gtk3
sudo pacman -S pkg-config
```

### 🍎 macOS

1. Installer Homebrew (si pas déjà fait) :
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Installer GTK+3 :
```bash
brew install gtk+3
brew install pkg-config
```

3. Configurer la variable d'environnement (important !) :
```bash
echo 'export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"' >> ~/.zshrc
source ~/.zshrc
```



## Vérification de l'installation

```bash
# Vérifier GTK+3
pkg-config --modversion gtk+-3.0

# Devrait afficher quelque chose comme : 3.24.x
```

## Compilation du projet

```bash
cd demineur
make
```

## En cas de problème

### Erreur : `command not found: pkg-config`
```bash
# Linux
sudo apt-get install pkg-config

# macOS
brew install pkg-config
```

### Erreur : Bibliothèques GTK non trouvées
```bash
# Réinstaller GTK+3
sudo apt-get install --reinstall libgtk-3-dev

# Ou sur macOS
brew reinstall gtk+3
```

### Problèmes de permissions
```bash
# Rendre le Makefile exécutable
chmod +x Makefile

# Nettoyer et recompiler
make clean
make
```
