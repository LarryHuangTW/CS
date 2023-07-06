> *It's not reinventing the wheel. Just for practice and for fun.*<br>

![GCD and LCM](/Images/Recursion/GCD_and_LCM.svg)

### GCD recursive version:

```C++
/*
 *			 _
 *			|	0		,if m = 0 and n = 0
 *			|	
 *	GCD(m, n) =	|	n		,if m % n = 0
 *			|
 *			|_	GCD(n, m % n),	otherwise
 */

//computes the absolute value of the integer number and casts it to unsigned type
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr auto unsigned_abs(T value)
{
	return static_cast<std::make_unsigned_t<T>>(value < 0 && value != std::numeric_limits<T>::min() ? 0 - value : value);
}

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
```

### GCD non-recursive version:

```C++
//computes the absolute value of the integer number and casts it to unsigned type
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr auto unsigned_abs(T value)
{
	return static_cast<std::make_unsigned_t<T>>(value < 0 && value != std::numeric_limits<T>::min() ? 0 - value : value);
}

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
```

### LCM:

```C++
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
```

## Example:

```C++
#include <iostream>
#include <numeric>
#include <climits>
#include "GCD_and_LCM.h"

int main(int argc, char* argv[])
{
	static_assert(cust::unsigned_abs(0) == 0);
	static_assert(cust::unsigned_abs(1) == cust::unsigned_abs(-1));
	static_assert(cust::unsigned_abs(2) == cust::unsigned_abs(-2));
	static_assert(cust::unsigned_abs(3) == cust::unsigned_abs(-3));
	static_assert(cust::unsigned_abs(INT_MIN)     == cust::unsigned_abs(INT_MAX) + 1);
	static_assert(cust::unsigned_abs(INT_MIN + 1) == cust::unsigned_abs(INT_MAX));
	static_assert(cust::unsigned_abs(INT_MIN + 2) == cust::unsigned_abs(INT_MAX - 1));

	{
		using namespace std;

		constexpr auto m { std::numeric_limits<int>::min() };
		long long      n { m * 2LL };

		auto result { std::gcd(m, n) };

		std::cout << "type of result : " << typeid(decltype(result)).name() << "\n\n";
		std::cout << "std::gcd(" << m << ", " << n << ") = " << result << "\n\n";		// 2147483648

		std::cout << "std::gcd(INT_MIN, INT_MIN) = " << std::gcd(INT_MIN, INT_MIN) << "\n\n";	//-2147483648 (behavior is undefined)
	}

	{
		using namespace cust::recursive_version;

		static_assert(gcd(0, 0)     ==  0);
		static_assert(gcd(2, 0)     ==  2);
		static_assert(gcd(0, 3)     ==  3);
		static_assert(gcd(3, 5)     ==  1);
		static_assert(gcd(7, 2)     ==  1);
		static_assert(gcd(6, 10)    ==  2);
		static_assert(gcd(12, 12)   == 12);
		static_assert(gcd(15, 5)    ==  5);
		static_assert(gcd(63, 99)   ==  9);
		static_assert(gcd(-63, -99) ==  9);
		static_assert(gcd(-16, 0)   == 16);

		static_assert(gcd(10u, 20, 30LL)            == 10);
		static_assert(gcd(6, 3, 12)                 ==  3);
		static_assert(gcd(100, 98, 22, 8)           ==  2);
		static_assert(gcd(6, 90, 66, 42, 600, 6000) ==  6);
		static_assert(gcd(2, 3, 5, 7, 11, 13, 17)   ==  1);
	}

	{
		using namespace cust::non_recursive_version;

		static_assert(gcd(0, 0)     ==  0);
		static_assert(gcd(2, 0)     ==  2);
		static_assert(gcd(0, 3)     ==  3);
		static_assert(gcd(3, 5)     ==  1);
		static_assert(gcd(7, 2)     ==  1);
		static_assert(gcd(6, 10)    ==  2);
		static_assert(gcd(12, 12)   == 12);
		static_assert(gcd(15, 5)    ==  5);
		static_assert(gcd(63, 99)   ==  9);
		static_assert(gcd(-63, -99) ==  9);
		static_assert(gcd(-16, 0)   == 16);
	}

	std::cout << "std::lcm(INT_MIN, -1)      = " << std::lcm(INT_MIN, -1)      << "\n\n";	//-2147483648 (behavior is undefined)
	std::cout << "std::lcm(INT_MIN, INT_MAX) = " << std::lcm(INT_MIN, INT_MAX) << "\n\n";	//-2147483648 (behavior is undefined)

	static_assert(cust::lcm(0, 0)    ==  0);
	static_assert(cust::lcm(2, 0)    ==  0);
	static_assert(cust::lcm(0, 3)    ==  0);
	static_assert(cust::lcm(2, 2)    ==  2);
	static_assert(cust::lcm(2, 3)    ==  6);
	static_assert(cust::lcm(8, 2)    ==  8);
	static_assert(cust::lcm(6, 10)   == 30);
	static_assert(cust::lcm(-6, 10)  == 30);
	static_assert(cust::lcm(6, -10)  == 30);
	static_assert(cust::lcm(-6, -10) == 30);

	static_assert(cust::lcm(10u, 20, 30LL) ==  60);
	static_assert(cust::lcm(2, 3, 5, 7)    == 210);
	static_assert(cust::lcm(81, 27, 3, 9)  ==  81);

	std::cout << cust::lcm_oc(INT_MIN, -1)      << "\n\n";		//possible overflow
	std::cout << cust::lcm_oc(INT_MIN, INT_MAX) << "\n\n";		//possible overflow

	return 0;
}
```
