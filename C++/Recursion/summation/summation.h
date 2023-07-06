#pragma once

/*
 *	sum_1toN(n) =	1 + 2 + 3 + ... + (n - 1) + n = n * (n + 1) / 2, if n > 0
 *
 *					0, otherwise
 */

#include <iostream>
#include <type_traits>
#include <limits>
#include <safeint.h>

namespace cust					//customized / non-standard
{
	namespace recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		constexpr U sum_1toN(T n)
		{
			return n < 1 ? 0 : n + sum_1toN(n - 1);
		}
	}

	namespace non_recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		constexpr U sum_1toN(T n)
		{
			U result {};

			for (T i { 1 }; i <= n; ++i)
				result += static_cast<U>(i);

			return result;
		}
	}

	namespace mathematical_formula_version
	{
		//check if the integer is odd or not
		template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
		constexpr bool isOdd(T n)
		{
			return (n & 1) == 1;
		}

		template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		U sum_1toN(T n)
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
	}
}
