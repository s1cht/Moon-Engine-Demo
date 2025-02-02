#pragma once
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"

/*
	// Memory tracker singleton \\

	
*/

namespace Pawn {

	class PAWN_API MemWatch
	{
	public:
		MemWatch(const MemWatch&) = delete;

		inline static MemWatch& Get() { 
			static MemWatch instance;
			return instance; 
		};

	public:
		inline static bool IsMemWatchEnabled();
		static void EnableMemWatch();

		static void OnAllocated(SIZE_T size);
		static void OnFreed(SIZE_T size);
		static void OnUpdate();
		static void OnExit();

		inline static SIZE_T GetCurrentMemoryUsage() { return Get().PGetCurrentMemoryUsage(); }

	private:
		inline bool PIsMemWatchEnabled() { return m_MemWatchEnabled; };
		void PEnableMemWatch() { m_MemWatchEnabled = true; POnEnabled(); };
		inline SIZE_T PGetCurrentMemoryUsage() { return m_MemoryUsing; }

	private:
		void POnAllocated(SIZE_T size);
		void POnFreed(SIZE_T size);
		void POnEnabled();
		void POnUpdate();
		void POnExit();

	private:
		bool m_MemWatchEnabled = false;

		SIZE_T m_MemoryUsed = 0;
		SIZE_T m_MemoryFreed = 0;

		SIZE_T m_MemoryUsing = 0;

		uint64 m_TotalAllocations = 0;
		uint64 m_TotalDeletions = 0;

	private:
		MemWatch();

	};

}