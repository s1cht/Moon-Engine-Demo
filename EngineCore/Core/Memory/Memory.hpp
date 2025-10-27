#pragma once

#include <memory>

namespace ME::Core::Memory
{
	template<class T>
	using Reference = std::shared_ptr<T>;

	template<class T>
	using WeakReference = std::weak_ptr<T>;

	//template<class T>
	//using Scope = std::unique_ptr<T>;

	template<class T, class U = std::default_delete<T>>
	using Scope = std::unique_ptr<T, U>;

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