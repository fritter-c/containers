#include "hashmap.h"
#include <benchmark/benchmark.h>
#include <unordered_map>

using gtr::containers::hashmap;

static void BM_UnorderedMapInsert(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.insert({i, i});
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_UnorderedMapInsert)->Range(1, 1 << 10)->Complexity(benchmark::o1);

static void BM_UnorderedMapFind(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.insert({i, i});
        }
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(map.find(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_UnorderedMapFind)->Range(1, 1 << 10)->Complexity(benchmark::o1);

static void BM_HashMapInsert(benchmark::State& state) {
    for (auto _ : state) {
        hashmap<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.add(i, i);
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_HashMapInsert)->Range(1, 1 << 10)->Complexity(benchmark::o1);

static void BM_HashMapFind(benchmark::State& state) {
    for (auto _ : state) {
        hashmap<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.add(i, i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(map.get(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_HashMapFind)->Range(1, 1 << 10)->Complexity(benchmark::o1);

static void BM_HashMapContains(benchmark::State& state) {
    for (auto _ : state) {
        hashmap<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.add(i, i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(map.contains(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_HashMapContains)->Range(1, 1 << 18)->Complexity(benchmark::o1);

static void BM_UnorderedMapContains(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> map;
        for (int i = 0; i < state.range(0); ++i) {
            map.insert({i, i});
        }
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(map.find(i) != map.end());
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_UnorderedMapContains)->Range(1, 1 << 18)->Complexity(benchmark::o1);

BENCHMARK_MAIN();
