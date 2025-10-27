#include "RenderResourcesTracker.hpp"
#include "RenderCommand.h"

namespace ME::Render
{
	RenderResourcesTracker::RenderResourcesTracker()
	{
		m_Objects = {};
	}

	RenderResourcesTracker::~RenderResourcesTracker()
	{
		if (m_Objects.GetSize() > 0)
			ShutdownAll();
	}

	void RenderResourcesTracker::ShutdownAll()
	{
		for (SIZE_T i = m_Objects.GetSize(); i > 0; i--)
		{
			m_Objects[i - 1]->Shutdown();
		}
	}

	void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<RenderObject>& object)
	{
		m_Objects.EmplaceBack(object);
	}
}
