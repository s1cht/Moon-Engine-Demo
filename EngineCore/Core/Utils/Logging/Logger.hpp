#pragma once

#include "Core.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ostr.h>


namespace Pawn::Core::Utils 
{
	class CORE_API Logger
	{
	public:
		static void Init();
		static void Shutdown();

		inline static Pawn::Core::Memory::Reference<spdlog::logger>& GetCoreLogger() { return m_coreLogger; };
		inline static Pawn::Core::Memory::Reference<spdlog::logger>& GetClientLogger() { return m_clientLogger; };
		inline static Pawn::Core::Memory::Reference<spdlog::logger>& GetMemoryLogger() { return m_memoryLogger; };
		inline static Pawn::Core::Memory::Reference<spdlog::logger>& GetBenchmarkLogger() { return m_benchmarkLogger; };

	private:
		static Pawn::Core::Memory::Reference<spdlog::logger> m_coreLogger;
		static Pawn::Core::Memory::Reference<spdlog::logger> m_clientLogger;
		static Pawn::Core::Memory::Reference<spdlog::logger> m_memoryLogger;
		static Pawn::Core::Memory::Reference<spdlog::logger> m_benchmarkLogger;
	};

}