# 3d-graphics-from-scratch

A wireframe 3D renderer written in C using SDL3. No OpenGL, no GPU pipeline — just projecting points and drawing lines.

## what it does

Loads a 3D model from a header file, applies a perspective projection, and renders it as a rotating wireframe in a window. The model spins continuously around the Y axis. There's also an optional Z translation animation if you want the model to drift toward/away from the camera.

## installing dependencies

### Linux

**Arch Linux**
```
sudo pacman -S gcc sdl3 make
```

**Debian / Ubuntu**
```
sudo apt install gcc libsdl3-dev make
```

**Fedora**
```
sudo dnf install gcc SDL3-devel make
```

**openSUSE**
```
sudo zypper install gcc libSDL3-devel make
```

**Gentoo**
```
sudo emerge media-libs/libsdl3
```

**GNU Guix**
```
guix shell gcc-toolchain sdl3 make
```
No system install needed — just run that before `make` and you're in a shell with everything available.

**NixOS**
```nix
# shell.nix
{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = [ pkgs.gcc pkgs.SDL3 pkgs.gnumake ];
}
```
Then `nix-shell` and run `make` inside it.

### macOS

Install Homebrew if you don't have it, then:
```
brew install sdl3
```
Xcode command line tools provide `make` and `clang` (which the Makefile will pick up via `gcc`):
```
xcode-select --install
```

### Windows

The easiest path is MSYS2. Download it from msys2.org, then in the MSYS2 MINGW64 shell:
```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL3 make
```

## building

```
make
```

The Makefile picks up `GUIX_ENVIRONMENT` automatically if it's set (i.e. inside `guix shell`), otherwise it uses your system paths.

## running

```
./out
```

Close the window or hit the X button to quit.

## changing the model

Edit `config.h` and change `MODEL_PATH` to whichever header you want:

```c
#define MODEL_PATH "models/model_suzanne.h"
```

Then `make clean && make`. The `models/` directory has a bunch of them — nefertiti, buddha, dragon, horse, armadillo, ironman, mario, suzanne, and more.

## configuration

Everything lives in `config.h`:

- `WINDOW_WIDTH` / `WINDOW_HEIGHT` — window size in pixels
- `FPS` — target frame rate
- `ROTATIONAL_SPEED_PERCENTAGE` — how fast the model spins
- `USE_TRANSLATION_ANIMATION` — set to `1U` to enable Z drift
- `TRANSLATION_SPEED_PERCENTAGE` — speed of the Z drift
- `STARTING_Z_POS` — initial camera distance
- `BG_*` / `FG_*` — background and foreground RGBA colors

## model format

Each model header defines `POINT_COUNT`, `PHASE_COUNT`, a `vec_t points[]` array (x/y/z floats), and a `phase_t phases[]` array (pairs of point indices forming edges). If you want to add your own model just follow the same format.

## dependencies

- SDL3
- libm (standard)

## license

do whatever you want with it
