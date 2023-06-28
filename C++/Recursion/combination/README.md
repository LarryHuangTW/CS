![combination](/Images/Recursion/combination.svg)

```C++
/*
 *	Combination (each element is unique)
 *
 *	function parameter:
 *				first, last: the range of elements to generate combinations
 *				k: the number of elements to form a combination
 *
 *	return value: all possible combination results
 */
```

### 1. recursive version:

```C++
/*
 *	combination(n, k) = combination(n - 1, k - 1) + combination(n - 1, k)
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

template<class FwdIter, class T = typename std::iterator_traits<FwdIter>::value_type>
void comb(FwdIter first, size_t n, size_t k, vector<T>& tmp, vector<vector<T>>& results)
{
	if (k == 0)
	{
		results.push_back(tmp);

		return;
	}

	for ( ; k <= n; --n, ++first)
	{
		tmp.push_back(*first);

		comb(std::next(first), n - 1, k - 1, tmp, results);

		tmp.pop_back();
	}
}

template<class FwdIter, class T = typename std::iterator_traits<FwdIter>::value_type>
vector<vector<T>> combination(FwdIter first, FwdIter last, size_t k)
{
	const auto n { static_cast<size_t>(std::distance(first, last)) };
	vector<T> tmp {};
	vector<vector<T>> results {};

	if (0 < n && k <= n)
	{
		const auto sz { combination(n, k) };

		tmp.reserve(k);
		results.reserve(sz);

		comb(first, n, k, tmp, results);

		if (results.size() != sz)
			std::cerr << "size " << results.size() << " != " << sz << "\n";
	}

	return results;
}
```

### 2. non-recursive version (with overflow checking):

```C++
/*
 *				      n!
 *	combination(n, k) = ---------------------
 *				(n - k)! * k!
 */

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

template<class FwdIter, class T = typename std::iterator_traits<FwdIter>::value_type>
vector<vector<T>> combination(FwdIter first, FwdIter last, size_t k)
{
	struct stk_val
	{
		FwdIter iter;
		size_t  n;
		size_t  k;
	};

	const auto n  { static_cast<size_t>(std::distance(first, last)) };
	const auto sz { combination(n, k) };

	stack<stk_val>    stk {};
	vector<T>         tmp {};
	vector<vector<T>> results {};

	if (0 < n && k <= n)
	{
		tmp.reserve(k);
		results.reserve(sz);

		stk.emplace(first, n, k);
	}

	while ( !stk.empty() )
	{
		if (0 < stk.top().k && stk.top().k <= stk.top().n)
		{
			tmp.push_back(*stk.top().iter);

			stk.emplace(std::next(stk.top().iter), stk.top().n - 1, stk.top().k - 1);
		}
		else
		{
			if (stk.top().k == 0)
				results.push_back(tmp);

			stk.pop();

			if ( !tmp.empty() )
				tmp.pop_back();

			if ( !stk.empty() )
			{
				++stk.top().iter;
				--stk.top().n;
			}
		}
	}

	if (results.size() != sz)
		std::cerr << "size " << results.size() << " != " << sz << "\n";

	return results;
}
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include <string>
#include "combination.h"

template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void test_func(auto combination, T n)
{
	for (T i { 0 }; i <= n; ++i) {
		for (T j { 0 }; j <= i; ++j)
			std::cout << combination(i, j) << "   ";
		std::cout << "\n";
	}
	std::cout << "\n";
}

void test_func(auto combination, const std::string& str)
{
	using std::size_t;

	for (size_t n { 1 }; n <= str.size(); ++n) {
		for (size_t k { 1 }; k <= n; ++k) {
			auto results { combination(str.begin(), std::next(str.begin(), n), k) };

			std::cout << "There are  " << results.size() << " results of combination(" << n << ", " << k << ")\n\n";

			for (const auto& vec : results)
			{
				std::cout << "(";

				for (auto elem : vec)
					std::cout << elem << " ";

				std::cout << "\b) , ";
			}

			std::cout << "\b\b \n\n";
		}
	}
}

int main(int argc, char* argv[])
{
	using value_type = int;

	const value_type num { 10 };

	const std::string str { "ABCDEFG" };

	using iterator_type = typename std::string::const_iterator;

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

		test_func(combination<iterator_type>, str);
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

		test_func(combination<iterator_type>, str);
	}

	return 0;
}
```
