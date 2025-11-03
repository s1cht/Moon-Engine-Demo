#pragma once
// ----- Function + Bind() 


// ----- test Macros - do not know if I should implement them in Macros.hpp

#define PLACEHOLDER_LIST \
	X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9) X(10) 

#define X(N) struct _##N {};
PLACEHOLDER_LIST
#undef X

template<typename T> struct placeholder_index { static const int value = 0;  };
#define X(N) template<> struct placeholder_index<_##N> { static const int value = N; };
PLACEHOLDER_LIST
#undef X

#include "Core.hpp"

// ----- function

template<typename Signature>

template<typename R, typename... Args>
class Function<R(Args...)> {
private:
	struct CallableBase {
		virtual R invoke(Args... args) = 0;
		virtual CallableBase* clone() const = 0;
		virtual ~CallableBase() {}
	};

	template<typename F>
	struct CallableImpl : CallableBase {
		F func;
		CallableImpl(F f) : func(f) {}
		R invoke(Args... args) override { return func(args...); }
		CallableBase* clone() const override { return new CallableImpl(func); }
	};

	CallableBase* ptr;

public:
	Function() : ptr(nullptr) {}

	template<typename F>
	Function(F f) : ptr(new CallableImpl<F>(f)) {}

	Function(const Function& other)
		: ptr(other.ptr ? other.ptr->clone() : nullptr) {
	}

	Function& operator=(const Function& other) {
		if (this != &other) {
			delete ptr;
			ptr = other.ptr ? other.ptr->clone() : nullptr;
		}
		return *this;
	}

	~Function() { delete ptr; }

	R operator()(Args... args) const {
		if (!ptr) {
			// Null case - do not know what you wish to place here
			return R();
		}
		return ptr->invoke(args...);
	}

	operator bool() const { return ptr != nullptr; }
};

// ----- Bind() with placeholders

template<typename Head, typename... Tail>
struct BoundArgs {
	Head head;
	BoundArgs<Tail...> tail;

	BoundArgs(Head h, Tail... t) : head(h), tail(t...) {}
};

template<typename Head>
struct BoundArgs<Head> {
	Head head;
	BoundArgs(Head h) : head(h) {}
};

template<int N, typename Arg1, typename... Args>
auto& get_runtime_arg(Arg1& arg1, Args&... args) {
	if constexpr (N == 1) return arg1;
	else return get_runtime_arg<N - 1>(args...);
}

template<typename T, typename...RuntimeArgs>
auto get_value(T& val, RuntimeArgs&... args) {
	if constexpr (placeholder_index<T>::value > 0) {
		return get_runtime_arg<placeholder_index<T>::value>(args...);
	}
	else {
		return val;
	}
}

template<typename F, typename Arg1, typename... RuntimeArgs>
auto call_helper(F& f, BoundArgs<Arg1>& bound, RuntimeArgs&... args) {
	return f(get_value(bound.head, args...));
}

template<typename F, typename Head, typename... Tail, typename... RuntimeArgs>
auto call_helper(F& f, BoundArgs<Head, Tail...>& bound, RuntimeArgs&... args) {
	auto g = [&](auto... vals) {return f(vals..., get_value(bound.head, args...)); };
	return call_helper(g, bound.tail, args...);
}

template<typename F, typename... Bound>
struct Binder {
	F func;
	BoundArgs<Bound...> bound;

	Binder(F f, Bound... b) : func(f), bound(b...) {}

	template<typename... RuntimeArgs>
	auto operator()(RuntimeArgs... args) {
		return call_helper(func, bound, args...);
	}
};

template<typename F, typename... Bound>
auto Bind(F f, Bound... b) {
	return Binder<F, Bound...>(f, b...);
}