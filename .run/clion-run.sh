#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../malloc-lab"
exec make "$@"
