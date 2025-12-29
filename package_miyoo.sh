#!/bin/bash

# Configuration
APP_NAME="NotepadInc"
VERSION="1.0.0"
DIST_DIR="dist/$APP_NAME"

echo "📦 Packaging $APP_NAME for Miyoo Mini Plus..."

# 1. Clean previous build
make clean
rm -rf dist

# 2. Build for Miyoo (Cross-Compile)
# Check if compiler exists
if command -v arm-linux-gnueabihf-g++ &> /dev/null; then
    echo "🛠  Compiling with arm-linux-gnueabihf-g++..."
    make miyoo
else
    echo "⚠️  Cross-compiler not found! Building generic host binary instead for testing."
    echo "   (To build for device, ensure arm-linux-gnueabihf-g++ is in your PATH)"
    make all
fi

# 3. Create Directory Structure
echo "📂 Creating directory structure..."
mkdir -p "$DIST_DIR"
mkdir -p "$DIST_DIR/assets/fonts"
mkdir -p "$DIST_DIR/notes"
mkdir -p "$DIST_DIR/.sys_cache"

# 4. Copy Files
echo "📄 Copying files..."
if [ -f "$APP_NAME" ]; then
    cp "$APP_NAME" "$DIST_DIR/"
else
    echo "❌ Error: Binary not found!"
    exit 1
fi

# Copy assets if they exist
if [ -d "assets" ]; then
    cp -r assets/* "$DIST_DIR/assets/"
else
    echo "⚠️  Warning: assets directory not found. Creating placeholder."
    # We need a font for SDL_ttf
    # Assuming user has one or we should check
fi

# Create launch script (optional but good for Onion OS)
cat > "$DIST_DIR/launch.sh" << EOL
#!/bin/sh
cd \$(dirname "\$0")
./$APP_NAME
EOL
chmod +x "$DIST_DIR/launch.sh"

# Create icon placeholder
touch "$DIST_DIR/icon.png"

# Create config.json / settings.cfg placeholder
touch "$DIST_DIR/settings.cfg"

# 5. Zip it up
echo "🤐 Zipping package..."
cd dist
zip -r "$APP_NAME-$VERSION-Miyoo.zip" "$APP_NAME"
cd ..

echo "✅ Done! Package available at: dist/$APP_NAME-$VERSION-Miyoo.zip"
echo "   Unzip this to /mnt/SDCARD/App/ on your Miyoo Mini Plus."
