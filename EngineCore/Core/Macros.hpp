#pragma once

// Bitwise operations

#define BIT(x) (1 << x)

// Attrubutes

#define PE_DEPRECATED [[deprecated]]
#define PE_DEPRECATED_M(text) [[deprecated(text)]]

#define PE_NODISCARD [[nodiscard]]

// Misc

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define PE_FAILED_IO(result) (result != Pawn::IO::IOError::OK);
#define PE_FAILED_IO_INT(result) (result != (int32)Pawn::IO::IOError::OK);

#if 0

#if 1
	//ifndef PE_DEBUG
	// Core logging macros
	#define PE_CORE_TRACE(...)		Pawn::Core::Utils::Logger::GetCoreLogger()->trace(__VA_ARGS__)
	#define PE_CORE_INFO(...)		Pawn::Core::Utils::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define PE_CORE_WARN(...)		Pawn::Core::Utils::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define PE_CORE_ERROR(...)		Pawn::Core::Utils::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define PE_CORE_CRITICAL(...)	Pawn::Core::Utils::Logger::GetCoreLogger()->critical(__VA_ARGS__)
	
	//Client logging macros
	#define PE_TRACE(...)			Pawn::Core::Utils::Logger::GetClientLogger()->trace(__VA_ARGS__)
	#define PE_INFO(...)			Pawn::Core::Utils::Logger::GetClientLogger()->info(__VA_ARGS__)
	#define PE_WARN(...)			Pawn::Core::Utils::Logger::GetClientLogger()->warn(__VA_ARGS__)
	#define PE_ERROR(...)			Pawn::Core::Utils::Logger::GetClientLogger()->error(__VA_ARGS__)
	#define PE_CRITICAL(...)		Pawn::Core::Utils::Logger::GetClientLogger()->critical(__VA_ARGS__)
	
	//Memory logging macros
	#define PE_MEM_TRACE(...)		Pawn::Core::Utils::Logger::GetMemoryLogger()->trace(__VA_ARGS__)
	
	//Benchmark logging macros
	#define PE_BENCHMARK_LOG(...)	Pawn::Core::Utils::Logger::GetBenchmarkLogger()->info(__VA_ARGS__)
#else
	// Core logging macros
	#define PE_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define PE_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define PE_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define PE_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define PE_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Pawn::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	
	//Client logging macros
	#define PE_TRACE(...)			SPDLOG_LOGGER_TRACE(Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define PE_INFO(...)			SPDLOG_LOGGER_INFO(Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define PE_WARN(...)			SPDLOG_LOGGER_WARN(Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define PE_ERROR(...)			SPDLOG_LOGGER_ERROR(Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define PE_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Pawn::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	
	//Memory logging macros
	#define PE_MEM_TRACE(...)		SPDLOG_LOGGER_TRACE(Pawn::Core::Utils::Logger::GetMemoryLogger(), __VA_ARGS__)
	
	//Benchmark logging macros
	#define PE_BENCHMARK_LOG(...)	SPDLOG_LOGGER_INFO(Pawn::Core::Utils::Logger::GetBenchmarkLogger(), __VA_ARGS__)
#endif

#else

	//ifndef PE_DEBUG
	// Core logging macros
	#define PE_CORE_TRACE(...)		
	#define PE_CORE_INFO(...)		
	#define PE_CORE_WARN(...)		
	#define PE_CORE_ERROR(...)		
	#define PE_CORE_CRITICAL(...)	
	
	//Client logging macros
	#define PE_TRACE(...)			
	#define PE_INFO(...)			
	#define PE_WARN(...)			
	#define PE_ERROR(...)			
	#define PE_CRITICAL(...)		
	
	//Memory logging macros
	#define PE_MEM_TRACE(...)		
	
	//Benchmark logging macros
	#define PE_BENCHMARK_LOG(...)	

#endif

// Assertion
#ifdef _DEBUG
	#define PE_ASSERT(x, ...)			\
	if (!(x))							\
	{									\
		PE_CRITICAL(__VA_ARGS__);		\
		__debugbreak();					\
	}										

	#define PE_CORE_ASSERT(x, ...)		\
	if (!(x))							\
	{									\
		PE_CORE_CRITICAL(__VA_ARGS__);	\
		__debugbreak();					\
	}
#else
	#define PE_ASSERT(x, ...)
	#define PE_CORE_ASSERT(x, ...)
#endif

#include "Core/Events/EventMacros.hpp"
#include "Core/Containers/String/StringMacros.hpp"
