#pragma once

// Bitwise operations

#define BIT(x) (1 << x)

// Attrubutes

#define ME_DEPRECATED [[deprecated]]
#define ME_DEPRECATED_M(text) [[deprecated(text)]]

#define ME_NODISCARD [[nodiscard]]

// Misc

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define ME_FAILED_IO(result) (result != ME::IO::IOError::OK);
#define ME_FAILED_IO_INT(result) (result != (int32)ME::IO::IOError::OK);

#ifndef ME_DEBUG
	// Core logging macros
	#define ME_CORE_TRACE(...)			ME::Core::Utils::Logger::GetCoreLogger()->trace(__VA_ARGS__)
	#define ME_CORE_INFO(...)			ME::Core::Utils::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define ME_CORE_WARN(...)			ME::Core::Utils::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define ME_CORE_ERROR(...)			ME::Core::Utils::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define ME_CORE_CRITICAL(...)		ME::Core::Utils::Logger::GetCoreLogger()->critical(__VA_ARGS__)

	//Client logging macros
	#define ME_TRACE(...)				ME::Core::Utils::Logger::GetClientLogger()->trace(__VA_ARGS__)
	#define ME_INFO(...)				ME::Core::Utils::Logger::GetClientLogger()->info(__VA_ARGS__)
	#define ME_WARN(...)				ME::Core::Utils::Logger::GetClientLogger()->warn(__VA_ARGS__)
	#define ME_ERROR(...)				ME::Core::Utils::Logger::GetClientLogger()->error(__VA_ARGS__)
	#define ME_CRITICAL(...)			ME::Core::Utils::Logger::GetClientLogger()->critical(__VA_ARGS__)

	//Render logging macros
	#define ME_RENDER_TRACE(...)		ME::Core::Utils::Logger::GetRenderLogger()->trace(__VA_ARGS__)
	#define ME_RENDER_INFO(...)			ME::Core::Utils::Logger::GetRenderLogger()->info(__VA_ARGS__)
	#define ME_RENDER_WARN(...)			ME::Core::Utils::Logger::GetRenderLogger()->warn(__VA_ARGS__)
	#define ME_RENDER_ERROR(...)		ME::Core::Utils::Logger::GetRenderLogger()->error(__VA_ARGS__)
	#define ME_RENDER_CRITICAL(...)		ME::Core::Utils::Logger::GetRenderLogger()->critical(__VA_ARGS__)
	
	//Memory logging macros
	#define ME_MEM_TRACE(...)		ME::Core::Utils::Logger::GetMemoryLogger()->trace(__VA_ARGS__)
	
	//Benchmark logging macros
	#define ME_BENCHMARK_LOG(...)	ME::Core::Utils::Logger::GetBenchmarkLogger()->info(__VA_ARGS__)
#else
	// Core logging macros
	#define ME_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(ME::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define ME_CORE_INFO(...)		SPDLOG_LOGGER_INFO(ME::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define ME_CORE_WARN(...)		SPDLOG_LOGGER_WARN(ME::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define ME_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(ME::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	#define ME_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(ME::Core::Utils::Logger::GetCoreLogger(), __VA_ARGS__)
	
	//Client logging macros
	#define ME_TRACE(...)			SPDLOG_LOGGER_TRACE(ME::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define ME_INFO(...)			SPDLOG_LOGGER_INFO(ME::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define ME_WARN(...)			SPDLOG_LOGGER_WARN(ME::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define ME_ERROR(...)			SPDLOG_LOGGER_ERROR(ME::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)
	#define ME_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(ME::Core::Utils::Logger::GetClientLogger(), __VA_ARGS__)

	//Render logging macros
	#define ME_RENDER_TRACE(...)		ME::Core::Utils::Logger::GetRenderLogger()->trace(__VA_ARGS__)
	#define ME_RENDER_INFO(...)			ME::Core::Utils::Logger::GetRenderLogger()->info(__VA_ARGS__)
	#define ME_RENDER_WARN(...)			ME::Core::Utils::Logger::GetRenderLogger()->warn(__VA_ARGS__)
	#define ME_RENDER_ERROR(...)		ME::Core::Utils::Logger::GetRenderLogger()->error(__VA_ARGS__)
	#define ME_RENDER_CRITICAL(...)		ME::Core::Utils::Logger::GetRenderLogger()->critical(__VA_ARGS__)

	//Memory logging macros
	#define ME_MEM_TRACE(...)		SPDLOG_LOGGER_TRACE(ME::Core::Utils::Logger::GetMemoryLogger(), __VA_ARGS__)
	
	//Benchmark logging macros
	#define ME_BENCHMARK_LOG(...)	SPDLOG_LOGGER_INFO(ME::Core::Utils::Logger::GetBenchmarkLogger(), __VA_ARGS__)

#endif

#define ME_FMT_FORMATTER_VALUE(val) obj.val
#define ME_FMT_FORMATTER(type, str, ...)														\
namespace fmt																					\
{																								\
	template <> struct formatter<type, char>													\
	{																							\
	    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }					\
	    template <typename FormatContext>														\
	    auto format(const type& obj, FormatContext& ctx) const									\
		{																						\
	        return format_to(ctx.out(), str, __VA_ARGS__);										\
	    }																						\
	};																							\
	template <> struct formatter<type, wchar_t>													\
	{																							\
	    constexpr auto parse(basic_format_parse_context<wchar_t>& ctx) { return ctx.begin(); }	\
	    template <typename FormatContext>														\
	    auto format(const type& obj, FormatContext& ctx) const									\
		{																						\
	        return format_to(ctx.out(), L##str, __VA_ARGS__);									\
	    }																						\
	};																							\
}


// Assertion
#ifdef ME_DEBUG
	#define ME_ASSERT(x, ...)			\
	do {								\
		if (!(x))						\
		{								\
			ME_CRITICAL(__VA_ARGS__);	\
			__debugbreak();				\
		}								\
	} while (0)

	#define ME_CORE_ASSERT(x, ...)			\
	do {									\
		if (!(x))							\
		{									\
			ME_CORE_CRITICAL(__VA_ARGS__);	\
			__debugbreak();					\
		}									\
	} while (0)
#else
	#define ME_ASSERT(x, ...)
	#define ME_CORE_ASSERT(x, ...)
#endif

#include "Core/Events/EventMacros.hpp"
#include "Core/Containers/String/StringMacros.hpp"
