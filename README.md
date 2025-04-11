# Blockworld

A simple Minecraft-inspired game created as a school project using C++ and SFML graphics library.

## Description

This project is a basic implementation of a Minecraft-style game featuring:
- Block placement and destruction
- Basic player movement
- Simple world generation
- Inventory system
- First-person camera controls

## Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- SFML 2.5 library

## Installation

### Linux
1. Install SFML and CMake:
```bash
sudo apt-get update
sudo apt-get install libsfml-dev cmake g++
```

2. Clone the repository:
```bash
git clone <repository-url>
cd minecraft
```

3. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

4. Run the game:
```bash
./MinecraftClone
```

### Windows
1. Install SFML and CMake:
   - Download SFML 2.5 from the [official website](https://www.sfml-dev.org/download.php)
   - Install CMake from the [official website](https://cmake.org/download/)

2. Follow steps 2-4 from the Linux installation using appropriate Windows commands

## Controls
- WASD: Movement
- Mouse: Look around
- Left Click: Break block
- Right Click: Place block
- ESC: Exit game

## Project Structure
- `src/`: Source files
- `include/`: Header files

## Note
This project was created as a school assignment and completed within an hour. While it demonstrates basic game development concepts, it may lack some features and polish found in more complex implementations.
