#!/bin/bash
#
# build_xmms_homebrew.sh - Build XMMS using Homebrew GTK3 and create macOS app bundle
#
# This script:
# 1. Builds XMMS with using Homebrew's GTK3
# 2. Installs to a local prefix
# 3. Creates relocatable app bundle with all dependencies
#
# Requirements:
# - Homebrew with gtk+3 installed (brew install gtk+3)
# - AppBundleGenerator tool
# - Autotools 
#

set -e  # Exit on error

# Configuration
HOMEBREW_PREFIX="$(brew --prefix)"
XMMS_PREFIX="$HOME/source/xmms/xmms-install"
XMMS_SOURCE="$(cd "$(dirname "$0")" && pwd)"
DEST_DIR="${DEST_DIR:-.}"
APP_NAME="XMMS"
BUNDLE_ID="org.updatez.xmms-gtk"
VERSION="2.99.1"

# --- Find AppBundleGenerator ---
# 1. Use environment variable if set
# 2. Otherwise, search in PATH
# 3. Check for ../AppBundleGenerator
# 4. If not found, provide instructions
APP_BUNDLE_GENERATOR=""
if [ -n "$APP_BUNDLE_GENERATOR_PATH" ]; then
    if [ -x "$APP_BUNDLE_GENERATOR_PATH" ]; then
        APP_BUNDLE_GENERATOR="$APP_BUNDLE_GENERATOR_PATH"
    fi
elif command -v AppBundleGenerator &>/dev/null; then
    APP_BUNDLE_GENERATOR=$(command -v AppBundleGenerator)
elif [ -x "../AppBundleGenerator/AppBundleGenerator" ]; then
    APP_BUNDLE_GENERATOR="../AppBundleGenerator/AppBundleGenerator"
fi
# --- End Find AppBundleGenerator ---

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
error() {
    echo -e "${RED}ERROR: $1${NC}" >&2
    exit 1
}

info() {
    echo -e "${GREEN}INFO: $1${NC}"
}

warn() {
    echo -e "${YELLOW}WARN: $1${NC}"
}

if [ "$1" == "clean" ]; then
    info "Cleaning up..."
    rm -rf build
    rm -rf XMMS.app
    rm -rf xmms-install
    rm -f xmms.icns
    rm -f /tmp/xmms-launcher.sh
    info "Cleanup complete."
    exit 0
fi

echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}  XMMS macOS Builder (Homebrew)${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo -e "${BLUE}Configuration:${NC}"
echo "  Homebrew prefix: $HOMEBREW_PREFIX"
echo "  XMMS install prefix: $XMMS_PREFIX"
echo "  Source directory: $XMMS_SOURCE"
echo "  AppBundleGenerator: $APP_BUNDLE_GENERATOR"
echo "  Destination: $DEST_DIR"
echo "  Bundle identifier: $BUNDLE_ID"
echo "  Version: $VERSION"
echo ""

# Step 0: Check prerequisites
echo -e "${YELLOW}Checking prerequisites...${NC}"

# Check for GTK3
if ! pkg-config --exists gtk+-3.0; then
    echo -e "${RED}Error: GTK3 not found. Please install with:${NC}"
    echo "  brew install gtk+3"
    exit 1
fi

GTK_VERSION=$(pkg-config --modversion gtk+-3.0)
echo "  ✓ GTK3 version: $GTK_VERSION"

# Check for autotools
if ! command -v autoupdate &> /dev/null; then
    echo -e "${RED}Error: autotools not found. Installing...${NC}"
fi


# Check for AppBundleGenerator
if [ -z "$APP_BUNDLE_GENERATOR" ]; then
    error "AppBundleGenerator not found. Please ensure the 'AppBundleGenerator' executable is in your PATH or set the APP_BUNDLE_GENERATOR_PATH environment variable. You can download it from: https://github.com/sedwards-ibowl/AppBundleGenerator"
fi
info "Found AppBundleGenerator at: $APP_BUNDLE_GENERATOR"

echo -e "${GREEN}✓ All required prerequisites found${NC}"
echo ""

# Step 1: Set up environment for Homebrew GTK
echo -e "${YELLOW}Step 1: Setting up build environment...${NC}"

export PKG_CONFIG_PATH="$HOMEBREW_PREFIX/lib/pkgconfig:$HOMEBREW_PREFIX/opt/libffi/lib/pkgconfig"
export PATH="$HOMEBREW_PREFIX/bin:$PATH"
export CFLAGS="-I$HOMEBREW_PREFIX/include"
export LDFLAGS="-L$HOMEBREW_PREFIX/lib"

# Create install prefix
mkdir -p "$XMMS_PREFIX"

echo "  PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
echo "  PREFIX: $XMMS_PREFIX"
echo ""

# Step 2: Build XMMS
echo -e "${YELLOW}Step 2: Building XMMS...${NC}"

cd "$XMMS_SOURCE"

# Clean previous build
if [ -d "build" ]; then
    echo "  Cleaning previous build..."
    rm -rf build
fi

# Build
echo "  Building..."

 ./configure CFLAGS="$CFLAGS -Wall" --disable-opengl --enable-metal --enable-macos-audio --prefix="$XMMS_PREFIX"
  make
  make install

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: installation failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ XMMS structue built and installed${NC}"
echo ""

# Explicity add glib-2.0 schemas for AppBundleGenerator
echo -e "${GREEN}Please ignore the following glib-2.0 templates to bundle error, it is resolved later in the build...${NC}"

# Step 4: Creating .icns file from SVG
echo -e "${YELLOW}Step 4: Creating .icns file from SVG...${NC}"
SVG_ICON="$XMMS_SOURCE/icons/scalable/xmms.svg"
ICNS_FILE="$XMMS_SOURCE/xmms.icns"

if [ -f "$SVG_ICON" ]; then
    echo "  Found SVG icon, converting to ICNS..."
    TEMP_ICONSET=$(mktemp -d)/xmms.iconset
    mkdir -p "$TEMP_ICONSET"

    # Convert SVG to PNG at high resolution
    # Try multiple methods
    TEMP_PNG=""
    
    # Method 1: Try rsvg-convert if available
    if command -v rsvg-convert &> /dev/null; then
        TEMP_PNG="/tmp/xmms_icon.png"
        rsvg-convert -w 1024 -h 1024 "$SVG_ICON" -o "$TEMP_PNG" 2>/dev/null
    # Method 2: Try qlmanage
    elif command -v qlmanage &> /dev/null; then
        qlmanage -t -s 1024 -o /tmp "$SVG_ICON" 2>/dev/null
        TEMP_PNG=$(ls /tmp/xmms.svg.png 2>/dev/null | head -1)
    fi

    if [ -f "$TEMP_PNG" ]; then
        # Generate all required icon sizes
        for size in 16 32 128 256 512; do
            sips -z $size $size "$TEMP_PNG" --out "$TEMP_ICONSET/icon_${size}x${size}.png" >/dev/null 2>&1
            sips -z $((size*2)) $((size*2)) "$TEMP_PNG" --out "$TEMP_ICONSET/icon_${size}x${size}@2x.png" >/dev/null 2>&1
        done

        # Create ICNS file
        iconutil -c icns "$TEMP_ICONSET" -o "$ICNS_FILE"
        if [ -f "$ICNS_FILE" ]; then
            echo -e "${GREEN}  ✓ Icon converted successfully${NC}"
        fi

        # Cleanup
        rm -rf "$(dirname "$TEMP_ICONSET")" "$TEMP_PNG"
    else
        echo -e "${YELLOW}  Warning: Failed to convert SVG icon${NC}"
        ICNS_FILE=""
    fi
else
    echo -e "${YELLOW}  Warning: SVG icon not found${NC}"
    ICNS_FILE=""
fi
echo ""

# Step 5: Create app bundle
echo -e "${YELLOW}Step 5: Creating macOS app bundle...${NC}"

# Remove existing bundle
if [ -d "$DEST_DIR/$APP_NAME.app" ]; then
    warn "Removing existing bundle: $DEST_DIR/$APP_NAME.app"
    rm -rf "$DEST_DIR/$APP_NAME.app"
fi

# Create a wrapper script that sets up the environment
WRAPPER_SCRIPT="/tmp/xmms-launcher.sh"
cat > "$WRAPPER_SCRIPT" << 'EOF'
#!/bin/bash
# XMMS launcher script
BUNDLE_DIR="$(cd "$(dirname "$0")/../../.." && pwd)"
export GTK_PATH="$BUNDLE_DIR/Contents/Resources"
export GDK_PIXBUF_MODULE_FILE="$BUNDLE_DIR/Contents/Resources/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache"
export XMMS_SHARE_DIR="$BUNDLE_DIR/Contents/Resources/share"
export XDG_CONFIG_HOME="$HOME/Library/Application Support"
export XDG_DATA_HOME="$HOME/Library/Application Support"
export XDG_CACHE_HOME="$HOME/Library/Caches"
export XDG_STATE_HOME="$HOME/Library/Application Support:$BUNDLE_DIR/Contents/Resources/share"
export XDG_DATA_DIRS="/Library/Application Support"
export XDG_CONFIG_DIRS="$HOME/Library/Preferences:/Library/Application Support:/Library/Preferences"

# HiDPI and macOS Backend support
export GDK_BACKEND="quartz"
export PANGOCAIRO_BACKEND="coretext"

# Fallback scale detection in shell
if [ -z "$GDK_SCALE" ]; then
    # Try to detect if we are on a retina display
    if system_profiler SPDisplaysDataType | grep -q "Retina"; then
        export GDK_SCALE=2
        export GDK_DPI_SCALE=1.0
    fi
fi

# Set XMMS_CONFIG_DIR to point to bundled resources for default config lookups
# This will be used by lib/misc.c on macOS if XMMS_CONFIG_DIR is set.
export XMMS_CONFIG_DIR="$BUNDLE_DIR/Contents/Resources/share/xmms"

# Define the user's config directory (where XMMS expects writable config)
# This mimics the C code's default if XMMS_CONFIG_DIR is not set or ignored.
USER_HOME=$(eval echo "~") # Safely get home directory
USER_CONFIG_DIR="$USER_HOME/Library/XMMS"

# Ensure the user's config directory exists
mkdir -p "$USER_CONFIG_DIR"


USER_XMMSRC="$USER_CONFIG_DIR/xmmsrc"
BUNDLED_XMMSRC="$BUNDLE_DIR/Contents/Resources/share/xmms/xmmsrc"
BUNDLED_BOOKMARKS="$BUNDLE_DIR/Contents/Resources/share/xmms/bookmarks"
USER_BOOKMARKS="$USER_CONFIG_DIR/bookmarks"

# Copy default xmmsrc and bookmarks if they don't exist in the user's config
if [ ! -f "$USER_XMMSRC" ]; then
    echo "Initializing user's xmmsrc from bundle defaults..."
    if [ -f "$BUNDLED_XMMSRC" ]; then
        cp "$BUNDLED_XMMSRC" "$USER_XMMSRC"
    else
        echo "Warning: Bundled xmmsrc not found at $BUNDLED_XMMSRC"
    fi
fi

if [ ! -f "$USER_BOOKMARKS" ]; then
    echo "Initializing user's bookmarks from bundle defaults..."
    if [ -f "$BUNDLED_BOOKMARKS" ]; then
        cp "$BUNDLED_BOOKMARKS" "$USER_BOOKMARKS"
    else
        echo "Warning: Bundled bookmarks not found at $BUNDLED_BOOKMARKS"
    fi
fi

exec "$BUNDLE_DIR/Contents/Resources/bin/xmms-gtk" "$@"
EOF
chmod +x "$WRAPPER_SCRIPT"

echo "  Running AppBundleGenerator..."

"$APP_BUNDLE_GENERATOR" \
    --gtk \
    --icon "$ICNS_FILE" \
    --sign - \
    --hardened-runtime \
    --category public.app-category.utilities \
    --version "$VERSION" \
    --min-os 12.0 \
    --stage-dependencies "$XMMS_PREFIX" \
    --stage-dependencies "$HOMEBREW_PREFIX" \
    "$APP_NAME" \
    "$DEST_DIR" \
    "$WRAPPER_SCRIPT"

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: AppBundleGenerator failed${NC}"
    exit 1
fi

BUNDLE_PATH="$DEST_DIR/$APP_NAME.app"
echo -e "${GREEN}✓ App bundle created${NC}"
echo ""

# Explicitly copy xmms-gtk into the bundle's Resources/bin
echo -e "${YELLOW}Copying xmms-gtk to bundle...${NC}"
mkdir -p "$BUNDLE_PATH/Contents/Resources/bin"
cp "$XMMS_PREFIX/bin/xmms-gtk" "$BUNDLE_PATH/Contents/Resources/bin/xmms-gtk"
echo -e "${GREEN}✓ xmms-gtk copied to bundle${NC}"
echo ""

# Explicitly copy default config files (xmmsrc and bookmarks) into the bundle
echo -e "${YELLOW}Copying default config files to bundle...${NC}"
mkdir -p "$BUNDLE_PATH/Contents/Resources/share/xmms"
cp "$XMMS_PREFIX/share/xmms/xmmsrc" "$BUNDLE_PATH/Contents/Resources/share/xmms/xmmsrc"
cp "$XMMS_PREFIX/share/xmms/bookmarks" "$BUNDLE_PATH/Contents/Resources/share/xmms/bookmarks"
echo -e "${GREEN}✓ Default config files copied to bundle${NC}"
echo ""

# Explicity add glib-2.0 schemas for AppBundleGenerator
echo -e "${YELLOW}Copying glib-2.0 templates to bundle...${NC}"
mkdir -p "$BUNDLE_PATH/Contents/Resources/share/glib-2.0/"
cp -R "$XMMS_SOURCE/third_party/glib-2.0-schemas" "$BUNDLE_PATH/Contents/Resources/share/glib-2.0/schemas"

# Explicitly copy images and icons into the bundle
echo -e "${YELLOW}Copying images and icons to bundle...${NC}"

# Copy xmms.png from share/xmms
mkdir -p "$BUNDLE_PATH/Contents/Resources/share/xmms"
cp "$XMMS_PREFIX/share/xmms/xmms.png" "$BUNDLE_PATH/Contents/Resources/share/xmms/xmms.png"

# Copy hicolor icons from the main build
if [ -d "$XMMS_PREFIX/share/icons/hicolor" ]; then
    cp -R "$XMMS_PREFIX/share/icons/hicolor" "$BUNDLE_PATH/Contents/Resources/share/icons/"
fi

echo -e "${GREEN}✓ Images and icons copied to bundle${NC}"
echo ""

# Remove unwanted Qt dependencies
echo -e "${YELLOW}Cleaning unwanted Qt and Node.js dependencies...${NC}"
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "Qt*.framework" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "libqhull_r*.dylib" -exec rm -rf {} +
rm -rf "$BUNDLE_PATH/Contents/Resources/share/qt"

# Remove unwanted Node.js dependencies
rm -rf "$BUNDLE_PATH/Contents/Resources/lib/node_modules"
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "libnode*.dylib" -exec rm -rf {} +
rm -rf "$BUNDLE_PATH/Contents/Resources/etc/bash_completion.d/npm"
echo -e "${GREEN}✓ Unwanted dependencies cleaned${NC}"
echo ""

# Remove all other unwanted dependencies
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "libvtk*.dylib" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "critcl*" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "tcl*" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "tk9.0" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "X11" -exec rm -rf {} +


echo -e "${GREEN}✓ Unwanted dependencies cleaned${NC}"
echo ""

echo -e "${YELLOW}Step 6: Verifying app bundle...${NC}"
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "python3.1*"  -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "libpython*.dylib" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "libpython*.dylib" -exec rm -rf {} +
find "$BUNDLE_PATH/Contents/Resources/lib/" -depth 1 -name "*python*" -exec rm -rf {} +
gtk-update-icon-cache -f -t XMMS.app/Contents/Resources/share/icons/hicolor
echo ""


# Step 6: Verify the bundle
echo -e "${YELLOW}Step 6: Verifying app bundle...${NC}"

if [ ! -d "$BUNDLE_PATH" ]; then
    echo -e "${RED}Error: Bundle not created at $BUNDLE_PATH${NC}"
    exit 1
fi

echo "  Checking bundle structure..."
if [ -d "$BUNDLE_PATH/Contents/MacOS" ]; then
    echo "  ✓ MacOS directory exists"
fi

if [ -f "$BUNDLE_PATH/Contents/Info.plist" ]; then
    echo "  ✓ Info.plist exists"
fi

# Find the main executable
MAIN_BINARY=""
for candidate in "$BUNDLE_PATH/Contents/MacOS/xmms-gtk" \
                 "$BUNDLE_PATH/Contents/MacOS/XMMS" \
                 "$BUNDLE_PATH/Contents/MacOS/xmms-launcher.sh"; do
    if [ -f "$candidate" ] && file "$candidate" 2>/dev/null | grep -q "Mach-O"; then
        MAIN_BINARY="$candidate"
        echo "  ✓ Found executable: $(basename $(dirname $candidate))/$(basename $candidate)"
        break
    fi
done

if [ -n "$MAIN_BINARY" ]; then
    echo "  Checking dylib dependencies..."
    BAD_DEPS=$(otool -L "$MAIN_BINARY" 2>/dev/null | grep -v "@" | grep -v "/usr/lib" | grep -v ":" | wc -l | tr -d ' ')
    if [ "$BAD_DEPS" -gt 0 ]; then
        echo -e "${YELLOW}  Warning: Found $BAD_DEPS non-relocatable dependencies${NC}"
        otool -L "$MAIN_BINARY" | grep -v "@" | grep -v "/usr/lib" | grep -v ":"
    else
        echo "  ✓ All dependencies are relocatable"
    fi
fi

echo ""

# Summary
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}     Build Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo -e "${BLUE}Installed files:${NC}"
echo "  XMMS GTK: $XMMS_PREFIX/bin/xmms-gtk"
echo "  XMMS Text: $XMMS_PREFIX/bin/xmms-text (included as resource)"
echo ""

