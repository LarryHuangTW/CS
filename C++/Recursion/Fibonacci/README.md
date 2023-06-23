![Fibonacci numbers](/Images/Fibonacci.svg)

### 1. recursive version:

```C++
/*			 _
			|	fibonacci(n + 2) - fibonacci(n + 1) , if n < 0
			|
	fibonacci(n) =	|-	n				, if n = 0 , 1
			|
			|_	fibonacci(n - 2) + fibonacci(n - 1) , if n > 1
*/

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr U fibonacci(T n)
{
	return n < 0 ? fibonacci(n + 2) - fibonacci(n + 1) : (1 < n ? fibonacci(n - 2) + fibonacci(n - 1) : n);
}
```

### 2. dynamic programming version:

```C++
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr bool isOdd(T n)
{
	return (n & 1) == 1;
}

template<class T, class U>
U fibonacci(T n, std::unordered_map<T, U>& dp)
{
	auto iter { dp.find(n) };

	if (iter != dp.end())
		return iter->second;

	return dp[n] = fibonacci(n - 2, dp) + fibonacci(n - 1, dp);
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
U fibonacci(T n)
{
	bool flag { false };			//negative sign of result
	std::unordered_map<T, U> dp {};

	if (n < 0)
	{
		n = 0 - n;

		flag = !isOdd(n);
	}

	dp[0] = 0;
	dp[1] = 1;

	fibonacci(n, dp);

	return flag ? 0 - dp[n] : dp[n];
}
```

### 3. fake dynamic programming version:

```C++
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr bool isOdd(T n)
{
	return (n & 1) == 1;
}

template<class T, class U>
constexpr void fibonacci(T n, std::vector<U>& dp)
{
	if (1 < n)
	{
		fibonacci(n - 1, dp);

		dp[n] = dp[n - 2] + dp[n - 1];
	}
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr U fibonacci(T n)
{
	bool flag { false };		//negative sign of result
	std::vector<U> dp {};

	if (n < 0)
	{
		n = 0 - n;

		flag = !isOdd(n);
	}

	dp.reserve(static_cast<std::size_t>(n) + 1);
	dp.insert(dp.end(), n + 1, 0);

	if (0 < n)
		dp[1] = 1;

	fibonacci(n, dp);

	return flag ? 0 - dp[n] : dp[n];
}
```

### 4. non-recursive version (with overflow checking):

```C++
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr bool isOdd(T n)
{
	return (n & 1) == 1;
}

template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
U fibonacci(T n)
{
	U    num[3] { 0 , 1 , static_cast<U>(n == 0 ? 0 : 1) };
	bool flag   { false };						//negative sign of result

	if (n < 0)
	{
		n = 0 - n;

		flag = !isOdd(n);
	}

	for (T i { 2 }; i <= n; ++i)
	{
		num[2] = num[0] + num[1];

		//check the possibility of overflow
		if (num[2] < num[1])
			std::cerr << "possible overflow\n";

		num[0] = num[1];
		num[1] = num[2];
	}

	return flag ? 0 - num[2] : num[2];
}
```

## Example:

```C++
#include <iostream>
#include "Fibonacci.h"

void test_func(auto fibonacci, int n)
{
	if (0 <= n)
	{
		for (int i { 0 }; i <= n; ++i)
			std::cout << fibonacci(i) << "\t";
	}
	else
	{
		for (int i { 0 }; n <= i; --i)
			std::cout << fibonacci(i) << "\t";
	}
	
	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	const int num { 10 };

	{
		using namespace cust::recursive_version;

		static_assert(fibonacci(0)  ==    0);
		static_assert(fibonacci(1)  ==    1);
		static_assert(fibonacci(2)  ==    1);
		static_assert(fibonacci(3)  ==    2);
		static_assert(fibonacci(10) ==   55);
		static_assert(fibonacci(20) == 6765);

		test_func(fibonacci<int>,  num);
		test_func(fibonacci<int>, -num);

		std::cout << "fibonacci(30ULL) = " << fibonacci(30ULL) << "\n\n";
	}

	{
		using namespace cust::dynamic_programming_version;

		test_func(fibonacci<int, int, 0>,  num);
		test_func(fibonacci<int, int, 0>, -num);

		std::cout << "fibonacci(50ULL) = " << fibonacci(50ULL) << "\n\n";
	}

	{
		using namespace cust::fake_dynamic_programming_version;

		static_assert(fibonacci(0)  ==    0);
		static_assert(fibonacci(1)  ==    1);
		static_assert(fibonacci(2)  ==    1);
		static_assert(fibonacci(3)  ==    2);
		static_assert(fibonacci(10) ==   55);
		static_assert(fibonacci(20) == 6765);

		test_func(fibonacci<int, int, 0>,  num);
		test_func(fibonacci<int, int, 0>, -num);

		std::cout << "fibonacci(50ULL) = " << fibonacci(50ULL) << "\n\n";
	}

	{
		using namespace cust::non_recursive_version;

		test_func(fibonacci<int>,  num);
		test_func(fibonacci<int>, -num);

		std::cout << "fibonacci(49)    = " << fibonacci(49)    << "\n\n";			//possible overflow
		std::cout << "fibonacci(50ULL) = " << fibonacci(50ULL) << "\n\n";
	}

	return 0;
}
```

**runtime performance:** non-recursive >> fake dynamic programming >> dynamic programming >> recursive
