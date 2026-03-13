# Roadmap - XMMS Darwin Port

## Phase 1: Core Migration (Current)
- [x] GTK 3 & Cairo structural migration for skin engine.
- [x] Modernize all custom widgets (buttons, sliders, vis).
- [x] Native CoreAudio output plugin implementation.
- [ ] Clean compilation of all core plugins (WAV, Tonegen, Echo, etc.).
- [ ] Link final `xmms` executable on macOS.

## Phase 2: Native Integration & Stability
- [ ] Implement macOS native directory browsing (`NSOpenPanel` / `NSSavePanel`) in `libxmms/darwin_dirbrowser.m`.
- [ ] Replace stubbed legacy dialogs (Preferences, URL window) with modern GTK 3 implementations.
- [ ] Fix threading/event loop artifacts in the new Cairo rendering loop.
- [ ] Verify pixel-perfect skin rendering against original assets.

## Phase 3: Audacious Plugin Compatibility
- [ ] **Audacious API Wrapper:** Create an input plugin wrapper to load and run the ~40 Audacious plugins located in `Audacious-Plugins/`.
- [ ] Integrate the Audacious plugin build process into the GNU Autotools system.
- [ ] Verify functionality of key decoders and effects from the Audacious suite.
