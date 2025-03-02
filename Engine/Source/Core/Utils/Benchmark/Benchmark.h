#pragma once

#include "Core/CoreTypes.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/String/String.h"

#ifndef PLATFORM_WINDOWS

#include <chrono>CoreTypes
#define BNCHM_SEC(x) std::chrono::x

template<typename T>
class Benchmarker
{
public:
	Benchmarker(const ansichar* unit, const uchar* benchmarkName)
	{
		m_Unit = unit;
		m_BenchmarkName = benchmarkName;
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	~Benchmarker()
	{
		m_EndTime = std::chrono::high_resolution_clock::now();
		Stop();
	}

private:
	void Stop();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;
	const uchar* m_BenchmarkName;
	const ansichar* m_Unit;

};

template<typename T>
inline void Benchmarker<T>::Stop()
{
	auto start = std::chrono::time_point_cast<T>(m_StartTime).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<T>(m_EndTime).time_since_epoch().count();

	auto duration = end - start;
	auto a3 = reinterpret_cast<const ansichar*>(m_BenchmarkName);

	PE_TRACE("Benchmark {} ended! Last: {} {}", a3, duration, m_Unit);
}

#define Benchmark(unit, name) Benchmarker<BNCHM_SEC(unit)>(#unit, name);

#else

class Benchmarker
{
public:
    Benchmarker(const ansichar* unit, const uchar* benchmarkName, SIZE_T iterCount)
        : m_Unit(unit), m_BenchmarkName(benchmarkName), m_IterCount(iterCount)
    {
        QueryPerformanceFrequency(&m_Frequency);
        QueryPerformanceCounter(&m_StartTime);
    }

    ~Benchmarker()
    {
        QueryPerformanceCounter(&m_EndTime);
        Stop();
    }

private:
    void Stop()
    {
        // Вычисляем разницу в тиках и переводим в наносекунды
        int64 elapsedTicks = m_EndTime.QuadPart - m_StartTime.QuadPart;
        float64 durationNs = (elapsedTicks * 1e9) / m_Frequency.QuadPart;
        float64 oneIteration = (m_IterCount > 0) ? durationNs / (float64)m_IterCount : 0;

        if (m_IterCount > 1)
            PE_BENCHMARK_LOG("Scope {} with {} iterations, lasted: {:.3f} {}. Average per iteration: {:.5f} {}", PE_LOG_STR(m_BenchmarkName), m_IterCount, 
                durationNs, m_Unit, 
                oneIteration, m_Unit);
        else
            PE_BENCHMARK_LOG("Scope {}, lasted: {:.3f} {}", PE_LOG_STR(m_BenchmarkName), durationNs, m_Unit);
    }

private:
    LARGE_INTEGER m_StartTime{};
    LARGE_INTEGER m_EndTime{};
    LARGE_INTEGER m_Frequency{};

    const uchar* m_BenchmarkName;
    const ansichar* m_Unit;
    SIZE_T m_IterCount;
};

#define Benchmark(unit, name) auto _BENCHMARK = Benchmarker("nanoseconds", name, 0);
#define IterationBenchmark(unit, name, iterCount) auto _BENCHMARK = Benchmarker("nanoseconds", name, iterCount);

#endif