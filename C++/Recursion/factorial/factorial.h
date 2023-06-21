#pragma once

/*
 *	factorial(n) =	1 * 2 * 3 * ... * (n - 1) * n = n!, if 1 < n
 *
 *					1, otherwise
 */

#include <iostream>
#include <type_traits>
#include <cstddef>

namespace cust					//customized / non-standard
{
	namespace recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		U factorial(T n)
		{
			if (n < 2)
				return 1;

			auto tmp { factorial(n - 1) };

			if (static_cast<U>(n * tmp) / n != tmp)
				std::cerr << "possible overflow\n";

			return n * tmp;
		}
	}

	namespace constexpr_recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		constexpr U factorial(T n)
		{
			return n < 2 ? 1 : n * factorial(n - 1);
		}

		namespace overflow_checking
		{
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
				return 1ULL;
			}
		}
	}

	namespace non_recursive_version
	{
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
	}
}