if [ -d "$BUNDLE_PATH" ]; then
    echo -e "${BLUE}App bundle:${NC}"
    echo "  $BUNDLE_PATH"
    echo ""
    echo -e "${BLUE}To test the application:${NC}"
    echo "  open \"$BUNDLE_PATH\""
    echo ""
else
    echo -e "${BLUE}To run XMMS:${NC}"
    echo "  $XMMS_PREFIX/bin/xmms-gtk"
    echo ""
fi

echo -e "${BLUE}To build an app bundle manually:${NC}"
echo "  1. Install AppBundleGenerator from: https://github.com/yourusername/AppBundleGenerator"
echo "  2. Set APP_BUNDLE_GENERATOR environment variable"
echo "  3. Re-run this script"
echo ""

echo -e "${BLUE}Build information:${NC}"
echo "  GTK version: $(pkg-config --modversion gtk+-3.0)"
echo "  GLib version: $(pkg-config --modversion glib-2.0)"
echo "  Build directory: $XMMS_SOURCE/build"
echo "  Install prefix: $XMMS_PREFIX"
echo ""

# Final Post-Processing: Ensure HiDPI is enabled in Info.plist
if [ -f "$BUNDLE_PATH/Contents/Info.plist" ]; then
    echo -e "${YELLOW}Final check: Enabling HiDPI support in Info.plist...${NC}"
    plutil -replace NSHighResolutionCapable -bool YES "$BUNDLE_PATH/Contents/Info.plist"
    echo -e "${GREEN}✓ HiDPI verified and enabled${NC}"
fi

# Cleanup 
rm -fr $DEST_DIR/build

