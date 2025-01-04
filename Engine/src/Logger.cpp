#include "Logger.h"
#include "spdlog\sinks\stdout_color_sinks.h"

namespace Pawn {

	std::shared_ptr<spdlog::logger> Logger::m_coreLogger;
	std::shared_ptr<spdlog::logger> Logger::m_clientLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_coreLogger = spdlog::stdout_color_mt("Core");
		m_coreLogger->set_level(spdlog::level::trace);

		m_clientLogger = spdlog::stdout_color_mt("Client");
		m_clientLogger->set_level(spdlog::level::trace);
	}

}
