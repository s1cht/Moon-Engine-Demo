#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Pawn {

	class CORE_API Logger
	{
	public:
		static void Init();
		static void Shutdown();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_coreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_clientLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetMemoryLogger() { return m_memoryLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetBenchmarkLogger() { return m_benchmarkLogger; };

	private:
		static std::shared_ptr<spdlog::logger> m_coreLogger;
		static std::shared_ptr<spdlog::logger> m_clientLogger;
		static std::shared_ptr<spdlog::logger> m_memoryLogger;
		static std::shared_ptr<spdlog::logger> m_benchmarkLogger;

	};

}

#ifndef PE_DEBUG

// Core logging macros
#define PE_CORE_TRACE(...)		::Pawn::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define PE_CORE_INFO(...)		::Pawn::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define PE_CORE_WARN(...)		::Pawn::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define PE_CORE_ERROR(...)		::Pawn::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define PE_CORE_CRITICAL(...)	::Pawn::Logger::GetCoreLogger()->critical(__VA_ARGS__)

//Client logging macros
#define PE_TRACE(...)			::Pawn::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define PE_INFO(...)			::Pawn::Logger::GetClientLogger()->info(__VA_ARGS__)
#define PE_WARN(...)			::Pawn::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define PE_ERROR(...)			::Pawn::Logger::GetClientLogger()->error(__VA_ARGS__)
#define PE_CRITICAL(...)		::Pawn::Logger::GetClientLogger()->critical(__VA_ARGS__)

//Memory logging macros
#define PE_MEM_TRACE(...)		::Pawn::Logger::GetMemoryLogger()->trace(__VA_ARGS__)

//Benchmark logging macros
#define PE_BENCHMARK_LOG(...)	::Pawn::Logger::GetBenchmarkLogger()->info(__VA_ARGS__)

#else

// Core logging macros
#define PE_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(::Pawn::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_INFO(...)		SPDLOG_LOGGER_INFO(::Pawn::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_WARN(...)		SPDLOG_LOGGER_WARN(::Pawn::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(::Pawn::Logger::GetCoreLogger(), __VA_ARGS__)
#define PE_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(::Pawn::Logger::GetCoreLogger(), __VA_ARGS__)

//Client logging macros
#define PE_TRACE(...)			SPDLOG_LOGGER_TRACE(::Pawn::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_INFO(...)			SPDLOG_LOGGER_INFO(::Pawn::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_WARN(...)			SPDLOG_LOGGER_WARN(::Pawn::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_ERROR(...)			SPDLOG_LOGGER_ERROR(::Pawn::Logger::GetClientLogger(), __VA_ARGS__)
#define PE_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(::Pawn::Logger::GetClientLogger(), __VA_ARGS__)

//Memory logging macros
#define PE_MEM_TRACE(...)		SPDLOG_LOGGER_TRACE(::Pawn::Logger::GetMemoryLogger(), __VA_ARGS__)

//Benchmark logging macros
#define PE_BENCHMARK_LOG(...)	SPDLOG_LOGGER_INFO(::Pawn::Logger::GetBenchmarkLogger(), __VA_ARGS__)

#endif
