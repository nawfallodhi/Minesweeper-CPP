# Minesweeper

A classic Minesweeper game built with Qt/C++ featuring multiple difficulty levels, flagging, and a clean interface.

## 🎮 Download and Play

[**Download for Windows**](https://github.com/nawfallodhi/Minesweeper-CPP/releases/latest/download/minesweeper_windows.zip)

### Quick Start:
1. Download the ZIP file above
2. Extract it to any folder
3. Run `minesweeper.exe`
4. No installation required!

## 🚀 Features
- **3 Difficulty Levels**: Beginner (9x9), Intermediate (16x16), Advanced (24x24)
- **Smart First Click**: Guaranteed safe start
- **Right-Click Flagging**: Mark potential mines with 🚩
- **Live Mine Counter**: Tracks remaining mines
- **Resizable Window**: Adaptive tile sizing
- **Win/Lose Detection**: Clear game end states

## 🛠️ Build from Source

### Requirements
- Qt 6.x
- CMake 3.16+
- C++ compiler

### Build Steps
```bash
git clone https://github.com/nawfallodhi/Minesweeper-CPP
cd Minesweeper-CPP
cmake -B build
cmake --build build
./build/minesweeper  # Or minesweeper.exe on Windows