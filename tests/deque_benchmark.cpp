#include <benchmark/benchmark.h>
#include <deque>
#include "deque.h"

static void BM_deque_push_back(benchmark::State &state) {
    for (auto _ : state) {
        gtr::containers::deque<int> d;
        d.initialize_map();
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
    }
}

static void BM_std_deque_push_back(benchmark::State &state) {
    for (auto _ : state) {
        std::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
    }
}

static void BM_deque_push_front(benchmark::State &state) {
    for (auto _ : state) {
        gtr::containers::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_front(i);
        }
    }
}

static void BM_std_deque_push_front(benchmark::State &state) {
    for (auto _ : state) {
        std::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_front(i);
        }
    }
}

static void BM_deque_pop_back(benchmark::State &state) {
    for (auto _ : state) {
        gtr::containers::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            d.pop_back();
        }
    }
}

static void BM_std_deque_pop_back(benchmark::State &state) {
    for (auto _ : state) {
        std::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            d.pop_back();
        }
    }
}

static void BM_deque_pop_front(benchmark::State &state) {
    for (auto _ : state) {
        gtr::containers::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            d.pop_front();
        }
    }
}

static void BM_std_deque_pop_front(benchmark::State &state) {
    for (auto _ : state) {
        std::deque<int> d;
        for (int i = 0; i < state.range(0); ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < state.range(0); ++i) {
            d.pop_front();
        }
    }
}

BENCHMARK(BM_deque_push_back)->Range(8, 8 << 10);
BENCHMARK(BM_std_deque_push_back)->Range(8, 8 << 10);
BENCHMARK(BM_deque_push_front)->Range(8, 8 << 10);
BENCHMARK(BM_std_deque_push_front)->Range(8, 8 << 10);
BENCHMARK(BM_deque_pop_back)->Range(8, 8 << 10);
BENCHMARK(BM_std_deque_pop_back)->Range(8, 8 << 10);
BENCHMARK(BM_deque_pop_front)->Range(8, 8 << 10);
BENCHMARK(BM_std_deque_pop_front)->Range(8, 8 << 10);

BENCHMARK_MAIN();

