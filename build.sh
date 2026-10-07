set -euo pipefail

# Run from the repository root
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

# Configure and build. Stop if either command fails.
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build

mkdir -p data
