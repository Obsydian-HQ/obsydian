#include <benchmark/benchmark.h>
#include <obsidian/obsidian.h>
#include <obsidian/window.h>

// Benchmark: Window creation and destruction
static void BM_WindowCreation(benchmark::State& state) {
    for (auto _ : state) {
        // Create a window using the actual API
        obsidian::Window window;
        window.create(800, 600, "Benchmark Window");
        
        // Window is automatically destroyed when it goes out of scope
        // Pause timing for cleanup
        state.PauseTiming();
        window.close();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_WindowCreation);

// Benchmark: Multiple window creation
static void BM_MultipleWindowCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<obsidian::Window> windows;
        windows.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            obsidian::Window window;
            window.create(800, 600, "Window " + std::to_string(i));
            windows.push_back(std::move(window));
        }
        
        // Cleanup (automatic via RAII)
        state.PauseTiming();
        for (auto& window : windows) {
            window.close();
        }
        state.ResumeTiming();
    }
}
BENCHMARK(BM_MultipleWindowCreation)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(50);

// Benchmark: Window property access
static void BM_WindowPropertyAccess(benchmark::State& state) {
    obsidian::Window window;
    window.create(800, 600, "Test Window");
    
    for (auto _ : state) {
        // Access window properties
        benchmark::DoNotOptimize(window.isValid());
        benchmark::DoNotOptimize(window.getNativeHandle());
    }
    
    window.close();
}
BENCHMARK(BM_WindowPropertyAccess);

BENCHMARK_MAIN();

