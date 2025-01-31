#include "pch.h"
#include "MemWatch.h"

static bool MemWatchInitialized = false;

namespace Pawn {

	MemWatch::MemWatch()
	{
		MemWatchInitialized = true;
	}

	void MemWatch::POnAllocated(SIZE_T size)
	{
		if (!allocBuf) return;
		totalAllocations++;
		if (m_MemWatchEnabled) PE_MEM_TRACE("MemWatch: New allocation! Size: {} bytes; Total allocations: {}", size, totalAllocations);
		else
		{
			if (allocBufElements == 250)
			{
				m_allocBufferOverloaded = true;
				return;
			}

			allocBuf[allocBufElements] = size;
			allocBufElements++;
		}
	}

	void MemWatch::POnFreed(SIZE_T size)
	{
		if (!deallocBuf) return;
		totalDeletions++;
		if (m_MemWatchEnabled) PE_MEM_TRACE("MemWatch: Freeing memory! Size: {} bytes; Total deletions: {}", size, totalDeletions);
		else
		{
			if (deallocBufElements == 250)
			{
				m_deallocBufferOverloaded = true;
				return;
			}
			deallocBuf[deallocBufElements] = size;
			deallocBufElements++;
		}
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

	void MemWatch::CleanBuffer()
	{
		delete[] allocBuf;
		delete[] deallocBuf;
	}

	void MemWatch::OutputBuffer()
	{
		std::string allocStr = "";
		for (SIZE_T i = 0; i < allocBufElements; i++)
		{
			allocStr += std::to_string(allocBuf[i]);
		}

		std::string deallocStr = "";
		for (SIZE_T i = 0; i < deallocBufElements; i++)
		{
			deallocStr += std::to_string(deallocBuf[i]);
		}

		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: Output enabled!");
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: Output enabled! MemWatch allocBuffer: {}", allocStr.c_str());
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: allocBuffer element count: {}; was overloaded: {}", allocBufElements, m_allocBufferOverloaded);
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: MemWatch deallocBuffer: {}", deallocStr.c_str());
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
		PE_MEM_TRACE("MemWatch: deallocBuffer element count: {}; was overloaded: {}", deallocBufElements, m_deallocBufferOverloaded);
		PE_MEM_TRACE("MemWatch: --------------------------------------------------------------");
	}

}

// Redefining allocation functions

void* operator new (SIZE_T size)
{
	Pawn::MemWatch::OnAllocated(size);
	return malloc(size);
}

void operator delete(void* ptr)
{
	Pawn::MemWatch::OnFreed(sizeof(ptr));
	free(ptr);
}

void operator delete[](void* ptr)
{
	Pawn::MemWatch::OnFreed(sizeof(ptr));
	free(ptr);
}