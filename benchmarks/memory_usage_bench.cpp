#include <benchmark/benchmark.h>
#include <obsidian/obsidian.h>
#include <obsidian/window.h>
#include <obsidian/button.h>
#include <cstdlib>

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/task_info.h>
#endif

// Helper function to get current memory usage (platform-specific)
size_t GetCurrentMemoryUsage() {
#ifdef __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t size = TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(),
                                    TASK_BASIC_INFO,
                                    (task_info_t)&info,
                                    &size);
    if (kerr == KERN_SUCCESS) {
        return info.resident_size;
    }
    return 0;
#elif defined(__linux__)
    // On Linux, read from /proc/self/status
    FILE* file = fopen("/proc/self/status", "r");
    if (!file) return 0;
    
    char line[128];
    size_t memory = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %zu kB", &memory);
            memory *= 1024; // Convert to bytes
            break;
        }
    }
    fclose(file);
    return memory;
#else
    return 0;
#endif
}

// Benchmark: Memory usage for window creation
static void BM_WindowMemoryUsage(benchmark::State& state) {
    size_t baseline_memory = GetCurrentMemoryUsage();
    
    for (auto _ : state) {
        obsidian::Window window;
        window.create(800, 600, "Memory Test");
        
        size_t current_memory = GetCurrentMemoryUsage();
        size_t memory_used = current_memory > baseline_memory 
            ? current_memory - baseline_memory 
            : 0;
        
        state.counters["memory_bytes"] = benchmark::Counter(
            static_cast<double>(memory_used),
            benchmark::Counter::kAvgIterations);
        
        window.close();
    }
}
BENCHMARK(BM_WindowMemoryUsage);

// Benchmark: Memory usage for multiple windows
static void BM_MultipleWindowMemoryUsage(benchmark::State& state) {
    size_t baseline_memory = GetCurrentMemoryUsage();
    
    for (auto _ : state) {
        std::vector<obsidian::Window> windows;
        windows.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            obsidian::Window window;
            window.create(800, 600, "Window " + std::to_string(i));
            windows.push_back(std::move(window));
        }
        
        size_t current_memory = GetCurrentMemoryUsage();
        size_t memory_used = current_memory > baseline_memory 
            ? current_memory - baseline_memory 
            : 0;
        
        state.counters["memory_bytes"] = benchmark::Counter(
            static_cast<double>(memory_used),
            benchmark::Counter::kAvgIterations);
        
        // Cleanup (automatic via RAII)
        for (auto& window : windows) {
            window.close();
        }
    }
}
BENCHMARK(BM_MultipleWindowMemoryUsage)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(20);

BENCHMARK_MAIN();

