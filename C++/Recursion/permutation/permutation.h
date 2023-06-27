#pragma once

/*
 *		Permutation (each element is unique)
 *
 *		function parameter: 
 *
 *							data: a collection of distinct elements to permutate
 *
 *		return value: all possible permutation results
 */

#include <numeric>
#include <vector>
#include <utility>
#include <cstddef>

namespace cust					//customized / non-standard
{
	using std::size_t;

	constexpr size_t factorial(size_t n)
	{
		size_t result { 1 };

		for (size_t i { 2 }; i <= n; ++i)
			result *= i;

		return result;
	}

	namespace recursive_version
	{
		using std::vector;

		template<class T>
		constexpr void permute(T& data, size_t idx, vector<T>& results)
		{
			if (idx + 1 == data.size()) {
				results.push_back(data);
			}
			else {
				for (size_t i { idx }; i < data.size(); ++i)
				{
					std::swap(data[idx], data[i]);

					permute(data, idx + 1, results);

					std::swap(data[idx], data[i]);			//restore previous swap
				}
			}
		}

		template<class T>
		constexpr vector<T> permutate(T& data)
		{
			vector<T> results {};

			results.reserve(factorial(data.size()));

			permute(data, 0, results);

			return results;
		}

		namespace Heap_algorithm
		{
			//check if the integer is odd or not
			template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
			constexpr bool isOdd(T n)
			{
				return (n & 1) == 1;
			}

			template<class T>
			constexpr void permute(T& data, size_t sz, vector<T>& results)
			{
				if (sz == 1) {
					results.push_back(data);
				}
				else {
					permute(data, sz - 1, results);

					for (size_t i = 0; i + 1 < sz; ++i)
					{
						std::swap(data[isOdd(sz) ? 0 : i], data[sz - 1]);

						permute(data, sz - 1, results);
					}
				}
			}

			template<class T>
			constexpr vector<T> permutate(T& data)
			{
				vector<T> results {};

				if ( !data.empty() ) {
					results.reserve(factorial(data.size()));

					permute(data, data.size(), results);
				}

				return results;
			}
		}
	}

	namespace non_recursive_version
	{
		using std::size_t;
		using std::vector;

		template<class T>
		constexpr vector<T> permutate(T& data)
		{
			const auto sz { data.size() };
			vector<size_t> indices(sz);
			vector<T>      results {};

			if (data.empty())
				return results;

			results.reserve(factorial(sz));

			//initialize indices with 0, 1, 2, 3, ...
			std::iota(indices.begin(), indices.end(), 0);

			indices[sz - 1] = sz;

			if (sz == 1)
				results.push_back(data);

			for (size_t i { 0 }; indices[0] != sz; )
			{
				if (indices[i] == sz)
				{
					if (i + 1 == sz)
						results.push_back(data);
					else
						indices[i] = i;

					--i;

					std::swap(data[i], data[indices[i]]);			//restore previous swap

					++indices[i];
				}
				else
				{
					std::swap(data[i], data[indices[i]]);			//swap and generate a new permutation

					++i;
				}
			}

			return results;
		}
	}
}
