#!/bin/bash
# Install development tools for Obsidian CI/CD
# Run this once to set up code quality tools

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$PROJECT_ROOT"

echo "Installing development tools for Obsidian..."

# Install buildifier (Bazel formatter)
if ! command -v buildifier &> /dev/null; then
    echo "Installing buildifier..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install buildifier
        else
            echo "Please install buildifier manually: https://github.com/bazelbuild/buildtools"
        fi
    else
        echo "Please install buildifier manually: https://github.com/bazelbuild/buildtools"
    fi
else
    echo "✓ buildifier already installed"
fi

# Install buildozer (Bazel refactoring tool)
if ! command -v buildozer &> /dev/null; then
    echo "Installing buildozer..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install buildozer
        else
            echo "Please install buildozer manually: https://github.com/bazelbuild/buildtools"
        fi
    else
        echo "Please install buildozer manually: https://github.com/bazelbuild/buildtools"
    fi
else
    echo "✓ buildozer already installed"
fi

# Install Node.js dependencies for CLI
if [[ -d "cli" ]]; then
    echo "Installing CLI dependencies..."
    cd cli
    if [[ -f "package.json" ]]; then
        npm install
        echo "✓ CLI dependencies installed"
    fi
    cd "$PROJECT_ROOT"
fi

echo ""
echo "Development tools installation complete!"
echo ""
echo "You can now run:"
echo "  make ci          # Run full CI pipeline"
echo "  ./scripts/ci.sh  # Or run the script directly"

