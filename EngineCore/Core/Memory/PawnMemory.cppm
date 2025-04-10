module;

export module Pawn.Core.Memory;

import <memory>;

namespace Pawn::Core::Memory
{
	export template<class T>
	using Reference = std::shared_ptr<T>;

	export template<class T>
	using WeakReference = std::weak_ptr<T>;

	//export template<class T>
	//using Scope = std::unique_ptr<T>;

	export template<class T, class U = std::default_delete<T>>
	using Scope = std::unique_ptr<T, U>;

	export template<class T, class... val>
	Reference<T> MakeReference(val... args)
	{
		return std::make_shared<T>(std::forward<val>(args)...);
	}

	export template<class T, class... val>
	Scope<T> MakeScope(val&&... args)
	{
		return std::make_unique<T>(std::forward<val>(args)...);
	}
};