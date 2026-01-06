# Obsidian CI/CD Pipeline Makefile
# Run `make ci` to execute the full pipeline

.PHONY: help ci ci-fast ci-full build test test-gui quality api-validate clean

# Default target
.DEFAULT_GOAL := help

# Colors for output
BLUE := \033[0;34m
GREEN := \033[0;32m
NC := \033[0m # No Color

help: ## Show this help message
	@echo "$(BLUE)Obsidian CI/CD Pipeline$(NC)"
	@echo ""
	@echo "Available targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  $(GREEN)%-15s$(NC) %s\n", $$1, $$2}'
	@echo ""
	@echo "Examples:"
	@echo "  make ci          # Run full CI pipeline (recommended)"
	@echo "  make ci-fast     # Run fast checks only (no GUI tests)"
	@echo "  make ci-full     # Run everything including GUI tests"
	@echo "  make build       # Build all targets"
	@echo "  make test        # Run all tests (excluding GUI)"

ci: ## Run full CI pipeline (default: fast, no GUI tests)
	@./scripts/ci.sh

ci-fast: ## Run fast CI pipeline (no GUI tests)
	@./scripts/ci.sh

ci-full: ## Run complete CI pipeline (includes GUI tests)
	@./scripts/ci.sh --gui-tests --memory-check

build: ## Build all targets
	@./scripts/ci.sh --skip-tests --skip-quality

test: ## Run all tests (excluding GUI tests)
	@./scripts/ci.sh --skip-build --skip-quality

test-gui: ## Run GUI tests (requires display)
	@./scripts/ci.sh --skip-build --skip-quality --gui-tests

quality: ## Run code quality checks only
	@./scripts/ci.sh --skip-build --skip-tests

api-validate: ## Validate API compatibility
	@bazel build //include/...

memory-check: ## Run memory leak detection
	@./scripts/ci.sh --skip-build --memory-check

clean: ## Clean Bazel build artifacts
	@echo "Cleaning Bazel build artifacts..."
	@bazel clean --expunge || true
	@echo "Clean complete"

# Platform-specific builds
build-macos: ## Build for macOS only
	@./scripts/ci.sh --platform macos --skip-tests --skip-quality

build-ios: ## Build for iOS only
	@./scripts/ci.sh --platform ios --skip-tests --skip-quality

# Development helpers
dev-setup: ## Set up development environment
	@echo "Setting up development environment..."
	@bazel version
	@node --version
	@npm --version || echo "npm not found"
	@echo "Development environment ready"

ci-verbose: ## Run CI pipeline with verbose output
	@./scripts/ci.sh --verbose

