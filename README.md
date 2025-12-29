# Miyoo Mini Plus Input Engine

This project implements a high-speed text input system designed for the Miyoo Mini Plus (ARMv7, Linux/Onion OS).
It features a "Linear QWERTY Ribbon" and a vertical "Prediction Crank" with smooth elastic physics.

## Features

- **Linear QWERTY Ribbon**: Horizontal character selection with smooth scrolling.
- **Prediction Crank**: Vertical word suggestion list on the right.
- **Physics**: Linear Interpolation (`VisualPos += (TargetPos - VisualPos) * 0.22f`) for premium feel.
- **Snap-and-Bounce**: Automatic alignment to center selection.
- **Controls**:
  - **D-pad L/R**: Spin Ribbon.
  - **L1/R1**: Jump 5 characters.
  - **SELECT**: Toggle focus between Ribbon and Crank.
  - **START**: Auto-complete word/character.
  - **D-pad U/D**: Scroll Prediction Crank (when focused).

## Dependencies

- **SDL2**
- **SDL2_ttf**
- **C++17 compliant compiler**

## Build Instructions

### macOS / Linux (Desktop Testing)

You can use CMake or the provided Makefile.

**Using CMake:**
```bash
mkdir build
cd build
cmake ..
make
./MiyooInputEngine
```

**Using Makefile:**
```bash
make
./MiyooInputEngine
```

### Cross-Compilation for Miyoo Mini Plus

To compile for the Miyoo Mini Plus, you need the appropriate toolchain (usually `arm-linux-gnueabihf`).

Example Makefile adjustment:
```makefile
CXX = arm-linux-gnueabihf-g++
```

Ensure SDL2 and SDL2_ttf libraries for the ARM target are available in your toolchain search path.

## Assets

Place a valid TrueType font at `assets/fonts/default.ttf`. The system attempts to load this file first.
