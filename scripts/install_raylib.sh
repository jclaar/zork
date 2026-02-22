#!/bin/bash
# Install raylib on Ubuntu/Debian-based systems

set -e

echo "=========================================="
echo "Installing raylib build dependencies..."
echo "=========================================="

sudo apt update
sudo apt install -y \
    libasound2-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxcursor-dev \
    libxinerama-dev \
    pkg-config \
    cmake \
    git \
    build-essential

echo ""
echo "=========================================="
echo "Cloning raylib repository..."
echo "=========================================="

RAYLIB_DIR="${RAYLIB_DIR:-/tmp/raylib}"

if [ -d "$RAYLIB_DIR" ]; then
    echo "Removing existing raylib directory..."
    rm -rf "$RAYLIB_DIR"
fi

git clone https://github.com/raysan5/raylib.git "$RAYLIB_DIR"
cd "$RAYLIB_DIR"

echo ""
echo "=========================================="
echo "Building raylib..."
echo "=========================================="

mkdir -p build
cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)

echo ""
echo "=========================================="
echo "Installing raylib..."
echo "=========================================="

sudo make install
sudo ldconfig

echo ""
echo "=========================================="
echo "raylib installed successfully!"
echo "=========================================="
echo ""
echo "To build zork with raylib support, run:"
echo ""
echo "  cmake --preset linux-debug -DUSE_RAYLIB=ON"
echo "  cmake --build out/build/linux-debug"
echo ""
echo "Or with fresh build directory:"
echo ""
echo "  rm -rf out/build/linux-debug"
echo "  cmake --preset linux-debug -DUSE_RAYLIB=ON"
echo "  cmake --build out/build/linux-debug"
echo ""