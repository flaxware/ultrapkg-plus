# Ultrapkg Plus

Ultrapkg Plus is the default desktop package and app storefront for the Flax.system ecosystem on XFCE. It is designed to feel refined like GNOME Software and Apple App Store, but built for minimalism, portability, and Unix discipline. It is not a heavy web app, and it never depends on the external stores it is emulating.

This project is a modular frontend over `ultrapkg`, with native support for Debian/Ubuntu packages, Flatpak, and AppImage workflows. It is built for XFCE4, Thunar, and lightweight desktop systems that want security, speed, and clarity without unnecessary overhead.

## Flax.system philosophy

This project follows the Flax.system ideals:

- General: hierarchical and predictable by design
- Efficient: lightweight, fast, and quiet on system resources
- Perfect: ready to use with sane defaults and clean UX
- Flexible: configuration through plain text and native desktop files
- Portable: modular, maintainable, and suitable for live builds and custom distros
- Safe: security-first, dependency-aware, and careful with package execution paths

## Project intent

Ultrapkg Plus is the XFCE4 store layer for Flax.system. It unifies the user experience for:

- APT/Debian package installation and removal
- Flatpak application installation and updates
- AppImage convenience workflows
- local package and archive installation from the file manager
- package update visibility, metadata, and install history

It is intentionally designed to sit in the same category as a native desktop store, but without the weight and platform lock-in of large app ecosystems.

## Why this exists

The desktop store should not be a bloated shell. It should be a compact, maintainable control layer that:

- is native to XFCE and Thunar
- integrates with `ultrapkg` as the system package manager backend
- supports Flatpak and AppImage as first-class targets
- remains minimal and easy to ship in custom Linux builds
- stays modular enough to be maintained by a small team or an individual distro project

## Design direction

- Debian/Ubuntu and XFCE-oriented first
- Flatpak-aware without becoming a Flatpak launcher
- AppImage-aware without becoming an AppImage launcher
- Thunar-native file association for `.deb`, `.flatpakref`, `.flatpak`, and `.appimage`
- GTK-first front-end, no embedded browser or heavyweight UI shell
- C++20, strong RAII, and a thin backend boundary

## Features

- package catalog view with categories and search
- install, update, and remove actions for supported sources
- `.deb` support via `gdebi` when available, otherwise controlled fallback to `ultrapkg`
- `.appimage` direct execution and install flow
- `.flatpakref` and `.flatpak` install flow through Flatpak
- Thunar integration for local package files
- live-build friendly design and desktop integration metadata
- GPL-3.0-or-later licensing

## Architecture

The project is designed as a small, layered system:

- UI layer: GTK4/gtkmm app shell and package catalog interface
- domain layer: package model, source types, and install intent
- backend adapters: APT, Flatpak, AppImage, and `ultrapkg` command wrappers
- desktop integration: `.desktop` launcher and Thunar action metadata

This keeps the UI thin and the system behavior explicit, which is consistent with Flax.system values.

## Repository layout

- `source-code/` — source tree and build logic
- `data/` — desktop entry, icon, and Thunar integration files
- `README.md` — project overview and developer documentation
- `LICENSE` — GPL-3.0-or-later

## Build from source

### Dependencies

On Debian/Ubuntu or Debian-based systems:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libgtkmm-4.0-dev flatpak gdebi-core
```

If you want stronger Flatpak support for full system integration, install the Flatpak runtime tooling too.

### Configure and build

```bash
cmake -S source-code -B build
cmake --build build
```

### Run

```bash
./build/ultrapkg-plus
```

### Package file handling

```bash
./build/ultrapkg-plus /path/to/package.deb
./build/ultrapkg-plus /path/to/appimage.AppImage
./build/ultrapkg-plus /path/to/application.flatpakref
```

## XFCE / Thunar integration

This app is explicitly built to fit into XFCE4, not replace it.

It supports:

- launching from the desktop app menu
- opening files directly from Thunar with the app selected as handler
- `.deb` and AppImage handling from the file manager
- a lightweight GTK front-end that respects XFCE windowing and desktop integration

Desktop and Thunar metadata are provided in `data/` so the store can be integrated into a live system or a custom Linux desktop image.

## Security and correctness

- no hardcoded secrets
- no bypass of package manager validation
- no silent weakening of install verification
- no web UI that undermines the system trust boundary
- package actions remain explicit and visible in the user interface

## Versioning

The project is versioned from `v0.1.0` and will continue forward with a lightweight semantic model as the product matures.

## License

This project is licensed under GPL-3.0-or-later.

## Notes for live-build and flax.system integration

This project is designed to be easy to bring into live-build systems and custom Linux images:

- build once and install the binary
- install the `.desktop` entry and icon
- install the Thunar action metadata
- ensure `ultrapkg`, `flatpak`, and `gdebi` are available in the target image
- set the store as the preferred GUI package entry point for XFCE environments

This makes Ultrapkg Plus a strong candidate for the default XFCE4 store layer in a Flax.system desktop.
