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

	private:
		inline bool PIsMemWatchEnabled() { return m_MemWatchEnabled; };
		void PEnableMemWatch() { m_MemWatchEnabled = true; OutputBuffer(); CleanBuffer(); };

	private:
		void CleanBuffer();

		void OutputBuffer();

	private:
		void POnAllocated(SIZE_T size);
		void POnFreed(SIZE_T size);

	private:
		bool m_MemWatchEnabled = false;
		bool m_deallocBufferOverloaded = false;
		bool m_allocBufferOverloaded = false;

		SIZE_T* allocBuf = new SIZE_T[250];
		uint8 allocBufElements = 0;

		SIZE_T* deallocBuf = new SIZE_T[250];
		uint8 deallocBufElements = 0;

		uint64 totalAllocations = 0;
		uint64 totalDeletions = 0;

	private:
		MemWatch();

	};

}