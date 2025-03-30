#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"

import Pawn.Core.Memory;

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

#ifndef PE_DEBUG

// Core logging macros
#define PE_CORE_TRACE(...)		::Pawn::Core::Utils::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define PE_CORE_INFO(...)		::Pawn::Core::Utils::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define PE_CORE_WARN(...)		::Pawn::Core::Utils::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define PE_CORE_ERROR(...)		::Pawn::Core::Utils::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define PE_CORE_CRITICAL(...)	::Pawn::Core::Utils::Logger::GetCoreLogger()->critical(__VA_ARGS__)

//Client logging macros
#define PE_TRACE(...)			::Pawn::Core::Utils::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define PE_INFO(...)			::Pawn::Core::Utils::Logger::GetClientLogger()->info(__VA_ARGS__)
#define PE_WARN(...)			::Pawn::Core::Utils::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define PE_ERROR(...)			::Pawn::Core::Utils::Logger::GetClientLogger()->error(__VA_ARGS__)
#define PE_CRITICAL(...)		::Pawn::Core::Utils::Logger::GetClientLogger()->critical(__VA_ARGS__)

//Memory logging macros
#define PE_MEM_TRACE(...)		::Pawn::Core::Utils::Logger::GetMemoryLogger()->trace(__VA_ARGS__)

//Benchmark logging macros
#define PE_BENCHMARK_LOG(...)	::Pawn::Core::Utils::Logger::GetBenchmarkLogger()->info(__VA_ARGS__)

#else

// Core logging macros
#define PE_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(::Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_INFO(...)		SPDLOG_LOGGER_INFO(::Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_WARN(...)		SPDLOG_LOGGER_WARN(::Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(::Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(::Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)

//Client logging macros
#define PE_TRACE(...)			SPDLOG_LOGGER_TRACE(::Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_INFO(...)			SPDLOG_LOGGER_INFO(::Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_WARN(...)			SPDLOG_LOGGER_WARN(::Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_ERROR(...)			SPDLOG_LOGGER_ERROR(::Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(::Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)

//Memory logging macros
#define PE_MEM_TRACE(...)		SPDLOG_LOGGER_TRACE(::Pawn::Core::Utils::Logger::GetMemoryLogger(), __VA_ARGS__)

//Benchmark logging macros
#define PE_BENCHMARK_LOG(...)	SPDLOG_LOGGER_INFO(::Pawn::Core::Utils::Logger::GetBenchmarkLogger(), __VA_ARGS__)

#endif
