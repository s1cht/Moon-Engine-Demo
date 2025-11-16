#pragma once
#include "Core/Types.hpp"

namespace ME::Core::Memory
{
	template <typename T>
	class Allocator
	{
	public:
		using value_type = T;

	public:
		Allocator() = default;

		template <typename U>
		Allocator(const Allocator<U>&) noexcept {}

		value_type* Allocate(SIZE_T n)
		{
			constexpr size_t alignment = alignof(T);
			void* ptr = ::operator new(n * sizeof(T), std::align_val_t{ alignment });
			return static_cast<T*>(ptr);
		}

		void Deallocate(T* ptr, SIZE_T n)
		{
			::operator delete(ptr, n * sizeof(T), std::align_val_t{ alignof(T) });
		}

		template <typename U>
		struct Rebind
		{
			using Other = Allocator<U>;
		};

		template <class Val, class... varg>
		void Construct(Val* ptr, varg&&... args)
		{
			new (static_cast<void*>(ptr)) Val(std::forward<varg>(args)...);
		}

		template <class Val>
		void Destroy(Val* ptr)
		{
			ptr->~Val();
		}

		bool operator==(const Allocator&) const noexcept { return true; }
		bool operator!=(const Allocator&) const noexcept { return false; }
	};
}