#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>

#include <type_traits>

namespace ME::Render
{
	class MOON_API RenderObject
	{
	public:
		template<typename T>
		ME_NODISCARD inline constexpr T* As() noexcept
		{
			static_assert(!std::is_same_v<T, RenderObject> && std::is_base_of_v<RenderObject, T>, 
				"T must be derived from RenderObject, but not RenderObject itself!");
			return static_cast<T*>(this);
		}
		template<typename T>
		ME_NODISCARD inline ME::Core::Memory::Reference<T> AsReference() noexcept
		{
			static_assert(!std::is_same_v<T, RenderObject> && std::is_base_of_v<RenderObject, T>, 
				"T must be derived from RenderObject, but not RenderObject itself!");
			return ME::Core::Memory::Reference<T>(this);
		}

		virtual void Shutdown() = 0;

	};
}
