#!/usr/bin/env bash
# Install lely-core python/dcf-tools into a Python venv (with pinned setuptools for pkg_resources).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Defaults
VENV_DIR="${SCRIPT_DIR}/.venv-dcf"
PRIMARY_SOURCE_DIR="${SCRIPT_DIR}/lely-core"
FALLBACK_SOURCE_DIR="${SCRIPT_DIR}/lely-core-dcf"
REPO_URL="https://gitlab.com/n7space/canopen/lely-core.git"

# Try these refs if dcf-tools not found in PRIMARY_SOURCE_DIR
FALLBACK_REFS=("v3.5.0" "ecss" "master" "main")
FORCE_REF=""

usage() {
  cat <<EOF
Usage: $0 [OPTIONS]
  -v, --venv-dir DIR
  -s, --source-dir DIR
  -f, --fallback-dir DIR
  -r, --repo-url URL
  -B, --branch NAME
  -h, --help
EOF
  exit 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -v|--venv-dir)      VENV_DIR="$2"; shift 2;;
    -s|--source-dir)    PRIMARY_SOURCE_DIR="$2"; shift 2;;
    -f|--fallback-dir)  FALLBACK_SOURCE_DIR="$2"; shift 2;;
    -r|--repo-url)      REPO_URL="$2"; shift 2;;
    -B|--branch)        FORCE_REF="$2"; shift 2;;
    -h|--help)          usage;;
    *) echo "Unknown option: $1"; usage;;
  esac
done

VENV_DIR="$(realpath -m "${VENV_DIR}")"
PRIMARY_SOURCE_DIR="$(realpath -m "${PRIMARY_SOURCE_DIR}")"
FALLBACK_SOURCE_DIR="$(realpath -m "${FALLBACK_SOURCE_DIR}")"

need_cmd() { command -v "$1" >/dev/null 2>&1 || { echo "Missing tool: $1"; exit 1; }; }
need_cmd git
need_cmd python3

find_dcf_tools_dir() {
  local src="$1"
  local d="${src}/python/dcf-tools"
  [[ -d "$d" ]] && echo "$d" && return 0
  return 1
}

select_refs() {
  if [[ -n "${FORCE_REF}" ]]; then
    printf "%s\n" "${FORCE_REF}"
  else
    printf "%s\n" "${FALLBACK_REFS[@]}"
  fi
}

echo "======================================"
echo "dcf-tools installer (pinned setuptools)"
echo "Repo:   ${REPO_URL}"
echo "Venv:   ${VENV_DIR}"
echo "Primary:${PRIMARY_SOURCE_DIR}"
echo "Fallback:${FALLBACK_SOURCE_DIR}"
echo "Ref:    ${FORCE_REF:-<auto>}"
echo "======================================"

DCF_TOOLS_DIR=""

# Try primary first
if [[ -d "${PRIMARY_SOURCE_DIR}" ]]; then
  if DCF_TOOLS_DIR="$(find_dcf_tools_dir "${PRIMARY_SOURCE_DIR}")"; then
    echo "Found dcf-tools in primary: ${DCF_TOOLS_DIR}"
  fi
fi

# Fallback clone if needed
if [[ -z "${DCF_TOOLS_DIR}" ]]; then
  echo "dcf-tools not found in primary. Using fallback checkout..."

  if [[ ! -d "${FALLBACK_SOURCE_DIR}/.git" ]]; then
    rm -rf "${FALLBACK_SOURCE_DIR}"
    git clone --depth=1 "${REPO_URL}" "${FALLBACK_SOURCE_DIR}"
  fi

  pushd "${FALLBACK_SOURCE_DIR}" >/dev/null
  git fetch --all --tags --prune >/dev/null 2>&1 || true

  FOUND_REF=""
  while read -r ref; do
    echo "Trying ref: ${ref}"
    if git checkout -f "${ref}" >/dev/null 2>&1; then
      git pull --ff-only >/dev/null 2>&1 || true
      if [[ -d "python/dcf-tools" ]]; then
        FOUND_REF="${ref}"
        break
      fi
    fi
  done < <(select_refs)

  popd >/dev/null

  if [[ -z "${FOUND_REF}" ]]; then
    echo "ERROR: Could not find python/dcf-tools in tried refs."
    exit 1
  fi

  DCF_TOOLS_DIR="${FALLBACK_SOURCE_DIR}/python/dcf-tools"
  echo "Found dcf-tools in fallback (${FOUND_REF}): ${DCF_TOOLS_DIR}"
fi

# Create venv
if [[ ! -d "${VENV_DIR}" ]]; then
  echo "Creating venv..."
  python3 -m venv "${VENV_DIR}"
fi

PY="${VENV_DIR}/bin/python"
PIP="${VENV_DIR}/bin/pip"

# Ensure pip exists inside venv (some setups need ensurepip)
"${PY}" -m ensurepip --upgrade >/dev/null 2>&1 || true

# Install pinned setuptools to guarantee pkg_resources
echo "Installing pinned setuptools (pkg_resources fix)..."
"${PY}" -m pip install --upgrade pip
"${PY}" -m pip install --upgrade "setuptools==65.5.1" wheel

# Quick sanity check
"${PY}" -c "import pkg_resources; print('pkg_resources OK:', pkg_resources.__version__)"

# Install dcf-tools
echo "Installing dcf-tools from: ${DCF_TOOLS_DIR}"
"${PY}" -m pip install "${DCF_TOOLS_DIR}"

echo "======================================"
echo "OK. Installed."
echo "Activate: source ${VENV_DIR}/bin/activate"
echo "Test:     dcf2dev --help"
echo "======================================"
