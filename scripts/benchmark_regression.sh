#!/bin/bash
# Performance Regression Detection Script
# Compares current benchmark results against baseline and fails if regression > threshold

set -euo pipefail

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BASELINE_DIR="$PROJECT_ROOT/.benchmark_baselines"
REGRESSION_THRESHOLD=0.10  # 10% regression threshold
BENCHMARK_OUTPUT_DIR="$PROJECT_ROOT/benchmark_results"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

# Parse benchmark output and extract timing information
parse_benchmark_output() {
    local output_file="$1"
    local results=()
    
    # Google Benchmark outputs in format:
    # BM_TestName/arg1/arg2   1234 ns    5678 ns    12345 ns
    while IFS= read -r line; do
        if [[ $line =~ ^BM_ ]]; then
            # Extract benchmark name and time
            local name=$(echo "$line" | awk '{print $1}')
            local time_ns=$(echo "$line" | awk '{print $2}')
            
            # Convert to nanoseconds (handle different units)
            local time_value=$(echo "$time_ns" | sed 's/[^0-9.]//g')
            local time_unit=$(echo "$time_ns" | sed 's/[0-9.]//g')
            
            # Convert to nanoseconds
            case "$time_unit" in
                "ns") time_ns_value="$time_value" ;;
                "us") time_ns_value=$(echo "$time_value * 1000" | bc) ;;
                "ms") time_ns_value=$(echo "$time_value * 1000000" | bc) ;;
                "s")  time_ns_value=$(echo "$time_value * 1000000000" | bc) ;;
                *)    time_ns_value="$time_value" ;;
            esac
            
            results+=("$name:$time_ns_value")
        fi
    done < "$output_file"
    
    printf '%s\n' "${results[@]}"
}

# Load baseline results
load_baseline() {
    local baseline_file="$BASELINE_DIR/baseline.txt"
    
    if [[ ! -f "$baseline_file" ]]; then
        log_warning "No baseline found at $baseline_file"
        log_info "Creating initial baseline from current results..."
        return 1
    fi
    
    declare -gA baseline_results
    while IFS=: read -r name time; do
        baseline_results["$name"]="$time"
    done < "$baseline_file"
    
    return 0
}

# Save current results as baseline
save_baseline() {
    local output_file="$1"
    mkdir -p "$BASELINE_DIR"
    local baseline_file="$BASELINE_DIR/baseline.txt"
    
    cp "$output_file" "$baseline_file"
    log_success "Baseline saved to $baseline_file"
}

# Compare results and detect regressions
check_regression() {
    local current_file="$1"
    local has_regression=false
    
    log_info "Comparing against baseline..."
    
    # Parse current results
    declare -A current_results
    while IFS=: read -r name time; do
        current_results["$name"]="$time"
    done < "$current_file"
    
    # Compare with baseline
    local regression_count=0
    for name in "${!current_results[@]}"; do
        if [[ -n "${baseline_results[$name]:-}" ]]; then
            local baseline_time="${baseline_results[$name]}"
            local current_time="${current_results[$name]}"
            
            # Calculate percentage change
            local diff=$(echo "scale=4; ($current_time - $baseline_time) / $baseline_time" | bc)
            local percent_diff=$(echo "scale=2; $diff * 100" | bc)
            
            # Check if regression exceeds threshold
            if (( $(echo "$diff > $REGRESSION_THRESHOLD" | bc -l) )); then
                log_error "REGRESSION: $name"
                log_error "  Baseline: ${baseline_time} ns"
                log_error "  Current:  ${current_time} ns"
                log_error "  Change:   +${percent_diff}%"
                has_regression=true
                ((regression_count++))
            elif (( $(echo "$diff < -$REGRESSION_THRESHOLD" | bc -l) )); then
                log_success "IMPROVEMENT: $name"
                log_info "  Baseline: ${baseline_time} ns"
                log_info "  Current:  ${current_time} ns"
                log_info "  Change:   ${percent_diff}%"
            fi
        else
            log_warning "New benchmark: $name (no baseline)"
        fi
    done
    
    if [[ "$has_regression" == "true" ]]; then
        log_error "Found $regression_count regression(s) exceeding ${REGRESSION_THRESHOLD} threshold"
        return 1
    else
        log_success "No regressions detected"
        return 0
    fi
}

# Main execution
main() {
    cd "$PROJECT_ROOT"
    
    log_info "Performance Regression Detection"
    log_info "Threshold: ${REGRESSION_THRESHOLD} (${REGRESSION_THRESHOLD}%)"
    
    # Create output directory
    mkdir -p "$BENCHMARK_OUTPUT_DIR"
    
    # Run benchmarks and capture output
    local benchmark_output="$BENCHMARK_OUTPUT_DIR/current_results.txt"
    log_info "Running benchmarks..."
    
    # Run all benchmarks and capture output
    bazel run //benchmarks:window_creation_bench -- --benchmark_format=json > "$benchmark_output.window.json" 2>&1 || true
    bazel run //benchmarks:ui_render_bench -- --benchmark_format=json > "$benchmark_output.ui.json" 2>&1 || true
    bazel run //benchmarks:memory_usage_bench -- --benchmark_format=json > "$benchmark_output.memory.json" 2>&1 || true
    bazel run //benchmarks:startup_time_bench -- --benchmark_format=json > "$benchmark_output.startup.json" 2>&1 || true
    
    # Convert JSON to simple format for comparison
    local parsed_output="$BENCHMARK_OUTPUT_DIR/parsed_results.txt"
    > "$parsed_output"
    
    # Parse JSON outputs using jq (proper JSON parser)
    if command -v jq &> /dev/null; then
        for json_file in "$benchmark_output".*.json; do
            if [[ -f "$json_file" ]]; then
                # Extract benchmark name and real_time_mean from JSON using jq
                jq -r '.benchmarks[] | "\(.name):\(.real_time)"' "$json_file" >> "$parsed_output" 2>/dev/null || true
            fi
        done
    else
        log_error "jq is required for parsing benchmark JSON output. Please install jq."
        log_info "On macOS: brew install jq"
        log_info "On Linux: sudo apt-get install jq"
        exit 1
    fi
    
    # Try to load baseline
    if load_baseline; then
        # Compare with baseline
        if ! check_regression "$parsed_output"; then
            log_error "Performance regression detected!"
            exit 1
        fi
    else
        # No baseline exists, save current as baseline
        save_baseline "$parsed_output"
        log_info "Initial baseline created. Future runs will compare against this."
    fi
    
    log_success "Benchmark regression check completed"
}

# Check if required tools are available
if ! command -v bc &> /dev/null; then
    log_error "bc command not found. Please install bc for regression calculations."
    log_info "On macOS: bc is usually pre-installed"
    log_info "On Linux: sudo apt-get install bc"
    exit 1
fi

if ! command -v jq &> /dev/null; then
    log_error "jq command not found. Please install jq for JSON parsing."
    log_info "On macOS: brew install jq"
    log_info "On Linux: sudo apt-get install jq"
    exit 1
fi

main "$@"

