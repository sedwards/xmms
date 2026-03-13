#!/usr/bin/env bash
set -e

ROOT="$HOME/source"

AUD="$ROOT/audacious"
PLUG="$ROOT/audacious-plugins"

BUILD="$ROOT/build"
STAGE="$ROOT/stage"
APP="$ROOT/xmms.app"

PREFIX="$(pwd)/plugins/audacious-plugins"

export PREFIX="$HOME/source/stage/usr"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
export PKG_CONFIG_PATH=$HOME/source/stage/usr/lib/pkgconfig:$PKG_CONFIG_PATH

########################################
# CLEAN
########################################

if [ "$1" = "clean" ]; then
    echo "Cleaning build directories"
    rm -rf "$BUILD" "$STAGE" "$APP"
    exit 0
fi

########################################
# INSTALL DEPENDENCIES
########################################

install_deps() {

echo "Installing dependencies"

brew install \
  meson ninja pkg-config cmake \
  gettext glib gtk+3 \
  ffmpeg \
  libvorbis libogg flac opus opusfile \
  mpg123 libcue \
  wavpack lame \
  soxr libbs2b || true

}

########################################
# ENVIRONMENT
########################################

setup_env() {

export PATH="/opt/homebrew/bin:$PATH"

export PKG_CONFIG_PATH="$STAGE/usr/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:/opt/homebrew/opt/gettext/lib/pkgconfig"

export CFLAGS="-I$STAGE/usr/include/audacious -I$STAGE/usr/include"
export CXXFLAGS="-I$STAGE/usr/include/audacious -I$STAGE/usr/include"
export LDFLAGS="-L$STAGE/usr/lib"

}

########################################
# BUILD AUDACIOUS
########################################

build_audacious() {

echo "Building audacious"

#meson setup "$BUILD/audacious" "$AUD" \
#  --prefix=$PREFIX \
#  -Dqt=false \
#  -Ddbus=false

meson compile -C "$BUILD/audacious"

meson install -C "$BUILD/audacious" --destdir "$STAGE"

}

########################################
# BUILD PLUGINS
########################################

build_plugins() {

echo "Building plugins"

rm -rf "$BUILD/plugins"

meson setup "$BUILD/plugins" "$PLUG" \
  --prefix="$PREFIX"

ninja -C "$BUILD/plugins"
ninja -C "$BUILD/plugins" install

}

########################################
# CREATE APP BUNDLE
########################################

create_bundle() {

echo "Creating bundle"

mkdir -p "$APP/Contents/MacOS"
mkdir -p "$APP/Contents/Resources"
mkdir -p "$APP/Contents/Frameworks"

cp "$STAGE/$PREFIX/bin/audacious" \
   "$APP/Contents/MacOS/"

cp -R "$STAGE/$PREFIX/lib" \
      "$APP/Contents/Resources/"

cp -R "$STAGE/$PREFIX/share" \
      "$APP/Contents/Resources/"

}

########################################
# COPY BREW LIBRARIES
########################################

copy_libs() {

BIN="$1"

otool -L "$BIN" | awk '{print $1}' | grep "/opt/homebrew" | while read LIB
do
    NAME=$(basename "$LIB")
    DEST="$APP/Contents/Frameworks/$NAME"

    if [ ! -f "$DEST" ]; then
        echo "Copying $NAME"
        cp "$LIB" "$DEST"
    fi
done

}

########################################
# FIX LIBRARY PATHS
########################################

fix_paths() {

BIN="$1"

otool -L "$BIN" | awk '{print $1}' | grep "/opt/homebrew" | while read LIB
do
    NAME=$(basename "$LIB")

    install_name_tool \
        -change "$LIB" \
        "@executable_path/../Frameworks/$NAME" \
        "$BIN"
done

}

########################################
# BUNDLE LIBRARIES
########################################

bundle_libs() {

echo "Bundling libraries"

copy_libs "$APP/Contents/MacOS/audacious"

for LIB in "$APP/Contents/Frameworks/"*.dylib
do
    install_name_tool -id "@rpath/$(basename "$LIB")" "$LIB"
    fix_paths "$LIB"
done

fix_paths "$APP/Contents/MacOS/audacious"

}

########################################
# INFO PLIST
########################################

create_plist() {

cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
"http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>

<key>CFBundleExecutable</key>
<string>audacious</string>

<key>CFBundleIdentifier</key>
<string>org.audacious.player</string>

<key>CFBundleName</key>
<string>Audacious</string>

<key>CFBundleVersion</key>
<string>4.6</string>

<key>CFBundlePackageType</key>
<string>APPL</string>

</dict>
</plist>
EOF

}

########################################
# RUN BUILD
########################################

install_deps
setup_env

mkdir -p "$BUILD"
mkdir -p "$STAGE"

#build_audacious
build_plugins

create_bundle
bundle_libs
create_plist

echo ""
echo "Build finished:"
echo "$APP"

