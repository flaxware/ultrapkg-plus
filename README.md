# Ultrapkg Plus

Ultrapkg Plus is the premium graphical front-end for the `ultrapkg` backend.

It is intentionally separate from the core backend package so the system remains modular, clean, and maintainable.

The GUI is designed to feel premium and polished, inspired by the best parts of AppManager, but adapted to the Flax system direction and XFCE/XFWM environment.

This is a C++ GTK application with a candy-like visual language: soft glass, purple highlights, clean cards, fast actions, and little visual noise.

## Why separate GUI package?

Because the backend and the UI are different jobs.

- `ultrapkg` = engine, logic, package operations, metadata, CLI tools
- `ultrapkg-plus` = user experience, package browsing, drag-and-drop install flow, premium desktop polish

This keeps the project clean and hierarchical.

## Design direction

- AppManager-inspired elegance
- Flax-system philosophy: simple, clear, stable, modular
- XFCE/XFWM compatibility first
- lightweight interface, not a heavyweight app-store shell
- premium look without killing performance

## Features

- drag-and-drop `.deb` install
- install/uninstall buttons
- package metadata panel
- quick package info display
- desktop layout optimized for Linux and XFCE
- backend calls to `ultrapkg` as the engine

## Build

### Dependencies

On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libgtkmm-4.0-dev
```

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Run

```bash
./build/ultrapkg-plus
```

## Usage

1. Start `ultrapkg-plus`
2. Drag a `.deb` into the install area or click Select Package
3. Review metadata
4. Click Install
5. Use Uninstall for installed packages when needed

## Backend contract

`ultrapkg-plus` talks to the backend package via the `ultrapkg` CLI.

This means:
- the UI stays thin
- logic stays in the backend
- maintainability stays high
- package operations stay consistent

## License

GPL-3.0-or-later

## Philosophy

This repository is not a giant app store.
It is a premium desktop experience for a disciplined Linux system.
