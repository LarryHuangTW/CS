#pragma once

/*
 *	Binary Search: to check if an element within the ordered range matches the specified value or search condition.
 */

#include <cstddef>
#include <memory>
#include <functional>
#include <iterator>
#include <forward_list>
#include <list>
#include <type_traits>

namespace cust					//customized / non-standard
{
	using std::size_t;
	using std::forward_list;
	using std::list;

	/*
	 *	The signature of the comparison function that could be used by the binary search function (C generic version).
	 *
	 *	It should return 0			  if the first argument is equal   to   the second, 
	 *			 a negative integer value if the first argument is less    than the second, 
	 *			 a positive integer value if the first argument is greater than the second.
	 */
	using compare_function_type = int(*)(const void* ptrL, const void* ptrR);

	/*
	 *	[ basic version ]
	 *
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

	/*
	 *	[ C generic version ]
	 *
	 *	function parameters:
	 *				key:   pointer to the element to search for
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

	/*
	 *	[ C++ function template version ]
	 *
	 *	function parameters:
	 *				ary:  the data array with elements to examine
	 *				key:  the element to search for
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

	/*
	 *	[ C++ STL version ]
	 */
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

	//variable template to check if a type has find() member function
	template<class, class = void>
	constexpr bool has_find_function = false;

	template<class T>
	constexpr bool has_find_function<T, std::void_t<decltype(std::declval<T>().find(T::key_type()))>> = true;

	/*
	 *	[ customized version ]
	 *
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
	 *	[ customized version ]
	 *
	 *	function parameters:
	 *				cont:  the data array or data container with elements to examine
	 *				value: value to search for
	 *				comp:  comparison function or binary predicate which returns 
	 *				       true if the first argument is less than the second, false otherwise 
	 *				       the signature of the comparison function should be: 
	 *				       bool func(const Type1& lhs, const Type2& rhs)
	 *
	 *	return value: iterator pointing to the found element, or iterator one past the end of the data container otherwise.
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
}
