#include "Logger.h"
#include "spdlog\sinks\stdout_color_sinks.h"

#include <locale>

namespace Pawn {

	std::shared_ptr<spdlog::logger> Logger::m_coreLogger;
	std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
	std::shared_ptr<spdlog::logger> Logger::m_memoryLogger;
	std::shared_ptr<spdlog::logger> Logger::m_benchmarkLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		m_coreLogger = spdlog::stdout_color_mt("Core");
		m_coreLogger->set_level(spdlog::level::trace);

		m_clientLogger = spdlog::stdout_color_mt("Client");
		m_clientLogger->set_level(spdlog::level::trace);

		m_memoryLogger = spdlog::stdout_color_mt("HeapMemory");
		m_memoryLogger->set_level(spdlog::level::trace);

		m_benchmarkLogger = spdlog::stdout_color_mt("Benchmark");
		m_benchmarkLogger->set_level(spdlog::level::trace);
	}
}