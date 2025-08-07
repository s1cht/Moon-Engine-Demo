#include "Logger.hpp"

namespace ME::Core::Utils
{
	ME::Core::Memory::Reference<spdlog::logger> Logger::m_CoreLogger;
	ME::Core::Memory::Reference<spdlog::logger> Logger::m_ClientLogger;
	ME::Core::Memory::Reference<spdlog::logger> Logger::m_RenderLogger;
	ME::Core::Memory::Reference<spdlog::logger> Logger::m_MemoryLogger;
	ME::Core::Memory::Reference<spdlog::logger> Logger::m_BenchmarkLogger;

	void Logger::Init()
	{
#ifndef ME_DEBUG
		spdlog::set_pattern("%^[%H:%I:%M:%S:%e ThreadID: %t] %n: %$ %v");
#else
		spdlog::set_pattern("%^[%H:%I:%M:%S:%e ThreadID: %t] [%@] %n: %$ %v");
#endif

		m_CoreLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Core");
		m_ClientLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Client");
		m_RenderLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Render");
		m_MemoryLogger = spdlog::stdout_color_mt<spdlog::default_factory>("HeapMemory");
		m_BenchmarkLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Benchmark");

		m_RenderLogger->set_pattern("%^[%H:%I:%M:%S] %n: %$ %v");

#ifdef ME_DEBUG
		m_CoreLogger->set_level(spdlog::level::level_enum::trace);
		m_ClientLogger->set_level(spdlog::level::level_enum::trace);
		m_RenderLogger->set_level(spdlog::level::level_enum::trace);
		m_MemoryLogger->set_level(spdlog::level::level_enum::trace);
		m_BenchmarkLogger->set_level(spdlog::level::level_enum::trace);
#else
		m_CoreLogger->set_level(spdlog::level::info);
		m_ClientLogger->set_level(spdlog::level::info);
		m_RenderLogger->set_level(spdlog::level::info);
		m_MemoryLogger->set_level(spdlog::level::warn);
		m_BenchmarkLogger->set_level(spdlog::level::off);
#endif
	}

	void Logger::Shutdown()
	{
		m_CoreLogger.reset();
		m_ClientLogger.reset();
		m_MemoryLogger.reset();
		m_BenchmarkLogger.reset();
		spdlog::shutdown();
	}
}