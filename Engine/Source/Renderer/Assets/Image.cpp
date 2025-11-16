#include "Image.hpp"

namespace ME::Assets
{
	Image::Image(ME::Core::Array<uint8>&& image, ME::Core::Math::Resolution2D<uint32> resolution, const ME::Core::String& name)
	{
		m_Data = std::move(image);
		m_Resolution = resolution;
		m_Name = name;
	}

	Image::Image(const uint8* data, SIZE_T size, ME::Core::Math::Resolution2D<uint32> resolution, const ME::Core::String& name)
	{
		m_Data = std::initializer_list<uint8>(data, data + size);
		m_Resolution = resolution;
		m_Name = name;
	}
}
