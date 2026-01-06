# Obsidian CI/CD Pipeline Makefile
# Run `make ci` to execute the full pipeline

.PHONY: help ci ci-verbose build quality api-validate clean

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
	@echo "  make build       # Build all targets"

ci: ## Run full CI pipeline
	@./scripts/ci.sh

build: ## Build all targets
	@./scripts/ci.sh --skip-quality

quality: ## Run code quality checks only
	@./scripts/ci.sh --skip-build

api-validate: ## Validate API compatibility
	@bazel build //include/...

clean: ## Clean Bazel build artifacts
	@echo "Cleaning Bazel build artifacts..."
	@bazel clean --expunge || true
	@echo "Clean complete"

# Platform-specific builds
build-macos: ## Build for macOS only
	@./scripts/ci.sh --platform macos --skip-quality

build-ios: ## Build for iOS only
	@./scripts/ci.sh --platform ios --skip-quality

# Development helpers
dev-setup: ## Set up development environment
	@echo "Setting up development environment..."
	@bazel version
	@echo "Development environment ready"

ci-verbose: ## Run CI pipeline with verbose output
	@./scripts/ci.sh --verbose
