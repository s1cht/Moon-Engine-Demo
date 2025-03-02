#pragma once

#include "Core/CoreTypes.h"

template <typename T>
class Allocator
{
public:
	using DataType = T;

public:
	Allocator() {};
	~Allocator() {};

public:
	
	DataType* Allocate(SIZE_T size)
	{
		return static_cast<DataType*>(::operator new(size));
	}

	void Deallocate(DataType* ptr, SIZE_T size)
	{
		::operator delete(ptr, size);
	}

	template <class Val, class... varg>
	void Construct(Val* ptr, varg&&... args)
	{
		new(static_cast<void*>(ptr)) Val(std::forward<varg>(args)...);
	}
};	