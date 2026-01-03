#include <benchmark/benchmark.h>
#include <obsidian/obsidian.h>
#include <obsidian/app.h>

// Benchmark: Application initialization time
static void BM_AppInitialization(benchmark::State& state) {
    for (auto _ : state) {
        obsidian::App app;
        
        // Measure initialization time
        bool initialized = app.initialize();
        benchmark::DoNotOptimize(initialized);
        
        // Cleanup
        state.PauseTiming();
        if (initialized) {
            app.shutdown();
        }
        state.ResumeTiming();
    }
}
BENCHMARK(BM_AppInitialization);

// Benchmark: Full application lifecycle
static void BM_AppLifecycle(benchmark::State& state) {
    for (auto _ : state) {
        obsidian::App app;
        
        if (!app.initialize()) {
            continue;
        }
        
        // Set up callbacks
        obsidian::AppCallbacks callbacks;
        bool init_called = false;
        bool update_called = false;
        bool shutdown_called = false;
        
        callbacks.onInit = [&init_called]() { init_called = true; };
        callbacks.onUpdate = [&update_called]() { update_called = true; };
        callbacks.onShutdown = [&shutdown_called]() { shutdown_called = true; };
        
        // Simulate one update cycle (non-blocking)
        benchmark::DoNotOptimize(init_called);
        benchmark::DoNotOptimize(update_called);
        
        // Cleanup
        state.PauseTiming();
        app.shutdown();
        state.ResumeTiming();
        
        benchmark::DoNotOptimize(shutdown_called);
    }
}
BENCHMARK(BM_AppLifecycle);

BENCHMARK_MAIN();

