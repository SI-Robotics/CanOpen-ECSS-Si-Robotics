#!/usr/bin/env bash
# Build and install lely-core for Cortex-M4 (ECSS) using n7space lely-core.
# - clones repo (depth=1) on branch v3.5.0
# - runs autoreconf -i
# - configures for arm-none-eabi
# - builds static libs, no tests, no threads
# - NO dcf-tools here

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${SCRIPT_DIR}/lely-core"
BUILD_DIR=""
INSTALL_PREFIX=""
JOBS="$(nproc 2>/dev/null || echo 4)"

REPO_URL="https://gitlab.com/n7space/canopen/lely-core.git"
REPO_BRANCH="v3.5.0"

# Cortex-M4 defaults
CPU="cortex-m4"
FLOAT_ABI="hard"        # hard | softfp | soft
FPU="fpv4-sp-d16"       # set empty if no FPU, e.g. FPU=""

usage() {
  cat <<EOF
Usage: $0 -b <build-dir> -p <install-prefix> [OPTIONS]

Required:
  -b, --build-dir DIR      Build directory
  -p, --prefix DIR         Installation prefix

Optional:
  -s, --source-dir DIR     Source directory (default: ${SOURCE_DIR})
  -j, --jobs NUM           Parallel make jobs (default: ${JOBS})
      --cpu NAME           CPU (default: ${CPU})
      --float-abi MODE     hard|softfp|soft (default: ${FLOAT_ABI})
      --fpu NAME           FPU name, empty disables (default: ${FPU})
  -h, --help               Help

Examples:
  $0 -b ./build-cm4 -p ./install-cm4
  $0 -b ./b -p ./i --float-abi soft --fpu ""
EOF
  exit 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -b|--build-dir) BUILD_DIR="$2"; shift 2;;
    -p|--prefix)    INSTALL_PREFIX="$2"; shift 2;;
    -s|--source-dir) SOURCE_DIR="$2"; shift 2;;
    -j|--jobs)      JOBS="$2"; shift 2;;
    --cpu)          CPU="$2"; shift 2;;
    --float-abi)    FLOAT_ABI="$2"; shift 2;;
    --fpu)          FPU="$2"; shift 2;;
    -h|--help)      usage;;
    *) echo "Unknown option: $1"; usage;;
  esac
done

[[ -n "${BUILD_DIR}" ]] || { echo "Error: build dir required"; usage; }
[[ -n "${INSTALL_PREFIX}" ]] || { echo "Error: prefix required"; usage; }

BUILD_DIR="$(realpath -m "${BUILD_DIR}")"
INSTALL_PREFIX="$(realpath -m "${INSTALL_PREFIX}")"
mkdir -p "${BUILD_DIR}" "${INSTALL_PREFIX}"

echo "======================================"
echo "lely-core Cortex-M4 ECSS Build"
echo "======================================"
echo "Repo:      ${REPO_URL}"
echo "Branch:    ${REPO_BRANCH}"
echo "Source:    ${SOURCE_DIR}"
echo "Build:     ${BUILD_DIR}"
echo "Prefix:    ${INSTALL_PREFIX}"
echo "Jobs:      ${JOBS}"
echo "CPU:       ${CPU}"
echo "float-abi: ${FLOAT_ABI}"
echo "FPU:       ${FPU:-<none>}"
echo "======================================"

# Clone / refresh source
if [[ ! -d "${SOURCE_DIR}" ]]; then
  echo "Cloning lely-core (depth=1, branch=${REPO_BRANCH})..."
  command -v git >/dev/null
  git clone "${REPO_URL}" --depth=1 --branch="${REPO_BRANCH}" "${SOURCE_DIR}"
elif [[ -d "${SOURCE_DIR}/.git" ]]; then
  echo "Updating existing repo..."
  (
    cd "${SOURCE_DIR}"
    git fetch --depth=1 origin "${REPO_BRANCH}"
    git checkout -f "${REPO_BRANCH}"
  )
else
  echo "Warning: ${SOURCE_DIR} exists but is not a git repo. Using as-is."
fi

SOURCE_DIR="$(cd "${SOURCE_DIR}" && pwd)"

# Tools check
echo "Checking tools..."
for tool in git autoreconf autoconf automake libtool make arm-none-eabi-gcc arm-none-eabi-ar arm-none-eabi-ranlib arm-none-eabi-g++ arm-none-eabi-strip; do
  command -v "${tool}" >/dev/null || { echo "Missing tool: ${tool}"; exit 1; }
done

# Prepare build dir
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Always run autoreconf -i
echo "Running autoreconf -i..."
autoreconf -i "${SOURCE_DIR}"

# Flags based on your working manual configure flow
ARCH_FLAGS="-mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections"
FP_FLAGS=""
if [[ -n "${FPU}" ]]; then
  FP_FLAGS="-mfloat-abi=${FLOAT_ABI} -mfpu=${FPU}"
else
  FP_FLAGS="-mfloat-abi=soft"
fi

CFLAGS="-O2 -ggdb3 ${ARCH_FLAGS} ${FP_FLAGS} \
  -DLELY_HAVE_ITIMERSPEC=1 \
  -DLELY_NO_MALLOC \
  -DLELY_NO_CO_OBJ_NAME \
  -DLELY_NO_CO_OBJ_LIMITS \
  -DLELY_NO_CO_OBJ_DEFAULT \
  -DLELY_NO_CO_OBJ_UPLOAD \
  -DLELY_NO_THREADS \
  -DLELY_NO_STDIO"
LDFLAGS="-mcpu=${CPU} -mthumb ${FP_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections --specs=nosys.specs"

CONFIGURE_OPTIONS=(
  "--prefix=${INSTALL_PREFIX}"
  "--host=arm-none-eabi"
  "--enable-ecss-compliance"
  "--disable-shared"
  "--disable-python"
  "--disable-tests"
  "--disable-unit-tests"
  "--disable-threads"
  "AR=arm-none-eabi-ar"
  "CC=arm-none-eabi-gcc"
  "CXX=arm-none-eabi-g++"
  "RANLIB=arm-none-eabi-ranlib"
  "STRIP=arm-none-eabi-strip"
  "CFLAGS=${CFLAGS}"
  "LDFLAGS=${LDFLAGS}"
)

echo "Configuring..."
echo "CFLAGS:  ${CFLAGS}"
echo "LDFLAGS: ${LDFLAGS}"
"${SOURCE_DIR}/configure" "${CONFIGURE_OPTIONS[@]}"

echo "Building..."
make -j"${JOBS}"

echo "Installing..."
make install

if [[ -f "${BUILD_DIR}/config.h" ]]; then
  echo "Copying config.h to ${INSTALL_PREFIX}/include/ ..."
  cp "${BUILD_DIR}/config.h" "${INSTALL_PREFIX}/include/"
fi

echo "======================================"
echo "Done."
echo "======================================"
echo "Include: -I${INSTALL_PREFIX}/include"
echo "Libs:    -L${INSTALL_PREFIX}/lib -llely-co -llely-can -llely-compat -llely-util"
