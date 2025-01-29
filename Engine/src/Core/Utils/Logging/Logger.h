#pragma once

#include <memory>
#include "Core\Core.h"
#include "spdlog\spdlog.h"

namespace Pawn {

	class ENGINE_API Logger
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_coreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_clientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> m_coreLogger;
		static std::shared_ptr<spdlog::logger> m_clientLogger;

	};

}

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