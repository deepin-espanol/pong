# Pong

Classic Pong game built with Qt6 and DTK6.

## Requirements

- Qt 6 (Widgets module)
- Dtk6Widget

### Ubuntu/Debian

```bash
sudo apt install qt6-base-dev libdtk6widget-dev
```

### Arch Linux

```bash
sudo pacman -S qt6-base dtk6
```

## Compile

```bash
cd Pong
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Run

```bash
./pong
```

## Controls

- Player 1 (left): W (up), S (down)
- Player 2 (right): Up arrow, Down arrow
- New game: Ctrl+N
- Quit: Ctrl+Q

## Scoring

A point is scored when the ball passes a player's paddle and reaches the edge of the screen.
