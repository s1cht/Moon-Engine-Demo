#pragma once

#include <chrono>
#include "Core/Types.h"
#include "Core/Utils/Logging/Logger.h"

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