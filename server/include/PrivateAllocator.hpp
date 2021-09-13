#pragma once

// https://stackoverflow.com/a/17008204/4945014
template <typename T>
struct PrivateAllocator : std::allocator<T> {
	template <class U, class... Args>
	void construct(U* p, Args&&... args) {
		::new ((void*)p) U(std::forward<Args>(args)...);
	}
	template <class U>
	struct rebind {
		// don't actually rebind, so only we can use it
		typedef PrivateAllocator other;
	};
};
