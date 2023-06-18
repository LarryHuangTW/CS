> *It's not reinventing the wheel. Just for practice and for fun.*

### 1. basic version:

```C++
/*
 *	function parameters:
 *				key:   the value to search for
 *				ary:   the data array with elements to examine
 *				count: number of elements in the array
 *
 *	return value: pointer to the found element in the data array, null pointer otherwise.
 */
constexpr int* binary_search(int key, const int ary[], size_t count)
{
	size_t middle { 0 };

	for (size_t left { 0 }; 0 < count; count >>= 1)
	{
		middle = left + (count >> 1);

		if (ary[middle] == key)
			break;

		if (ary[middle] < key)
		{
			left = middle + 1;
			--count;
		}
	}

	return count == 0 ? nullptr : const_cast<int*>(std::addressof(ary[middle]));
}
```

It works but what if we want to find values of other data types?<br><br>

### 2. C generic version:

```C++
/*
 *	function parameters:
 *				key:   pointer to the value to search for
 *				ptr:   pointer to the data array with elements to examine
 *				count: number of elements in the array
 *				sz:    size (in bytes) of each element in the data array
 *				comp:  comparison function to check the equivalence of two values
 *
 *	return value: pointer to the found element in the data array, null pointer otherwise.
 */
constexpr void* binary_search(const void* key, const void* ptr, size_t count, size_t sz, compare_function_type comp)
{
	const unsigned char* middle { nullptr };

	if (key == nullptr || ptr == nullptr || sz == 0)
		return nullptr;

	for (auto left { static_cast<const unsigned char*>(ptr) }; 0 < count; count >>= 1)
	{
		middle = left + (count >> 1) * sz;

		if (comp(middle, key) == 0)
			break;

		if (comp(middle, key) < 0)
		{
			left = middle + sz;
			--count;
		}
	}

	return count == 0 ? nullptr : const_cast<unsigned char*>(middle);
}
```

It works for different data types. 

Raw pointers are a little scary.<br><br>

### 3. C++ function template version:

```C++
/*
 *	function parameters:
 *				ary:  the data array with elements to examine
 *				key:  the value to search for
 *				comp: comparison function or binary predicate which returns 
 *				      true if the first argument is less than the second, false otherwise 
 *				      the signature of the comparison function should be: 
 *				      bool func(const Type1& lhs, const Type2& rhs)
 *
 *	return value: pointer to the found element in the data array, or pointer to the end of the array otherwise.
 */
template<class T, size_t N, class Compare = std::less<>>
constexpr T* binary_search(const T (&ary)[N], const T& key, Compare comp = Compare{})
{
	size_t middle { 0 }, count { N };

	for (size_t left { 0 }; 0 < count; count >>= 1)
	{
		middle = left + (count >> 1);

		if (comp(ary[middle], key))
		{
			left = middle + 1;
			--count;
		}
		else
		{
			if ( !comp(key, ary[middle]) )
				break;
		}
	}

	return const_cast<T*>(std::addressof(ary[count == 0 ? N : middle]));
}
```

It also works for different data types. 

What if we want to find any value in a C++ data container?<br><br>

### 4. C++ STL version:

```C++
template<class FwdIter, class T, class Compare = std::less<>>
constexpr FwdIter lower_bound(FwdIter first, FwdIter last, const T& value, Compare comp = Compare{})
{
	for (auto count { std::distance(first, last) }; 0 < count; count >>= 1)
	{
		auto middle { std::next(first, count >> 1) };

		if (comp(*middle, value))
		{
			first = std::next(middle);

			--count;
		}
	}

	return first;
}

template<class FwdIter, class T, class Compare = std::less<>>
constexpr bool binary_search(FwdIter first, FwdIter last, const T& value, Compare comp = Compare{})
{
	first = cust::lower_bound(first, last, value, comp);

	return !(first == last) && !comp(value, *first);
}
```

It works for different data types and C++ data containers. 

(To some degree this version could also be used with raw pointers)

What would happen if someone use it on non-contiguous data containers or (ordered or unordered) associative data containers?

For example: std::forward_list, std::list, std::set, std::map, std::unordered_set, std::unordered_map ...<br><br>

