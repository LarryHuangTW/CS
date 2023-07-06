#pragma once

/*
 *	GCD: Greatest Common Divisor
 *
 *				 _
 *				|	0		,if m = 0 and n = 0
 *				|	
 *		GCD(m, n) =	|	n		,if m % n = 0
 *				|
 *				|_	GCD(n, m % n)	,otherwise
 *
 *
 *	LCM: Least Common Multiple
 *
 *		LCM(m, n) = (m / GCD(m, n)) * n
 */

#include <iostream>
#include <type_traits>
#include <limits>
#include <utility>
#include <safeint.h>

namespace cust					//customized / non-standard
{
	//computes the absolute value of the integer number and casts it to unsigned type
	template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	constexpr auto unsigned_abs(T value)
	{
		return static_cast<std::make_unsigned_t<T>>(value < 0 && value != std::numeric_limits<T>::min() ? 0 - value : value);
	}

	namespace recursive_version
	{
		template<class T>
		constexpr T getGCD(T a, T b)
		{
			return b == 0 ? a : getGCD(b, a % b);
		}

		//computes the greatest common divisor of the integer numbers m and n
		template<class M, class N, class T = std::enable_if_t<std::is_integral_v<M> && std::is_integral_v<N>, std::common_type_t<M, N>>>
		constexpr T gcd(M m, N n)
		{
			return static_cast<T>(getGCD(unsigned_abs<T>(m), unsigned_abs<T>(n)));
		}

		template<class M, class... Ns, std::enable_if_t<std::is_integral_v<M> && std::conjunction_v<std::is_integral<Ns>...>, int> = 0>
		constexpr auto gcd(M m, Ns... ns)
		{
			return ((m = static_cast<M>(gcd(m, ns))), ...);
		}
	}

	namespace non_recursive_version
	{
		//computes the greatest common divisor of the integer numbers m and n
		template<class M, class N, class T = std::enable_if_t<std::is_integral_v<M> && std::is_integral_v<N>, std::common_type_t<M, N>>>
		constexpr T gcd(M m, N n)
		{
			auto um { unsigned_abs<T>(m) }, un { unsigned_abs<T>(n) };

			if (un == 0)
				return static_cast<T>(um);

			for ( ; (um %= un) != 0; std::swap(um, un));

			return static_cast<T>(un);
		}
	}

	//computes the least common multiple of the integer numbers m and n
	template<class M, class N, class T = std::enable_if_t<std::is_integral_v<M> && std::is_integral_v<N>, std::common_type_t<M, N>>>
	constexpr T lcm(M m, N n)
	{
		auto um { unsigned_abs<T>(m) }, un { unsigned_abs<T>(n) };

		return um == 0 || un == 0 ? 0 : static_cast<T>((um / recursive_version::gcd(um, un)) * un);
	}

	template<class M, class... Ns, std::enable_if_t<std::is_integral_v<M> && std::conjunction_v<std::is_integral<Ns>...>, int> = 0>
	constexpr auto lcm(M m, Ns... ns)
	{
		return ((m = static_cast<M>(lcm(m, ns))), ...);
	}

	//computes the least common multiple of the integer numbers m and n (with overflow checking)
	template<class M, class N, class T = std::enable_if_t<std::is_integral_v<M> && std::is_integral_v<N>, std::common_type_t<M, N>>>
	T lcm_oc(M m, N n)
	{
		using namespace msl::utilities;

		auto um { unsigned_abs<T>(m) }, un { unsigned_abs<T>(n) };

		if (um == 0 || un == 0)
			return 0;

		using U = decltype(um);

		try
		{
			return static_cast<T>(SafeInt<U>(um / non_recursive_version::gcd(um, un)) * SafeInt<U>(un));
		}
		catch (SafeIntException)
		{
			std::cerr << "cust::lcm_oc(" << m << ", " << n << ") possible overflow\n\n";
		}

		return static_cast<T>((um / non_recursive_version::gcd(um, un)) * un);
	}
}
