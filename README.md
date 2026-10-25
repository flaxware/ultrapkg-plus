# Ultrapkg Plus

Ultrapkg Plus is a lightweight XFCE-native package store for desktops that prefer minimal resource use, clean workflows, and strong portability.

It is designed to work as a premium front-end for `ultrapkg` while also handling Flatpak and AppImage packages from a single desktop experience.

## Design goals

- Keep the user experience polished without dragging in a heavy app-store stack.
- Prefer GTK-native integration instead of web or Qt dependencies.
- Fit naturally into XFCE and Thunar workflows.
- Make `.deb`, `.appimage`, and `.flatpakref` installation feel like a native desktop action.
- Keep the backend thin and the UI simple.

## Versioning

The project starts at `v0.1.0` and follows a lightweight semantic model for the desktop frontend.

## Dependencies

- CMake 3.21+
- C++20 compiler
- pkg-config
- gtkmm-4.0

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Runtime integration

- Install the desktop entry: `ultrapkg-plus.desktop`
- Install the icon: `ultrapkg-plus.svg`
- Register Thunar actions via `data/thunar-actions.xml`

## Philosophy

This project follows the Flax system direction: general, efficient, perfect, flexible, portable, and safe.
