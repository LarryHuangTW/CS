![summation](/../../../../Images/summation.svg)

### 1. recursive version:

```C++
template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
U sum_1toN(T n)
{
	return n < 1 ? 0 : n + sum_1toN(n - 1);
}
```

<br>

### 2. constexpr recursive version:

```C++
template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	return n < 1 ? 0 : n + sum_1toN(n - 1);
}
```

<br>

### 3. non-recursive version:

```C++
template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	U result {};

	for (T i { 1 }; i <= n; ++i)
		result += static_cast<U>(i);

	return result;
}
```

<br>

### 4. mathematical formula version (with overflow checking):

```C++
//to check if the integer is odd or not
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr bool isOdd(T n)
{
	return (n & 1) == 1;
}

template<class T, class U = T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr U sum_1toN(T n)
{
	bool flag { isOdd(n) };			//to indicate the number n is odd or not
	U    num  { (n + flag) / 2 };

	if (n < 0)
		return U{};

	++n -= flag;

	//to check the possibility of overflow
	if (num != 0 && std::numeric_limits<U>::max() / num < n)
		std::cerr << "possible overflow\n";

	return num * n;
}
```
