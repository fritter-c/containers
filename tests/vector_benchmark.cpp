#include <benchmark/benchmark.h>
#include "vector.h"          // Your custom vector implementation
#include <vector>            // Standard library vector

using namespace gtr::containers;

// ------------------------
// Benchmarks for gtr::containers::vector
// ------------------------

// Benchmark for default constructor
static void BM_GTR_Vector_DefaultConstructor(benchmark::State& state) {
    for (auto _ : state) {
        vector<int> v;
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_GTR_Vector_DefaultConstructor);

// Benchmark for push_back operation
static void BM_GTR_Vector_PushBack(benchmark::State& state) {
    vector<int> v;
    for (auto _ : state) {
        v.push_back(42);
    }
    benchmark::DoNotOptimize(v);
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GTR_Vector_PushBack)->Range(8, 8 << 10);

// Benchmark for reserve operation
static void BM_GTR_Vector_Reserve(benchmark::State& state) {
    for (auto _ : state) {
        vector<int> v;
        v.reserve(state.range(0));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_GTR_Vector_Reserve)->Range(8, 8 << 10);

// Benchmark for copy constructor
static void BM_GTR_Vector_CopyConstructor(benchmark::State& state) {
    vector<int> v_original(state.range(0));
    // Fill the vector with values
    for (size_t i = 0; i < state.range(0); ++i) {
        v_original.push_back(42);
    }
    for (auto _ : state) {
        vector<int> v_copy(v_original);
        benchmark::DoNotOptimize(v_copy);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GTR_Vector_CopyConstructor)->Range(8, 8 << 10);

// Benchmark for move constructor
static void BM_GTR_Vector_MoveConstructor(benchmark::State& state) {
    for (auto _ : state) {
        vector<int> v_original(state.range(0));
        for (size_t i = 0; i < state.range(0); ++i) {
            v_original.push_back(42);
        }
        vector<int> v_moved(std::move(v_original));
        benchmark::DoNotOptimize(v_moved);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GTR_Vector_MoveConstructor)->Range(8, 8 << 10);

// Benchmark for element access
static void BM_GTR_Vector_ElementAccess(benchmark::State& state) {
    vector<int> v(state.range(0));
    // Fill the vector with values
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    int sum = 0;
    for (auto _ : state) {
        for (size_t i = 0; i < v.size(); ++i) {
            sum += v[i];
        }
    }
    benchmark::DoNotOptimize(sum);
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GTR_Vector_ElementAccess)->Range(8, 8 << 10);

// Benchmark for clear operation
static void BM_GTR_Vector_Clear(benchmark::State& state) {
    vector<int> v(state.range(0));
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    for (auto _ : state) {
        v.clear();
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GTR_Vector_Clear)->Range(8, 8 << 10);

// Benchmark for pop_back operation
static void BM_GTR_Vector_PopBack(benchmark::State& state) {
    vector<int> v(state.range(0));
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    for (auto _ : state) {
        if (v.size() > 0) {
            v.pop_back();
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GTR_Vector_PopBack)->Range(8, 8 << 10);

// Benchmark for emplace_back operation
static void BM_GTR_Vector_EmplaceBack(benchmark::State& state) {
    vector<int> v;
    for (auto _ : state) {
        v.emplace_back(42);
    }
    benchmark::DoNotOptimize(v);
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GTR_Vector_EmplaceBack)->Range(8, 8 << 10);

// ------------------------
// Benchmarks for std::vector
// ------------------------

// Benchmark for default constructor
static void BM_STD_Vector_DefaultConstructor(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_STD_Vector_DefaultConstructor);

// Benchmark for push_back operation
static void BM_STD_Vector_PushBack(benchmark::State& state) {
    std::vector<int> v;
    for (auto _ : state) {
        v.push_back(42);
    }
    benchmark::DoNotOptimize(v);
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_STD_Vector_PushBack)->Range(8, 8 << 10);

// Benchmark for reserve operation
static void BM_STD_Vector_Reserve(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(state.range(0));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_STD_Vector_Reserve)->Range(8, 8 << 10);

// Benchmark for copy constructor
static void BM_STD_Vector_CopyConstructor(benchmark::State& state) {
    std::vector<int> v_original(state.range(0));
    // Fill the vector with values
    for (size_t i = 0; i < state.range(0); ++i) {
        v_original.push_back(42);
    }
    for (auto _ : state) {
        std::vector<int> v_copy(v_original);
        benchmark::DoNotOptimize(v_copy);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_STD_Vector_CopyConstructor)->Range(8, 8 << 10);

// Benchmark for move constructor
static void BM_STD_Vector_MoveConstructor(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v_original(state.range(0));
        for (size_t i = 0; i < state.range(0); ++i) {
            v_original.push_back(42);
        }
        std::vector<int> v_moved(std::move(v_original));
        benchmark::DoNotOptimize(v_moved);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_STD_Vector_MoveConstructor)->Range(8, 8 << 10);

// Benchmark for element access
static void BM_STD_Vector_ElementAccess(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    // Fill the vector with values
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    int sum = 0;
    for (auto _ : state) {
        for (size_t i = 0; i < v.size(); ++i) {
            sum += v[i];
        }
    }
    benchmark::DoNotOptimize(sum);
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_STD_Vector_ElementAccess)->Range(8, 8 << 10);

// Benchmark for clear operation
static void BM_STD_Vector_Clear(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    for (auto _ : state) {
        v.clear();
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_STD_Vector_Clear)->Range(8, 8 << 10);

// Benchmark for pop_back operation
static void BM_STD_Vector_PopBack(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    for (size_t i = 0; i < state.range(0); ++i) {
        v.push_back(42);
    }
    for (auto _ : state) {
        if (v.size() > 0) {
            v.pop_back();
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_STD_Vector_PopBack)->Range(8, 8 << 10);

// Benchmark for emplace_back operation
static void BM_STD_Vector_EmplaceBack(benchmark::State& state) {
    std::vector<int> v;
    for (auto _ : state) {
        v.emplace_back(42);
    }
    benchmark::DoNotOptimize(v);
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_STD_Vector_EmplaceBack)->Range(8, 8 << 10);

// ------------------------
// Main function to run benchmarks
// ------------------------
BENCHMARK_MAIN();
