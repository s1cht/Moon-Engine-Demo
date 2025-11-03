#pragma once

#include "Core.hpp"

template<typename Signature>

template<typename R, typename... Args>
class Function<R(Args...)> {
private:
	struct CallableBase {
		virtual R invoke(Args... args) = 0;
		virtual CallableBase* clone() const = 0;
		virtual ~CallableBase(){}
	};

	template<typename F>
	struct CallableBase : CallableBase {
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
		: ptr(other.ptr ? other.ptr->clone() : nullptr) {}

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
			// Null case
			return R();
		}
		return ptr->invoke(args...);
	}

	operator bool() const { return ptr != nullptr; }
};