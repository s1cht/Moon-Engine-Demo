#pragma once

#include "Core.hpp"
#include "Core/Memory/Memory.hpp"

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

namespace ME::Core::Utils 
{
	class CORE_API Logger
	{
	public:
		static void Init();
		static void Shutdown();

		inline static ME::Core::Memory::Reference<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static ME::Core::Memory::Reference<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		inline static ME::Core::Memory::Reference<spdlog::logger>& GetRenderLogger() { return m_RenderLogger; }
		inline static ME::Core::Memory::Reference<spdlog::logger>& GetMemoryLogger() { return m_MemoryLogger; }
		inline static ME::Core::Memory::Reference<spdlog::logger>& GetBenchmarkLogger() { return m_BenchmarkLogger; }

	private:
		static ME::Core::Memory::Reference<spdlog::logger> m_CoreLogger;
		static ME::Core::Memory::Reference<spdlog::logger> m_ClientLogger;
		static ME::Core::Memory::Reference<spdlog::logger> m_RenderLogger;
		static ME::Core::Memory::Reference<spdlog::logger> m_MemoryLogger;
		static ME::Core::Memory::Reference<spdlog::logger> m_BenchmarkLogger;
	};

}