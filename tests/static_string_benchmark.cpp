#include <benchmark/benchmark.h>
#include "static_string.h"

static void BM_StackString_DefaultConstructor(benchmark::State& state) {
    for (auto _ : state) {
        gtr::containers::static_string<> s;
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StackString_DefaultConstructor);

static void BM_StackString_CStringConstructor(benchmark::State& state) {
    const char* test_str = "Hello, World!";
    for (auto _ : state) {
        gtr::containers::static_string<> s(test_str);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StackString_CStringConstructor);

static void BM_StackString_CopyConstructor(benchmark::State& state) {
    gtr::containers::static_string<> s1("Hello, World!");
    for (auto _ : state) {
        gtr::containers::static_string<> s2(s1);
        benchmark::DoNotOptimize(s2);
    }
}
BENCHMARK(BM_StackString_CopyConstructor);

static void BM_StackString_MoveConstructor(benchmark::State& state) {
    for (auto _ : state) {
        gtr::containers::static_string<> s1("Hello, World!");
        gtr::containers::static_string<> s2(std::move(s1));
        benchmark::DoNotOptimize(s2);
    }
}
BENCHMARK(BM_StackString_MoveConstructor);

static void BM_StackString_AppendCString(benchmark::State& state) {
    for (auto _ : state) {
        gtr::containers::static_string<> s;
        s.append("Hello, World!");
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StackString_AppendCString);

static void BM_StackString_AppendChar(benchmark::State& state) {
    for (auto _ : state) {
        gtr::containers::static_string<> s;
        s.append('A');
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StackString_AppendChar);

static void BM_StackString_Find(benchmark::State& state) {
    gtr::containers::static_string<> s("Hello, World!");
    for (auto _ : state) {
        auto pos = s.find("World");
        benchmark::DoNotOptimize(pos);
    }
}
BENCHMARK(BM_StackString_Find);

BENCHMARK_MAIN();
