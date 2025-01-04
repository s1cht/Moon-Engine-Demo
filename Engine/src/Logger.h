#pragma once

#include <memory>
#include "Core.h"
#include "spdlog\spdlog.h"

namespace Pawn {

	class PAWN_API Logger
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
#define PE_CORE_FATAL(...)		::Pawn::Logger::GetCoreLogger()->fatal(__VA_ARGS__)

//Client logging macros

#define PE_CLIENT_TRACE(...)	::Pawn::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define PE_CLIENT_INFO(...)		::Pawn::Logger::GetClientLogger()->info(__VA_ARGS__)
#define PE_CLIENT_WARN(...)		::Pawn::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define PE_CLIENT_ERROR(...)	::Pawn::Logger::GetClientLogger()->error(__VA_ARGS__)
#define PE_CLIENT_FATAL(...)	::Pawn::Logger::GetClientLogger()->fatal(__VA_ARGS__)