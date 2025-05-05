module;

export module Pawn.Core.Utils.Logger;

#include "Core.h"

import Pawn.Core;
import Pawn.Core.Memory;
import Pawn.Thirdparty.spdlog;

export namespace Pawn::Core::Utils 
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