#pragma once
#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>

#include "Base/RenderCore.hpp"

namespace ME::Render
{
	class MEAPI RenderResourceTracker
	{
	public:
		RenderResourceTracker();
		~RenderResourceTracker();

	public:
		void Add(const ME::Core::Memory::Reference<RenderObject>& object);
		void ShutdownAll();

	private:
		// TODO: SavePipelines()

	private:
		ME::Core::Array<ME::Core::Memory::Reference<RenderObject>> m_Phase1Objects;
		ME::Core::Array<ME::Core::Memory::Reference<RenderObject>> m_Phase2Objects;
		ME::Core::Array<ME::Core::Memory::Reference<RenderObject>> m_Phase3Objects;
		ME::Core::Atomic_uint64 m_TotalObjectCount;
	};
}
