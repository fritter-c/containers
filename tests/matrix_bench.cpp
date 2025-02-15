#include "benchmark/benchmark.h"
#include "matrix.h"
#include <vector>

using namespace gtr::containers;
using namespace std;
static void BM_MatrixAddition(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        matrix<double> a(size, 5);
        for (std::size_t i = 0; i < 5; i++) {
            auto buffer = a[i];
            for (std::size_t j = 0; j < size; j++) {
                buffer[j] = 1.0;
            }
        }
    }
}

BENCHMARK(BM_MatrixAddition)->Range(8 << 1, 8 << 16);

static void BM_MatrixContiguos(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        matrix<double> a(size, 5);
        for (std::size_t i = 0; i < size; i++) {
            a.data[i] = 1.0;
        }
    }
}

BENCHMARK(BM_MatrixContiguos)->Range(8 << 1, 8 << 16);

static void BM_StdMatrixAddition(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        state.PauseTiming();
        vector<vector<double, c_allocator<double>>, c_allocator<vector<double, c_allocator<double>>>> a;
        a.resize(5);
        a[0].resize(size);
        a[1].resize(size);
        a[2].resize(size);
        a[3].resize(size);
        a[4].resize(size);
        state.ResumeTiming();
        for (std::size_t i = 0; i < 5; i++) {
            for (std::size_t j = 0; j < size; j++) {
                a[i][j] = 1.0;
            }
        }
    }
}

BENCHMARK(BM_StdMatrixAddition)->Range(8 << 1, 8 << 16);

static void BM_MatrixBufferContiguos(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        matrix<double> a(size, 5);
        auto buffer = a[0];
        for (std::size_t i = 0; i < size; i++) {
            buffer[i] = 1.0;
        }
    }
}

BENCHMARK(BM_MatrixBufferContiguos)->Range(8 << 1, 8 << 16);

static void BM_StdMatrixBufferContiguos(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        vector<vector<double>> a;
        a.resize(5);
        a[0].resize(size);
        a[1].resize(size);
        a[2].resize(size);
        a[3].resize(size);
        a[4].resize(size);
        auto v = a[0];
        for (std::size_t j = 0; j < size; j++) {
            v[j] = 1.0;
        }
    }
}

BENCHMARK(BM_StdMatrixBufferContiguos)->Range(8 << 1, 8 << 16);

static void BM_RawMatrixAddition(benchmark::State &state) {
    for (auto _ : state) {
        std::size_t size = state.range(0);
        double **values;
        values = new double *[5];
        for (int i = 0; i < 5; i++) {
            values[i] = new double[size];
        }
        for (std::size_t i = 0; i < 5; i++) {
            for (std::size_t j = 0; j < size; j++) {
                values[i][j] = 1.0;
            }
        }
        for (int i = 0; i < 5; i++) {
            delete [] values[i];
        }
        delete [] values;
    }
}

BENCHMARK(BM_RawMatrixAddition)->Range(8 << 1, 8 << 16);

BENCHMARK_MAIN();
