![Pascal triangle](/Images/Recursion/Pascal_triangle.svg)

### 1. recursive version:

```C++
/*
 *	To calculate and return row n of Pascal triangle
 *
 *	function parameter:
 *				n:   the row number, n = 0, 1, 2, 3 ...
 *
 *	return value: the row data of Pascal triangle
 */

template<class T, class U>
constexpr U pascal(T n, T k)
{
	return k == 0 || n == k ? 1 : pascal<T, U>(n - 1, k - 1) + pascal<T, U>(n - 1, k);
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr std::vector<U> pascal(T n)
{
	std::vector<U> vec {};

	if (0 <= n)
	{
		vec.reserve(n + 1);

		for (T k { 0 }; k <= n; ++k)
			vec.push_back(pascal<T, U>(n, k));
	}

	return vec;
}
```

### 2. non-recursive version (from left to right):

```C++
template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr std::vector<U> pascal(T n)
{
	std::vector<U> vec {};

	if (0 <= n)
	{
		vec.reserve(n + 1);
		vec.insert(vec.end(), n + 1, 0);
		vec.front() = 1;				//pascal(n, 0) == 1

		for (T i { 0 }; i <= n; ++i)
		{
			U tmp { 1 };

			for (T j { 1 }; j <= i; ++j)
			{
				tmp += vec[j];

				std::swap(tmp, vec[j]);
			}
		}
	}

	return vec;
}
```

### 3. non-recursive version (from right to left):

```C++
template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr std::vector<U> pascal(T n)
{
	std::vector<U> vec {};

	if (0 <= n)
	{
		vec.reserve(n + 1);
		vec.insert(vec.end(), n + 1, 0);
		vec.front() = 1;				//pascal(n, 0) == 1

		for (T i { 0 }; i <= n; ++i)
			for (T j { i }; 0 < j; --j)
				vec[j] += vec[j - 1];
	}

	return vec;
}
```

### 4. non-recursive version (from middle to two sides):

```C++
template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr std::vector<U> pascal(T n)
{
	std::vector<U> vec {};

	if (0 <= n)
	{
		vec.reserve(n + 1);
		vec.insert(vec.end(), n + 1, 0);

		for (T i { 0 }; i <= n; ++i) {
			for (T j { i >> 1 }; 0 < j; --j)
				vec[i - j] = vec[j] += vec[j - 1];
			vec[i] = 1;					//pascal(n, n) == 1
		}
	}

	return vec;
}
```

### 5. mathematical formula:

```C++
template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr U combination(T n, T k)
{
	//input checking
	if (n < 0 || k < 0 || n < k)
		return 0;				//invalid input

	if (n < 2 * k)
		k = n - k;

	U result { k == 0 ? 1 : static_cast<U>(n) };

	for (U i { static_cast<U>(n) - 1 }, j { 2 }; j <= k; --i, ++j)
	{
		auto prev { result };

		result *= i;

		if (result / i != prev)
			std::cerr << "possible overflow\n";

		result /= j;
	}

	return result;
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr std::vector<U> pascal(T n)
{
	std::vector<U> vec {};

	if (0 <= n)
	{
		vec.reserve(n + 1);

		for (T k { 0 }; k <= n; ++k)
			vec.push_back(combination<U>(n, k));
	}

	return vec;
}
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include <string>
#include "Pascal_triangle.h"

template<class T>
void test_func(const std::string& str, auto pascal, T num)
{
	std::cout << str << " :\n\n";

	for (T i { 0 }; i < num; ++i)
	{
		auto result { pascal(i) };

		for (auto item : result)
			std::cout << item << "   ";
		std::cout << "\n\n\n";
	}
}

int main(int argc, char* argv[])
{
	using std::vector;

	using value_type = int;

	const value_type num { 11 };

	{
		using namespace cust::recursive_version;

		static_assert(pascal(0) == vector<int>{ 1 });
		static_assert(pascal(1) == vector<int>{ 1 , 1 });
		static_assert(pascal(2) == vector<int>{ 1 , 2 ,  1 });
		static_assert(pascal(3) == vector<int>{ 1 , 3 ,  3 ,  1 });
		static_assert(pascal(4) == vector<int>{ 1 , 4 ,  6 ,  4 , 1 });
		static_assert(pascal(5) == vector<int>{ 1 , 5 , 10 , 10 , 5 , 1 });

		test_func("recursive", pascal<value_type>, num);
	}

	{
		using namespace cust::mathematical_formula;

		static_assert(pascal(0) == vector<int>{ 1 });
		static_assert(pascal(1) == vector<int>{ 1 , 1 });
		static_assert(pascal(2) == vector<int>{ 1 , 2 ,  1 });
		static_assert(pascal(3) == vector<int>{ 1 , 3 ,  3 ,  1 });
		static_assert(pascal(4) == vector<int>{ 1 , 4 ,  6 ,  4 , 1 });
		static_assert(pascal(5) == vector<int>{ 1 , 5 , 10 , 10 , 5 , 1 });

		test_func("mathematical formula", pascal<value_type>, num);

		auto result = pascal(30);					//possible overflow

		std::cout << "pascal(30) : ";

		for (auto item : result)
			std::cout << item << "   ";
		std::cout << "\n\n";
	}

	{
		using namespace cust::non_recursive_version;

		{
			using namespace from_left_to_right;

			static_assert(pascal(0) == vector<int>{ 1 });
			static_assert(pascal(1) == vector<int>{ 1 , 1 });
			static_assert(pascal(2) == vector<int>{ 1 , 2 ,  1 });
			static_assert(pascal(3) == vector<int>{ 1 , 3 ,  3 ,  1 });
			static_assert(pascal(4) == vector<int>{ 1 , 4 ,  6 ,  4 , 1 });
			static_assert(pascal(5) == vector<int>{ 1 , 5 , 10 , 10 , 5 , 1 });

			test_func("non-recursive (from left to right)", pascal<value_type>, num);
		}

		{
			using namespace from_right_to_left;

			static_assert(pascal(0) == vector<int>{ 1 });
			static_assert(pascal(1) == vector<int>{ 1 , 1 });
			static_assert(pascal(2) == vector<int>{ 1 , 2 ,  1 });
			static_assert(pascal(3) == vector<int>{ 1 , 3 ,  3 ,  1 });
			static_assert(pascal(4) == vector<int>{ 1 , 4 ,  6 ,  4 , 1 });
			static_assert(pascal(5) == vector<int>{ 1 , 5 , 10 , 10 , 5, 1 });

			test_func("non-recursive (from right to left)", pascal<value_type>, num);
		}

		{
			using namespace from_middle_to_two_sides;

			static_assert(pascal(0) == vector<int>{ 1 });
			static_assert(pascal(1) == vector<int>{ 1 , 1 });
			static_assert(pascal(2) == vector<int>{ 1 , 2 ,  1 });
			static_assert(pascal(3) == vector<int>{ 1 , 3 ,  3 ,  1 });
			static_assert(pascal(4) == vector<int>{ 1 , 4 ,  6 ,  4 , 1 });
			static_assert(pascal(5) == vector<int>{ 1 , 5 , 10 , 10 , 5, 1 });

			test_func("non-recursive (from middle to two sides)", pascal<value_type>, num);
		}
	}

	return 0;
}
```
