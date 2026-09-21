#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TARGET_DIR="${REPO_ROOT}/tests/files/xmlconf"

echo "=== Downloading W3C XML Conformance Test Suite ==="
mkdir -p "${TARGET_DIR}"
curl -A "Mozilla/5.0" -fL https://www.w3.org/XML/Test/xmlts20130923.tar.gz | tar -xz --strip-components=1 -C "${TARGET_DIR}"
echo "W3C XML Conformance Test Suite successfully downloaded to: ${TARGET_DIR}"
