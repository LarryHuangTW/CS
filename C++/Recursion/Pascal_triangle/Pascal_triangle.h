#pragma once

/*
 *		To calculate and return row n of Pascal triangle
 *
 *		function parameter:
 *					n:   the row number, n = 0, 1, 2, 3 ...
 *
 *		return value: the row data of Pascal triangle
 */

#include <type_traits>
#include <vector>
#include <utility>
#include "combination.h"

namespace cust					//customized / non-standard
{
	namespace recursive_version
	{
		using std::vector;

		template<class T, class U>
		constexpr U pascal(T n, T k)
		{
			return k == 0 || k == n ? 1 : (k == 1 ? n : pascal<T, U>(n - 1, k - 1) + pascal<T, U>(n - 1, k));
		}

		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
		constexpr vector<U> pascal(T n)
		{
			vector<U> vec {};

			if (0 <= n)
			{
				vec.reserve(n + 1);

				for (T k { 0 }; k <= n; ++k)
					vec.push_back(pascal<T, U>(n, k));
			}

			return vec;			//return value optimization (RVO) is expected
		}
	}

	namespace mathematical_formula
	{
		using namespace cust::non_recursive_version;

		using std::vector;

		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
		constexpr vector<U> pascal(T n)
		{
			vector<U> vec {};

			if (0 <= n)
			{
				vec.reserve(n + 1);

				for (T k { 0 }; k <= n; ++k)
					vec.push_back(combination<U>(n, k));
			}

			return vec;			//return value optimization (RVO) is expected
		}
	}

	namespace non_recursive_version
	{
		using std::vector;

		namespace from_left_to_right
		{
			template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
			constexpr vector<U> pascal(T n)
			{
				vector<U> vec {};

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

				return vec;			//return value optimization (RVO) is expected
			}
		}

		namespace from_right_to_left
		{
			template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
			constexpr vector<U> pascal(T n)
			{
				vector<U> vec {};

				if (0 <= n)
				{
					vec.reserve(n + 1);
					vec.insert(vec.end(), n + 1, 0);
					vec.front() = 1;				//pascal(n, 0) == 1

					for (T i { 0 }; i <= n; ++i)
						for (T j { i }; 0 < j; --j)
							vec[j] += vec[j - 1];
				}

				return vec;			//return value optimization (RVO) is expected
			}
		}

		namespace from_middle_to_two_sides
		{
			template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
			constexpr vector<U> pascal(T n)
			{
				vector<U> vec {};

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

				return vec;			//return value optimization (RVO) is expected
			}
		}
	}
}
