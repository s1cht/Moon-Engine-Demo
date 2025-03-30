module;

#include "Core.h"

export module Pawn.Core.Memory.Allocator;

export namespace Pawn::Core::Memory
{
	template <typename T>
	class Allocator
	{
	public:
		using DataType = T;

	public:
		Allocator() {};
		~Allocator() {};

	public:

		template <typename _Other>
		struct Rebind
		{
			using Other = Allocator<_Other>;
		};

		DataType* Allocate(SIZE_T size)
		{
			return static_cast<DataType*>(::operator new(size));
		}

		void Deallocate(void* ptr)
		{
			::operator delete(ptr);
		}

		void Deallocate(void* ptr, SIZE_T size)
		{
			::operator delete(ptr, size);
		}

		template <class Val, class... varg>
		void Construct(Val* ptr, varg&&... args)
		{
			new(static_cast<void*>(ptr)) Val(std::forward<varg>(args)...);
		}

		void Destroy(DataType* ptr)
		{
			ptr->~DataType();
		}
	};
}
