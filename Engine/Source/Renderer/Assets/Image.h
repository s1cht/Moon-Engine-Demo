#pragma once

#include <Core.hpp>

#include "Core/Containers/Array.hpp"
#include "Core/Containers/String/BasicString.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace ME::Assets
{
	class MEAPI Image
	{
	public:
		Image(ME::Core::Containers::Array<uint8>&& image, ME::Core::Math::Resolution2D<uint32> resolution, const ME::Core::String& name);
		Image(const uint8* data, SIZE_T size, ME::Core::Math::Resolution2D<uint32> resolution, const ME::Core::String& name);
		~Image();

	public:
		ME::Core::Containers::Array<uint8>& GetImage() { return m_Data; }
		ME::Core::Math::Resolution2D<uint32> GetResolution() const { return m_Resolution; }
		const ME::Core::String& GetName() const { return m_Name; }

	private:
		ME::Core::Containers::Array<uint8> m_Data;
		ME::Core::Math::Resolution2D<uint32> m_Resolution;
		ME::Core::String m_Name;

	};
}
