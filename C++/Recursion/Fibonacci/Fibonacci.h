#pragma once

/*					 _
					|	fibonacci(n + 2) - fibonacci(n + 1) , if n < 0
					|
	fibonacci(n) =	|-	n									, if n = 0 , 1
					|
					|_	fibonacci(n - 2) + fibonacci(n - 1) , if n > 1
*/

#include <iostream>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace cust					//customized / non-standard
{
	//check if the integer is odd or not
	template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	constexpr bool isOdd(T n)
	{
		return (n & 1) == 1;
	}

	namespace recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
		constexpr U fibonacci(T n)
		{
			return n < 0 ? fibonacci(n + 2) - fibonacci(n + 1) : (1 < n ? fibonacci(n - 2) + fibonacci(n - 1) : n);
		}
	}

	namespace non_recursive_version
	{
		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
		U fibonacci(T n)
		{
			U    num[3] { 0 , 1 , static_cast<U>(n == 0 ? 0 : 1) };
			bool flag   { false };						//negative sign of result

			if (n < 0)
			{
				n = 0 - n;

				flag = !cust::isOdd(n);
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
	}

	namespace dynamic_programming_version
	{
		using std::unordered_map;

		template<class T, class U>
		U fibonacci(T n, unordered_map<T, U>& dp)
		{
			auto iter { dp.find(n) };

			if (iter != dp.end())
				return iter->second;

			return dp[n] = fibonacci(n - 2, dp) + fibonacci(n - 1, dp);
		}

		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
		U fibonacci(T n)
		{
			bool flag { false };						//negative sign of result
			unordered_map<T, U> dp {};

			if (n < 0)
			{
				n = 0 - n;

				flag = !cust::isOdd(n);
			}

			dp[0] = 0;
			dp[1] = 1;

			fibonacci(n, dp);

			return flag ? 0 - dp[n] : dp[n];
		}
	}

	namespace fake_dynamic_programming_version			//fake and simplified dynamic programming
	{
		using std::vector;
		using std::size_t;

		template<class T, class U>
		constexpr void fibonacci(T n, vector<U>& dp)
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
			bool flag { false };						//negative sign of result
			vector<U> dp {};

			if (n < 0)
			{
				n = 0 - n;

				flag = !cust::isOdd(n);
			}

			dp.reserve(static_cast<size_t>(n) + 1);
			dp.insert(dp.end(), n + 1, 0);

			if (0 < n)
				dp[1] = 1;

			fibonacci(n, dp);

			return flag ? 0 - dp[n] : dp[n];
		}
	}
}
