#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

namespace cust					//customized / non-standard
{
	using std::size_t;

	//basic version
	constexpr int* find(int key, const int ary[], size_t count)
	{
		size_t idx { 0 };

		for ( ; idx != count && ary[idx] != key; ++idx);

		return idx == count ? nullptr : const_cast<int*>(std::addressof(ary[idx]));
	}

	//C generic version
	constexpr void* find(const void* key, const void* ptr, size_t count, size_t sz)
	{
		size_t idx { 0 };

		if (key == nullptr || ptr == nullptr || sz == 0)
			return nullptr;

		for ( ; idx != count && std::memcmp(static_cast<const unsigned char*>(ptr) + idx * sz, key, sz) != 0; ++idx);

		return idx == count ? nullptr : static_cast<unsigned char*>(const_cast<void*>(ptr)) + idx * sz;
	}

	//C generic version (to find C strings / null-terminated byte strings)
	constexpr char** find(const char* key, const char* ary[], size_t count)
	{
		size_t idx { 0 };

		if (key == nullptr || ary == nullptr)
			return nullptr;

		for ( ; idx != count && std::strcmp(ary[idx], key) != 0; ++idx);

		return idx == count ? nullptr : const_cast<char**>(std::addressof(ary[idx]));
	}

	//C++ function template version
	template<class T>
	constexpr T* find(const T& key, const T ary[], size_t count)
	{
		size_t idx { 0 };

		for ( ; idx != count && !(ary[idx] == key); ++idx);

		return idx == count ? nullptr : const_cast<T*>(std::addressof(ary[idx]));
	}

	//C++ STL version
	template<class InputIter, class T>
	constexpr InputIter find(InputIter first, InputIter last, const T& value)
	{
		for ( ; first != last && !(*first == value); ++first);

		return first;
	}

	template<class InputIter, class UnaryPredicate>
	constexpr InputIter find_if(InputIter first, InputIter last, UnaryPredicate p)
	{
		for ( ; first != last && !p(*first); ++first);

		return first;
	}

	//variable template to check if a type has find() member function
	template<class, class = void>
	constexpr bool has_find_function = false;

	template<class T>
	constexpr bool has_find_function<T, std::void_t<decltype(std::declval<T>().find(T::key_type()))>> = true;

	//variable template to check if a type has begin() and end() member functions
	template<class, class = void>
	constexpr bool is_iterable = false;

	template<class T>
	constexpr bool is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> = true;

	//customized find() function template
	template<class Container, class T>
	constexpr auto find(const Container& cont, const T& value)
	{
		if constexpr (has_find_function<Container>)
			return cont.find(value);
		else if constexpr (is_iterable<Container>)
			return cust::find(cont.begin(), cont.end(), value);
		else
			static_assert(false, "provided data container is not supported by this find() function");

		return cont.end();			//there would be a compile-time error issued before here
	}
}
