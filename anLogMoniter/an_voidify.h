#pragma once
#include <iostream>
#include <functional>
#include <memory>

namespace an {

	template<typename... Args, typename Lambda>
	std::pair< void(*)(void*, Args...), std::unique_ptr<void, void(*)(void*)> > voidify(Lambda&& l) {
		typedef typename std::decay<Lambda>::type Func;
		
		std::unique_ptr<void, void(*)(void*)> data(
			new Func(std::forward<Lambda>(l)),
			[](void* ptr) { delete (Func*)ptr; }
		);

		return{
			[](void* v, Args... args)->void {
			Func* f = static_cast<Func*>(v);
			(*f)(std::forward<Args>(args)...);
		},
			std::move(data)
		};
	}

	void register_callback(void(*function)(void*), void * p) {
		function(p); // to test
	}

	void test() {
		int x = 0;
		auto closure = [&]()->void { ++x; };
		auto voidified = voidify(closure);
		register_callback(voidified.first, voidified.second.get());
		register_callback(voidified.first, voidified.second.get());
		std::cout << x << "\n";
	}
}
