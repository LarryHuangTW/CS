> *It's not reinventing the wheel. Just for practice and for fun.*

### 1. basic version:

```C++
constexpr int* find(int key, const int ary[], size_t count)
{
	size_t idx { 0 };

	for ( ; idx != count && ary[idx] != key; ++idx);

	return idx == count ? nullptr : const_cast<int*>(std::addressof(ary[idx]));
}
```

It works but what if we want to find values of other data types?<br><br>

### 2. C generic version:

```C++
constexpr void* find(const void* key, const void* ptr, size_t count, size_t sz)
{
	size_t idx { 0 };

	if (key == nullptr || ptr == nullptr || sz == 0)
		return nullptr;

	for ( ; idx != count && std::memcmp(static_cast<const unsigned char*>(ptr) + idx * sz, key, sz) != 0; ++idx);

	return idx == count ? nullptr : static_cast<unsigned char*>(const_cast<void*>(ptr)) + idx * sz;
}

//to find C strings / null-terminated byte strings
constexpr char** find(const char* key, const char* ary[], size_t count)
{
	size_t idx { 0 };

	if (key == nullptr || ary == nullptr)
		return nullptr;

	for ( ; idx != count && std::strcmp(ary[idx], key) != 0; ++idx);

	return idx == count ? nullptr : const_cast<char**>(std::addressof(ary[idx]));
}
```

It works for different data types. 

Raw pointers are a little scary.<br><br>

### 3. C++ function template version:

```C++
template<class T>
constexpr T* find(const T& key, const T ary[], size_t count)
{
	size_t idx { 0 };

	for ( ; idx != count && !(ary[idx] == key); ++idx);

	return idx == count ? nullptr : const_cast<T*>(std::addressof(ary[idx]));
}
```

It also works for different data types. 

What if we want to find any value in a C++ data container?<br><br>

### 4. C++ STL version:

```C++
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
```

It works for different data types and C++ data containers. 

(To some degree this version could also be used with raw pointers)

What would happen if someone use it on (ordered or unordered) associative data containers?

For example: std::set, std::map, std::unordered_set, std::unordered_map ...<br><br>

**[ possible workaround ]**

```C++
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

template<class Container, class T>
constexpr auto find(const Container& cont, const T& value)
{
	if constexpr (std::is_array_v<Container>) {
		return cust::find(std::begin(cont), std::end(cont), value);
	}
	else if constexpr (has_find_function<Container>) {
		return cont.find(value);
	}
	else if constexpr (is_iterable<Container>) {
		return cust::find(std::begin(cont), std::end(cont), value);
	}
	else {
		static_assert(false, "provided data container is not supported by this find() function");

		return cont.end();			//there would be a compile-time error issued before here
	}
}
```

## Example:

```C++
#include <iostream>
#include <initializer_list>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include "algorithm.h"

template<class Container, class T>
void test_func(const Container& cont, const T& not_in, const T& initList)
{
	for (auto num : not_in)
	{
		auto iter { cust::find(cont, num) };

		if (iter != std::end(cont))
		{
			std::cout << "NOT correct ! Found " << num << " in the container.\n\n";

			return;
		}
	}

	for (auto num : initList)
	{
		auto iter { cust::find(cont, num) };

		if (iter == std::end(cont))
		{
			std::cout << "NOT correct and not found !\n\n";

			return;
		}
	}

	std::cout << "OK!\n\n";
}

int main(int argc, char* argv[])
{
	const auto initList = { 22 , 17 , 5 , 9 , 0 , 10 , 32 , 8 , 11 , 16 , 15 , 6 , 7 , 3 , 12 , 1 , 2 , -100 };
	const auto not_in   = { 25 , 30 , 50 , -5 , -50 , 70 , 28 , 60 };

	const int i_ary[] { 22 , 17 , 5 , 9 , 0 , 10 , 32 , 8 , 11 , 16 , 15 , 6 , 7 , 3 , 12 , 1 , 2 , -100 };

	const std::array<int, 18> ary { 22 , 17 , 5 , 9 , 0 , 10 , 32 , 8 , 11 , 16 , 15 , 6 , 7 , 3 , 12 , 1 , 2 , -100 };

	const std::vector<int>             vec  { initList };
	const std::deque<int>              deq  { initList };
	const std::forward_list<int>       flst { initList };
	const std::list<int>               lst  { initList };
	const std::set<int>                set  { initList };
	const std::map<int, int>           map  { {22 , 0} , {17 , 1} , {5 , 2} , {9 , 3} , {0 , 4} , {10 , 5} , {32 , 6} , {8 , 7} , {11 , 8} , {16 , 9} , {15 , 10} , {6 , 11} , {7 , 12} , {3 , 13} , {12 , 14} , {1 , 15} , {2 , 16} , {-100 , 17} };
	const std::unordered_set<int>      uset { initList };
	const std::unordered_map<int, int> umap { {22 , 0} , {17 , 1} , {5 , 2} , {9 , 3} , {0 , 4} , {10 , 5} , {32 , 6} , {8 , 7} , {11 , 8} , {16 , 9} , {15 , 10} , {6 , 11} , {7 , 12} , {3 , 13} , {12 , 14} , {1 , 15} , {2 , 16} , {-100 , 17} };
	const std::stack<int>              stk  { deq };
	const std::queue<int>              que  { deq };

	test_func(i_ary, not_in, initList);
	test_func(ary,   not_in, initList);
	test_func(vec,   not_in, initList);
	test_func(deq,   not_in, initList);
	test_func(flst,  not_in, initList);
	test_func(lst,   not_in, initList);
	test_func(set,   not_in, initList);
	test_func(map,   not_in, initList);
	test_func(uset,  not_in, initList);
	test_func(umap,  not_in, initList);

	/*   of course, compile-time error
	test_func(stk,  not_in, initList);
	test_func(que,  not_in, initList);
	*/

	return 0;
}
```
