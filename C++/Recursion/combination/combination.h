#pragma once

/*
 *		Combination (each element is unique)
 *
 *		function parameter:
 *
 *							first, last: the range of elements to generate combinations
 *							k: the number of elements to form a combination
 *
 *		return value: all possible combination results
 */

#include <iostream>
#include <type_traits>
#include <vector>
#include <cstddef>
#include <iterator>
#include <stack>

namespace cust					//customized / non-standard
{
	namespace recursive_version
	{
		using std::size_t;
		using std::vector;

		/*
		 *	combination(n, k) = combination(n - 1, k - 1) + combination(n - 1, k)
		 */
		template<class T, class U>
		constexpr U comb(T n, T k)
		{
			return k == 0 || k == n ? 1 : (k == 1 ? n : comb<T, U>(n - 1, k - 1) + comb<T, U>(n - 1, k));
		}

		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
		constexpr U combination(T n, T k)
		{
			//input checking
			if (n < 0 || k < 0 || n < k)
				return 0;						//invalid input

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

			return results;						//return value optimization (RVO) is expected
		}
	}

	namespace non_recursive_version				//mathematical formula
	{
		using std::size_t;
		using std::vector;
		using std::stack;

		/*
		 *								n!
		 *	combination(n, k) = ---------------------
		 *							(n - k)! * k!
		 */
		template<class T, class U = T, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>, int> = 0>
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

			return results;						//return value optimization (RVO) is expected
		}
	}
}
