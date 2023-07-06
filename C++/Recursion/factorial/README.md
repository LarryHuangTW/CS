## Basic idea:

### 1. recursive version:

```C++
/*
 *	factorial(n) =	1 * 2 * 3 * ... * (n - 1) * n = n!, if n > 0
 *
 *			1, otherwise
 */

using std::size_t;

constexpr size_t factorial(size_t n)
{
	return n < 1 ? 1 : n * factorial(n - 1);
}
```

### 2. non-recursive version:

```C++
using std::size_t;

constexpr size_t factorial(size_t n)
{
	size_t result { 1 };

	for (size_t i { 1 }; i <= n; ++i)
		result *= i;

	return result;
}
```

## With overflow checking:

### 1. recursive version:

```C++
template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
U factorial(T n)
{
	if (n < 1)
		return 1;

	auto tmp { factorial(n - 1) };

	if (static_cast<U>(n * tmp) / n != tmp)
		std::cerr << "possible overflow\n";

	return n * tmp;
}
```

### 2. constexpr recursive version:

```C++
using std::size_t;

template<size_t N>
constexpr size_t factorial()
{
	constexpr auto tmp { factorial<N - 1>() };

	static_assert((N * tmp) / N == tmp, "possible overflow");

	return N * tmp;
}

template<>
constexpr size_t factorial<0>()
{
	return 1;
}
```

### 3. non-recursive version:

```C++
/*
 *	To calculate the upper bound argument n of function factorial() that won't cause overflow
 */
#pragma warning(disable:4307)
template<class U>
constexpr U factorial_limit(U)
{
	U n { 1 };

	for (U prev { 1 }, curr { 1 }; prev == curr / n; )
	{
		prev = curr;
		curr *= ++n;
	}

	return --n;
}
#pragma warning(default:4307)

template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
U factorial(T n)
{
	static const U limit { factorial_limit(U{}) };

	U result { 1 };

	if (limit < n)
		std::cerr << "possible overflow\n";

	for (T i { 1 }; i <= n; ++i)
		result *= static_cast<U>(i);

	return result;
}
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include "factorial.h"

template<class T>
void test_func(auto factorial, T)
{
	for (T i { 0 }; i < 23; ++i)
		std::cout << "factorial(" << i << ") = " << factorial(i) << "\n";
	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	using std::size_t;

	{
		using namespace cust::constexpr_recursive_version;

		static_assert(factorial(0)  == 1);
		static_assert(factorial(1)  == 1);
		static_assert(factorial(2)  == 2);
		static_assert(factorial(3)  == 6);
		static_assert(factorial(10) == 3628800);
		//static_assert(factorial(13));			//overflowed with a compile-time error
		static_assert(factorial(20ULL));
		static_assert(factorial(21ULL));		//overflowed but no overflow checking
	}

	{
		using namespace cust::constexpr_recursive_version::overflow_checking;

		static_assert(factorial<0>()  == 1);
		static_assert(factorial<1>()  == 1);
		static_assert(factorial<2>()  == 2);
		static_assert(factorial<3>()  == 6);
		static_assert(factorial<10>() == 3628800);
		static_assert(factorial<20>());
		//static_assert(factorial<21>());		//overflowed with a compile-time error
	}

	test_func(cust::recursive_version::factorial<size_t>, size_t{});

	test_func(cust::non_recursive_version::factorial<size_t>, size_t{});

	return 0;
}
```
