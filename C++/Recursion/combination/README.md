![combination](/Images/Recursion/combination.svg)

### 1. recursive version:

```C++
/*
	                            n!
	combination(n, k) = -------------------
	                       (n - k)! * k!
*/

template<class T, class U>
constexpr U comb(T n, T k)
{
	if (n < 2 * k)
		k = n - k;

	return k == 0 ? 1 : (k == 1 ? n : comb<T, U>(n - 1, k - 1) + comb<T, U>(n - 1, k));
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr U combination(T n, T k)
{
	//input checking
	if (n < 0 || k < 0 || n < k)
		return 0;			//invalid input

	return comb<T, U>(n, k);
}
```

### 2. non-recursive version (with overflow checking):

```C++
template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
constexpr U combination(T n, T k)
{
	//input checking
	if (n < 0 || k < 0 || n < k)
		return 0;			//invalid input

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
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include "combination.h"

template<class T>
void test_func(auto combination, T n)
{
	for (T i { 0 }; i <= n; ++i) {
		for (T j { 0 }; j <= i; ++j)
			std::cout << combination(i, j) << "   ";
		std::cout << "\n";
	}
	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	using value_type = int;

	const value_type num { 10 };

	{
		using namespace cust::recursive_version;

		static_assert(combination(0, 0) ==  1);
		static_assert(combination(1, 0) ==  1);
		static_assert(combination(1, 1) ==  1);
		static_assert(combination(2, 1) ==  2);
		static_assert(combination(3, 1) ==  3);
		static_assert(combination(3, 2) ==  3);
		static_assert(combination(4, 2) ==  6);
		static_assert(combination(5, 2) == 10);
		static_assert(combination(6, 2) == 15);
		static_assert(combination(6, 3) == 20);

		test_func(combination<value_type>, num);

		std::cout << "combination(30, 15) = " << combination(30, 15) << "\n";
		std::cout << "combination(34, 15) = " << combination(34, 15) << "\n";
		//std::cout << "combination(34, 16) = " << combination(34, 16) << "\n";			//overflowed
		std::cout << "combination(34, 16) = " << combination<std::size_t>(34, 16) << "\n\n";
	}

	{
		using namespace cust::non_recursive_version;

		static_assert(combination(0, 0) ==  1);
		static_assert(combination(1, 0) ==  1);
		static_assert(combination(1, 1) ==  1);
		static_assert(combination(2, 1) ==  2);
		static_assert(combination(3, 1) ==  3);
		static_assert(combination(3, 2) ==  3);
		static_assert(combination(4, 2) ==  6);
		static_assert(combination(5, 2) == 10);
		static_assert(combination(6, 2) == 15);
		static_assert(combination(6, 3) == 20);

		test_func(combination<value_type>, num);

		std::cout << "combination(30, 15) = " << combination(30, 15)              << "\n";	//possible overflow
		std::cout << "combination(30, 15) = " << combination<std::size_t>(30, 15) << "\n";
		std::cout << "combination(34, 15) = " << combination<std::size_t>(34, 15) << "\n";
		std::cout << "combination(34, 16) = " << combination<std::size_t>(34, 16) << "\n\n";
	}

	return 0;
}
```
