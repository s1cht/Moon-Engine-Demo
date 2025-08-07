#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>

#include "Renderer/Base/RenderObject.hpp"

namespace ME::Render
{
	class MOON_API RenderResourcesTracker
	{
	public:
		RenderResourcesTracker();
		~RenderResourcesTracker();

	public:
		inline static RenderResourcesTracker& Get()
		{
			static RenderResourcesTracker instance;
			return instance;
		}

	public:
		void ShutdownAll();
		void AddItem(const ME::Core::Memory::Reference<RenderObject>& object);

	private:
		ME::Core::Containers::Array<ME::Core::Memory::Reference<RenderObject>> m_Objects;

	};
}