**[ possible workaround ]**

```C++
//variable template to check if a type has find() member function
template<class, class = void>
constexpr bool has_find_function = false;

template<class T>
constexpr bool has_find_function<T, std::void_t<decltype(std::declval<T>().find(T::key_type()))>> = true;

/*
 *	function parameters: the same as above C++ STL version
 *
 *	return value: iterator pointing to the found element, last otherwise.
 */
template<class FwdIter, class T, class Compare = std::less<>>
constexpr FwdIter binary_search2(FwdIter first, FwdIter last, const T& value, Compare comp = Compare{})
{
	first = cust::lower_bound(first, last, value, comp);

	return !(first == last) && !comp(value, *first) ? first : last;
}

/*
 *	function parameters:
 *				cont:  the data array or data container with elements to examine
 *				value: value to search for
 *				comp:  comparison function or binary predicate which returns 
 *				       true if the first argument is less than the second, false otherwise 
 *				       the signature of the comparison function should be: 
 *				       bool func(const Type1& lhs, const Type2& rhs)
 *
 *	return value: iterator pointing to the found element, or iterator one past the end of data container otherwise.
 */
template<class Container, class T, class Compare = std::less<>>
constexpr auto binary_search(const Container& cont, const T& value, Compare comp = Compare{})
{
	if constexpr (std::is_array_v<Container>) {
		return cust::binary_search2(std::begin(cont), std::end(cont), value, comp);
	}
	else if constexpr (std::is_same_v<Container, forward_list<typename Container::value_type>> || std::is_same_v<Container, list<typename Container::value_type>>) {
		return std::find(cont.begin(), cont.end(), value);
	}
	else if constexpr (has_find_function<Container>) {
		return cont.find(value);
	}
	else {
		return cust::binary_search2(std::begin(cont), std::end(cont), value, comp);
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
		auto iter { cust::binary_search(cont, num) };

		if (iter != std::end(cont))
		{
			std::cout << "NOT correct! Found " << num << " in the container.\n\n";

			return;
		}
	}

	for (auto num : initList)
	{
		auto iter { cust::binary_search(cont, num) };

		if (iter == std::end(cont))
		{
			std::cout << "NOT correct! Not found " << num << " in the container.\n\n";

			return;
		}
	}

	std::cout << "OK!\n\n";
}

int main(int argc, char* argv[])
{
	const auto initList = { 1 , 3 , 8 , 11 , 12 , 13 , 17 , 20 , 25 , 28 , 33 , 50 , 67 , 89 , 90 , 100 };
	const auto not_in   = { 22 , 30 , 49 , 51 , -5 , -50 , 70 , 80 , 60 };

	const int i_ary[] { 1 , 3 , 8 , 11 , 12 , 13 , 17 , 20 , 25 , 28 , 33 , 50 , 67 , 89 , 90 , 100 };

	const std::array<int, 16> ary { 1 , 3 , 8 , 11 , 12 , 13 , 17 , 20 , 25 , 28 , 33 , 50 , 67 , 89 , 90 , 100 };

	const std::vector<int>             vec  { initList };
	const std::deque<int>              deq  { initList };
	const std::forward_list<int>       flst { initList };
	const std::list<int>               lst  { initList };
	const std::set<int>                set  { initList };
	const std::map<int, int>           map  { {1 , 0} , {3 , 1} , {8 , 2} , {11 , 3} , {12 , 4} , {13 , 5} , {17 , 6} , {20 , 7} , {25 , 8} , {28 , 9} , {33 , 10} , {50 , 11} , {67 , 12} , {89 , 13} , {90 , 14} , {100 , 15} };
	const std::unordered_set<int>      uset { initList };
	const std::unordered_map<int, int> umap { {1 , 0} , {3 , 1} , {8 , 2} , {11 , 3} , {12 , 4} , {13 , 5} , {17 , 6} , {20 , 7} , {25 , 8} , {28 , 9} , {33 , 10} , {50 , 11} , {67 , 12} , {89 , 13} , {90 , 14} , {100 , 15} };
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

	/*	of course, compile-time error
	test_func(stk,  not_in, initList);
	test_func(que,  not_in, initList);
	*/

	return 0;
}
```
