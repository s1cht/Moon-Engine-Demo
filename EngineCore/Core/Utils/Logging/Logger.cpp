module Pawn.Core.Utils.Logger;

#include "Core.h"

import Pawn.Core;
import Pawn.Core.Memory;
import Pawn.Thirdparty.spdlog;

namespace Pawn::Core::Utils 
{
	Pawn::Core::Memory::Reference<spdlog::logger> Logger::m_coreLogger;
	Pawn::Core::Memory::Reference<spdlog::logger> Logger::m_clientLogger;
	Pawn::Core::Memory::Reference<spdlog::logger> Logger::m_memoryLogger;
	Pawn::Core::Memory::Reference<spdlog::logger> Logger::m_benchmarkLogger;

	void Logger::Init()
	{
#ifndef PE_DEBUG
		spdlog::set_pattern("%^[%H:%I:%M:%S:%e ThreadID: %t] %n: %$ %v");
#else
		spdlog::set_pattern("%^[%H:%I:%M:%S:%e ThreadID: %t] [%@] %n: %$ %v");
#endif

		m_coreLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Core");
		m_clientLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Client");
		m_memoryLogger = spdlog::stdout_color_mt<spdlog::default_factory>("HeapMemory");
		m_benchmarkLogger = spdlog::stdout_color_mt<spdlog::default_factory>("Benchmark");


#ifdef PE_DEBUG
		m_coreLogger->set_level(spdlog::level::trace);
		m_clientLogger->set_level(spdlog::level::trace);
		m_memoryLogger->set_level(spdlog::level::trace);
		m_benchmarkLogger->set_level(spdlog::level::trace);
#else
		m_coreLogger->set_level(spdlog::level::info);
		m_clientLogger->set_level(spdlog::level::info);
		m_memoryLogger->set_level(spdlog::level::warn);
		m_benchmarkLogger->set_level(spdlog::level::off);
#endif
	}

	void Logger::Shutdown()
	{
		m_coreLogger.reset();
		m_clientLogger.reset();
		m_memoryLogger.reset();
		m_benchmarkLogger.reset();
		spdlog::shutdown();
	}
}
