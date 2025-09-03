#!/bin/bash
set -euo pipefail

# Defaults
BUILD_DIR="build"
BUILD_TYPE="Release"          # Debug|Release|RelWithDebInfo|MinSizeRel
PREFIX="/usr/local"
COMPILED="ON"                 # ON to build a compiled lib
SHARED="OFF"                  # ON to build shared (only if COMPILED=ON)
GENERATOR=""                  # auto-pick Ninja or Unix Makefiles

usage() {
  cat <<EOF
FeBundle Linux build

Usage:
  $(basename "$0") [options]

Options:
  --build-type <t>      Debug|Release|RelWithDebInfo|MinSizeRel (default: ${BUILD_TYPE})
  --prefix <path>       CMAKE_INSTALL_PREFIX (default: ${PREFIX})
  --not-compiled        Build as header-only (default: compiled)
  --shared              If compiled, build shared (.so) instead of static
  --generator <name>    'Ninja' or 'Unix Makefiles' (auto if omitted)
  --build-dir <dir>     Build directory (default: ${BUILD_DIR})
  -h|--help             Show this help
EOF
}

# Parse args
while [[ $# -gt 0 ]]; do
  case "$1" in
    --build-type)      BUILD_TYPE="$2"; shift 2;;
    --prefix)          PREFIX="$2"; shift 2;;
    --not-compiled)    COMPILED="OFF"; shift;;
    --shared)          SHARED="ON"; shift;;
    --generator)       GENERATOR="$2"; shift 2;;
    --build-dir)       BUILD_DIR="$2"; shift 2;;
    -h|--help)         usage; exit 0;;
    *) echo "Unknown option: $1"; usage; exit 1;;
  esac
done

# Auto-pick generator if not given
if [[ -z "${GENERATOR}" ]]; then
  if command -v ninja >/dev/null 2>&1; then
    GENERATOR="Ninja"
  else
    GENERATOR="Unix Makefiles"
  fi
fi

echo "==> Generator: ${GENERATOR}"
echo "==> Build type: ${BUILD_TYPE}"
echo "==> Prefix: ${PREFIX}"
echo "==> Compiled: ${COMPILED}  Shared: ${SHARED}"
echo "==> Build dir: ${BUILD_DIR}"

# Fresh build dir
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Configure
cmake -S . -B "${BUILD_DIR}" -G "${GENERATOR}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
  -DFEBUNDLE_COMPILED="${COMPILED}" \
  -DFEBUNDLE_BUILD_SHARED="${SHARED}" \
  -DCMAKE_INSTALL_PREFIX="${PREFIX}" \

# Build
cmake --build "${BUILD_DIR}"

# Install
sudo cmake --install "${BUILD_DIR}"

# Symlink compile_commands.json to repo root (for clangd/VSCode)
if [[ -f "${BUILD_DIR}/compile_commands.json" ]]; then
  ln -sf "${BUILD_DIR}/compile_commands.json" compile_commands.json || true
fi

echo "Done."

