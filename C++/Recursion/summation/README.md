![summation](/Images/Recursion/summation.svg)

### 1. recursive version:

```C++
/*
 *	sum_1toN(n) =	1 + 2 + 3 + ... + (n - 1) + n = n * (n + 1) / 2, if 0 < n
 *
 *			0, otherwise
 */

template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	return n < 1 ? 0 : n + sum_1toN(n - 1);
}
```

### 2. non-recursive version:

```C++
template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	U result {};

	for (T i { 1 }; i <= n; ++i)
		result += static_cast<U>(i);

	return result;
}
```

### 3. mathematical formula version (with overflow checking):

```C++
//to check if the integer is odd or not
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr bool isOdd(T n)
{
	return (n & 1) == 1;
}

template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	using namespace msl::utilities;

	bool flag { isOdd(n) };				//a flag to indicate the number n is odd or not
	U    num  { (n + flag) / 2 }, result {};

	if (0 < n)
	{
		++n -= flag;

		if ( !SafeMultiply(num, n, result) )
			std::cerr << "possible overflow\n";
	}

	return result;
}
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include <array>
#include "summation.h"

template<class T, std::size_t N>
void test_func(auto sum_1toN, const std::array<T, N>& test_cases)
{
	for(auto n : test_cases)
		std::cout << "sum(" << n << ") = " << sum_1toN(n) << "\n";
	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	std::array<int, 14> test_cases { -2 , -1 , 0 , 1 , 2 , 3 , 10 , 11 , 100 , 101 , 1000 , 1001 , 10000 , 10001 };

	using value_type = typename decltype(test_cases)::value_type;

	{
		using namespace cust::recursive_version;

		static_assert(sum_1toN(-2)  ==    0);
		static_assert(sum_1toN(-1)  ==    0);
		static_assert(sum_1toN(0)   ==    0);
		static_assert(sum_1toN(1)   ==    1);
		static_assert(sum_1toN(2)   ==    3);
		static_assert(sum_1toN(3)   ==    6);
		static_assert(sum_1toN(10)  ==   55);
		static_assert(sum_1toN(100) == 5050);
	}

	{
		using namespace cust::non_recursive_version;

		static_assert(sum_1toN(-2)  ==    0);
		static_assert(sum_1toN(-1)  ==    0);
		static_assert(sum_1toN(0)   ==    0);
		static_assert(sum_1toN(1)   ==    1);
		static_assert(sum_1toN(2)   ==    3);
		static_assert(sum_1toN(3)   ==    6);
		static_assert(sum_1toN(10)  ==   55);
		static_assert(sum_1toN(100) == 5050);
	}

	std::cout << "[ recursive version ]\n\n";
	test_func(cust::recursive_version::sum_1toN<value_type>, test_cases);

	std::cout << "[ non-recursive version ]\n\n";
	test_func(cust::non_recursive_version::sum_1toN<value_type>, test_cases);

	std::cout << "[ mathematical formula version ]\n\n";
	test_func(cust::mathematical_formula_version::sum_1toN<value_type>, test_cases);

	{
		using namespace cust::mathematical_formula_version;
		using namespace msl::utilities;

		std::cout << "sum_1toN(5000000000ULL) = " << sum_1toN(5000000000ULL) << "\n";
		std::cout << "sum_1toN(6000000000ULL) = " << sum_1toN(6000000000ULL) << "\n";
		std::cout << "sum_1toN(6100000000ULL) = " << sum_1toN(6100000000ULL) << "\n";	//possible overflow
	}

	return 0;
}
```
