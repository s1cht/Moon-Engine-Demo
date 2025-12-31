#pragma once
#include "Core.hpp"
#include "Core/Memory/Memory.hpp"

namespace ME::Core
{
	template <class BaseClass>
    class ClassInterface
    {
	public:
		template<typename T>
		ME_NODISCARD inline constexpr T* As() noexcept
		{
			static_assert(!std::is_same_v<T, BaseClass> && std::is_base_of_v<BaseClass, T>,
				"T must be derived from RenderObject, but not the BaseClass itself!");
			return static_cast<T*>(this);
		}
		template<typename T>
		ME_NODISCARD inline ME::Core::Memory::Reference<T> AsReference() noexcept
		{
			static_assert(!std::is_same_v<T, BaseClass> && std::is_base_of_v<BaseClass, T>,
				"T must be derived from BaseClass, but not the BaseClass itself!");
			return ME::Core::Memory::Reference<T>(this);
		}
	};
}
