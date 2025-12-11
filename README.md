# M2 IMAGINE - PROJET 3D - Éditeur de Terrain

**REYNIER Théo - VIGUIER Killian**

## 📝 Description

Éditeur de terrain 3D interactif. Ce projet permet de créer, manipuler et visualiser des terrains en temps réel grâce à OpenGL et Qt.

## ✨ Fonctionnalités

- **Génération procédurale de terrain** avec algorithme Simplex Noise
- **Rendu 3D en temps réel** avec shaders
- **Interface graphique** basée sur Qt
- **Manipulation de maillages 3D** (import/export, modifications)
- **Système de textures** pour le rendu du terrain
- **Caméra interactive** avec contrôles souris/clavier
- **Édition de terrain** en temps réel

## 🛠️ Technologies utilisées

- **C++** (99.8%)
- **Qt** - Framework pour l'interface graphique
- **OpenGL** - Rendu 3D
- **GLSL** - Shaders (vertex et fragment)
- **Simplex Noise** - Génération procédurale

## 📁 Structure du projet

```
Projet_3D/
├── icons/              # Icônes de l'application
├── moc/                # Fichiers générés par Qt MOC
├── obj/                # Fichiers objets 3D
├── shaders/            # Shaders GLSL (vertex/fragment)
├── textures/           # Textures pour le rendu
├── glwidget.cpp/h      # Widget OpenGL principal
├── maillage.cpp/h      # Gestion du maillage 3D
├── mainwindow.cpp/h    # Fenêtre principale
├── meshdialog.cpp/h    # Dialogue de gestion de maillage
├── SimplexNoise.cpp/h  # Implémentation du bruit de Simplex
└── TerrainEditor.pro   # Fichier projet Qt
```

## 🚀 Installation

### Prérequis

- **Qt 5.x**
- **OpenGL** 4.3+ compatible
- **Compilateur C++** (g++, MSVC, clang)
- **qmake** (inclus avec Qt)

### Compilation

```bash
# Cloner le dépôt
git clone https://github.com/RTheoGH/Projet_3D.git
cd Projet_3D

# Clean (si besoins)
make clean

# Générer le Makefile avec qmake
qmake TerrainEditor.pro

# Compiler le projet
make

# Lancer l'application
./TerrainEditor
```

### Compilation avec Qt Creator (recommandé)

1. Ouvrir le fichier `TerrainEditor.pro` sur **Qt Creator**
2. Configurer le kit de compilation
4. Appuyer sur **Build** (**Clean** si besoins)
5. Lancer avec **Run**
