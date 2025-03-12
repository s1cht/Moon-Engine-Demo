#include "MemWatch.h"

static bool MemWatchInitialized = false;

namespace Pawn {

	MemWatch::MemWatch()
	{
		MemWatchInitialized = true;
	}

	void MemWatch::POnAllocated(SIZE_T size)
	{
		m_MemoryUsing += size;
		m_MemoryUsed += size;
		m_TotalAllocations++;
		if (m_MemWatchEnabled) PE_MEM_TRACE("--------------------------------------------------------------");
		if (m_MemWatchEnabled) PE_MEM_TRACE("New allocation! Size: {} bytes; Total allocations: {}", size, m_TotalAllocations);
		if (m_MemWatchEnabled) PE_MEM_TRACE("Memory usage: {} bytes; {} kilobytes; {} megabytes", m_MemoryUsing, m_MemoryUsing / 1024, m_MemoryUsing / 1024 / 1024);
		if (m_MemWatchEnabled) PE_MEM_TRACE("--------------------------------------------------------------");
	}

	void MemWatch::POnFreed(SIZE_T size)
	{
		m_MemoryUsing -= size;
		m_MemoryFreed += size;
		m_TotalDeletions++;

		if (m_MemWatchEnabled) PE_MEM_TRACE("--------------------------------------------------------------");
		if (m_MemWatchEnabled) PE_MEM_TRACE("Freeing memory! Size: {} bytes; Total deletions: {}", size, m_TotalDeletions);
		if (m_MemWatchEnabled) PE_MEM_TRACE("Memory usage: {} bytes; {} kilobytes; {} megabytes", m_MemoryUsing, m_MemoryUsing / 1024, m_MemoryUsing / 1024 / 1024);
		if (m_MemWatchEnabled) PE_MEM_TRACE("--------------------------------------------------------------");
	}

	void MemWatch::POnEnabled()
	{
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("----------------------Output enabled!-------------------------");
		PE_MEM_TRACE("----------------------/-------------\\-------------------------");
		PE_MEM_TRACE("Allocation count before enabling: {}; Memory used: {} bytes", m_TotalAllocations, m_MemoryUsed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Deallocation count before enabling: {}; Memory freed: {} bytes", m_TotalDeletions, m_MemoryFreed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Memory usage: {} bytes; {} kilobytes; {} megabytes", m_MemoryUsing, m_MemoryUsing / 1024, m_MemoryUsing / 1024 / 1024);
		PE_MEM_TRACE("--------------------------------------------------------------");
	}

	bool MemWatch::IsMemWatchEnabled()
	{ 
		if (!MemWatchInitialized) return false;
		return Get().PIsMemWatchEnabled(); 
	}

	void MemWatch::EnableMemWatch()
	{
		if (!MemWatchInitialized) return;
		Get().PEnableMemWatch(); 
	}

	void MemWatch::OnAllocated(SIZE_T size) 
	{ 
		if (!MemWatchInitialized) return;
		Get().POnAllocated(size);
	}

	void MemWatch::OnFreed(SIZE_T size) 
	{ 
		if (!MemWatchInitialized) return;
		Get().POnFreed(size); 
	}

	inline void MemWatch::OnUpdate()
	{
		if (!MemWatchInitialized) return;
		Get().POnUpdate();
	}

	void MemWatch::OnExit()
	{
		if (!MemWatchInitialized) return;
		Get().POnExit();
	}

	void MemWatch::POnUpdate()
	{
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Allocation count: {}; Memory used: {} bytes", m_TotalAllocations, m_MemoryUsed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Deallocation count: {}; Memory freed: {} bytes", m_TotalDeletions, m_MemoryFreed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Memory usage: {} bytes; {} kilobytes; {} megabytes", m_MemoryUsing, m_MemoryUsing / 1024, m_MemoryUsing / 1024 / 1024);
		PE_MEM_TRACE("--------------------------------------------------------------");
	}

	void MemWatch::POnExit()
	{
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Allocation count: {}; Memory used: {} bytes", m_TotalAllocations, m_MemoryUsed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Deallocation count: {}; Memory freed: {} bytes", m_TotalDeletions, m_MemoryFreed);
		PE_MEM_TRACE("--------------------------------------------------------------");
		PE_MEM_TRACE("Memory usage: {} bytes; {} kilobytes; {} megabytes", m_MemoryUsing, m_MemoryUsing / 1024, m_MemoryUsing / 1024 / 1024);
		PE_MEM_TRACE("--------------------------------------------------------------");
		MemWatchInitialized = false;
	}

}

// Redefining allocation functions

void* operator new (SIZE_T size)
{
	//Pawn::MemWatch::OnAllocated(size);
	return malloc(size);
}

void operator delete(void* ptr)
{
	//Pawn::MemWatch::OnFreed(sizeof(ptr));
	free(ptr);
}

void operator delete[](void* ptr)
{
	//Pawn::MemWatch::OnFreed(sizeof(ptr));
	free(ptr);
}
