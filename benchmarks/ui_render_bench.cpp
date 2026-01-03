#include <benchmark/benchmark.h>
#include <obsidian/obsidian.h>
#include <obsidian/window.h>
#include <obsidian/button.h>

// Benchmark: Button creation
static void BM_ButtonCreation(benchmark::State& state) {
    obsidian::Window window;
    window.create(800, 600, "Benchmark Window");
    
    for (auto _ : state) {
        obsidian::Button button;
        button.create("Test Button", 10, 10, 100, 30);
        button.addToWindow(window);
        
        // Cleanup
        state.PauseTiming();
        button.removeFromParent();
        state.ResumeTiming();
    }
    
    window.close();
}
BENCHMARK(BM_ButtonCreation);

// Benchmark: Multiple button creation
static void BM_MultipleButtonCreation(benchmark::State& state) {
    obsidian::Window window;
    window.create(800, 600, "Benchmark Window");
    
    for (auto _ : state) {
        std::vector<obsidian::Button> buttons;
        buttons.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            obsidian::Button button;
            button.create("Button " + std::to_string(i), 10 + (i % 10) * 80, 10 + (i / 10) * 40, 70, 30);
            button.addToWindow(window);
            buttons.push_back(std::move(button));
        }
        
        // Cleanup
        state.PauseTiming();
        for (auto& button : buttons) {
            button.removeFromParent();
        }
        state.ResumeTiming();
    }
    
    window.close();
}
BENCHMARK(BM_MultipleButtonCreation)
    ->Arg(10)
    ->Arg(50)
    ->Arg(100)
    ->Arg(500);

// Benchmark: Button property access
static void BM_ButtonPropertyAccess(benchmark::State& state) {
    obsidian::Window window;
    window.create(800, 600, "Test Window");
    obsidian::Button button;
    button.create("Test Button", 10, 10, 100, 30);
    button.addToWindow(window);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(button.getTitle());
        benchmark::DoNotOptimize(button.isEnabled());
        benchmark::DoNotOptimize(button.isVisible());
    }
    
    button.removeFromParent();
    window.close();
}
BENCHMARK(BM_ButtonPropertyAccess);

BENCHMARK_MAIN();

