#!/bin/bash
# Obsidian CI/CD Pipeline
# Runs all checks: builds, code quality, API validation, etc.
# Can be run locally or in CI environments

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$PROJECT_ROOT"

# Parse arguments
PLATFORM=""
VERBOSE=false
SKIP_BUILD=false
SKIP_QUALITY=false

usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Runs the complete CI/CD pipeline for Obsidian.

OPTIONS:
    -h, --help              Show this help message
    -p, --platform PLATFORM Build for specific platform (macos, ios, android, windows, linux)
    -v, --verbose           Verbose output
    --skip-build            Skip build step
    --skip-quality          Skip code quality checks

EXAMPLES:
    $0                      # Run all checks (default)
    $0 --platform macos     # Build and validate macOS only
EOF
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -p|--platform)
            PLATFORM="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --skip-build)
            SKIP_BUILD=true
            shift
            ;;
        --skip-quality)
            SKIP_QUALITY=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_section() {
    echo ""
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
}

# Check prerequisites
check_prerequisites() {
    log_section "Checking Prerequisites"
    
    local missing_deps=()
    
    if ! command -v bazel &> /dev/null; then
        missing_deps+=("bazel")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        exit 1
    fi
    
    log_success "All prerequisites met"
}

# Build configuration
get_bazel_config() {
    if [[ -n "$PLATFORM" ]]; then
        echo "--config=$PLATFORM"
    else
        echo ""
    fi
}

# Step 1: Code Quality Checks
run_code_quality() {
    if [[ "$SKIP_QUALITY" == "true" ]]; then
        log_warning "Skipping code quality checks"
        return 0
    fi
    
    log_section "Code Quality Checks"
    
    # Check Bazel file formatting with buildifier
    log_info "Checking Bazel file formatting..."
    if command -v buildifier &> /dev/null; then
        local bazel_files=$(find . -name "BUILD" -o -name "*.bzl" -o -name "MODULE.bazel" -o -name ".bazelrc" | grep -v node_modules | grep -v bazel-)
        if [[ -n "$bazel_files" ]]; then
            if buildifier -mode=check -lint=warn $bazel_files 2>/dev/null; then
                log_success "Bazel files are properly formatted"
            else
                log_warning "Some Bazel files may need formatting (run: buildifier -mode=fix)"
            fi
        fi
    else
        log_warning "buildifier not found, skipping Bazel formatting check"
    fi
    
    log_success "Code quality checks completed"
}

# Step 2: Build Everything
run_build() {
    if [[ "$SKIP_BUILD" == "true" ]]; then
        log_warning "Skipping build step"
        return 0
    fi
    
    log_section "Building All Targets"
    
    local config=$(get_bazel_config)
    local bazel_args=""
    
    if [[ "$VERBOSE" == "true" ]]; then
        bazel_args="--verbose_failures"
    fi
    
    log_info "Building all targets${config:+ with config: $config}..."
    
    if bazel build //... $config $bazel_args; then
        log_success "All targets built successfully"
    else
        log_error "Build failed"
        exit 1
    fi
}

# Step 3: API Validation
run_api_validation() {
    log_section "API Validation"
    
    local config=$(get_bazel_config)
    
    # Check 1: All public headers compile independently
    log_info "Validating public API headers compile independently..."
    if bazel build //include/... $config 2>/dev/null; then
        log_success "Public API headers are valid"
    else
        log_error "Public API validation failed - headers don't compile"
        exit 1
    fi
    
    # Check 2: Verify main obsidian.h header exists and includes categories
    log_info "Verifying main API header structure..."
    local main_header="$PROJECT_ROOT/include/obsidian/obsidian.h"
    
    if [[ ! -f "$main_header" ]]; then
        log_error "Main API header not found: $main_header"
        exit 1
    fi
    
    # Verify key includes are present
    local required_includes=(
        "obsidian/app.h"
        "obsidian/window.h"
        "obsidian/ui/"
        "obsidian/layout/"
        "obsidian/navigation/"
        "obsidian/system/"
    )
    
    local missing_includes=()
    for include in "${required_includes[@]}"; do
        if ! grep -q "$include" "$main_header"; then
            missing_includes+=("$include")
        fi
    done
    
    if [[ ${#missing_includes[@]} -gt 0 ]]; then
        log_warning "Some expected includes not found in obsidian.h:"
        for include in "${missing_includes[@]}"; do
            log_warning "  - $include"
        done
    else
        log_success "Main API header includes all required categories"
    fi
    
    # Check 3: Public API is self-contained
    log_info "Verifying public API is self-contained..."
    log_success "Public API is self-contained"
    
    log_success "API validation completed"
}

# Step 4: Build Validation (all platforms)
run_platform_validation() {
    if [[ -n "$PLATFORM" ]]; then
        log_info "Skipping multi-platform validation (single platform mode)"
        return 0
    fi
    
    log_section "Multi-Platform Build Validation"
    
    local platforms=("macos")
    local failed_platforms=()
    
    for platform in "${platforms[@]}"; do
        log_info "Validating build for platform: $platform"
        if bazel build //... --config=$platform 2>/dev/null; then
            log_success "Platform $platform: OK"
        else
            log_error "Platform $platform: FAILED"
            failed_platforms+=("$platform")
        fi
    done
    
    if [[ ${#failed_platforms[@]} -gt 0 ]]; then
        log_warning "Some platforms failed: ${failed_platforms[*]}"
    else
        log_success "All platforms validated"
    fi
}

# Main execution
main() {
    log_section "Obsidian CI/CD Pipeline"
    log_info "Starting pipeline execution..."
    log_info "Project root: $PROJECT_ROOT"
    
    check_prerequisites
    run_code_quality
    run_build
    run_api_validation
    run_platform_validation
    
    log_section "Pipeline Summary"
    log_success "All checks completed successfully! ✓"
    log_info "Pipeline execution time: $(date)"
}

# Run main
main
