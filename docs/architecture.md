# Ultrapkg Plus architecture

## 1. Scope

`ultrapkg-plus` is the user-facing desktop package for Flax system.

It is intentionally separate from the `ultrapkg` backend package.

- `ultrapkg` = command-line core and package logic
- `ultrapkg-plus` = polished user interface and desktop integration

## 2. Design intent

The UI should feel premium but still lightweight.

This means:
- clean glass-like panels
- soft purple accent colors
- smooth, modern layout
- quick drag-and-drop install behavior
- strong compatibility with XFCE/XFWM style and themes

## 3. Module split

### Front-end layer
The GUI shell, buttons, files, metadata cards, install flow, and UX polishing.

### Backend layer
The actual package management logic remains in `ultrapkg`.

### Driver layer
The GUI calls backend commands like:

```bash
ultrapkg install /path/to/package.deb
ultrapkg info /path/to/package.deb
```

## 4. Flax philosophy match

This project keeps the Flax principle: solid structure, no overgrown app-store shell, beautiful but restrained design.

The GUI is premium-looking without being heavy and bloated.

## 5. Implementation notes

The front-end should:
- allow drag-and-drop package import
- display metadata quickly
- keep the UX fast
- allow install and uninstall actions
- remain responsive under XFCE

This keeps the final app elegant and practical at the same time.
