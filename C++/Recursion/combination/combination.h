#pragma once

#include <iostream>
#include <type_traits>

namespace cust					//customized / non-standard
{
	namespace recursive_version
	{
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
				return 0;						//invalid input

			return comb<T, U>(n, k);
		}
	}

	namespace non_recursive_version				//mathematical formula
	{
		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>&& std::is_integral_v<U>, int> = 0>
		constexpr U combination(T n, T k)
		{
			//input checking
			if (n < 0 || k < 0 || n < k)
				return 0;						//invalid input

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
	}
}
