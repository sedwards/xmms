# Roadmap - XMMS Darwin Port

## Phase 1: Core Migration (COMPLETE)
- [x] GTK 3 & Cairo structural migration for skin engine.
- [x] Modernize all custom widgets (buttons, sliders, vis).
- [x] Native CoreAudio output plugin implementation.
- [x] Clean compilation of core plugins (WAV, MPG123, VORBIS).
- [x] Link final `xmms` executable on macOS (Darwin/ARM-64).

## Phase 2: Native Integration & Stability (COMPLETE)
- [x] Implement macOS native directory browsing (`NSOpenPanel`) in `libxmms/darwin_dirbrowser.m`.
- [x] Implement modern GTK 3 Preferences, Playlist, and Equalizer windows.
- [x] Integrate `GtkosxApplication` for native Apple menu bar support.
- [x] Verified playback: WAV, MP3, OGG.
- [x] Resolved plugin loading crashes and symbol resolution issues.

## Phase 3: Polish & Refinement (CURRENT)
- [ ] Implement volume/balance synchronization between Main and Equalizer.
- [ ] Finalize HiDPI sharp rendering using `CAIRO_FILTER_NEAREST`.
- [ ] Investigate potential Audacious plugin wrapper for extended format support.
- [ ] Implement Metal-based visualizer for macOS.

## Phase 4: Distribution & Packaging
- [ ] Create `.app` bundle for macOS.
- [ ] Configure `Info.plist` and file associations.
- [ ] Automate build and packaging process.
