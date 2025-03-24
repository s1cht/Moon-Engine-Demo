#pragma once

#include "Allocator.h"
#include "Iterator.h"
#include <memory>

namespace Pawn::Memory
{
	template<class T>
	using Reference = std::shared_ptr<T>;

	template<class T>
	using WeakReference = std::weak_ptr<T>;

	template<class T>
	using Scope = std::unique_ptr<T>;

	template<class T, class... val>
	Reference<T> MakeReference(val... args)
	{
		return std::make_shared<T>(std::forward<val>(args)...);
	}

	template<class T, class... val>
	Scope<T> MakeScope(val&&... args)
	{
		return std::make_unique<T>(std::forward<val>(args)...);
	}
};