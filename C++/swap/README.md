> *It's not reinventing the wheel. Just for practice and for fun.*

### 1. call by address version:

```C++
constexpr void swap(int* ptrL, int* ptrR)
{
	auto tmp { *ptrL };
	*ptrL = *ptrR;
	*ptrR = tmp;
}
```

It works but what if we want to swap values of other built-in or user-defined data types? 

Some people view raw pointers frightening and out of date.<br><br>

### 2. call by reference version:

```C++
constexpr void swap(int& lhs, int& rhs)
{
	auto tmp { lhs };
	lhs = rhs;
	rhs = tmp;
}
```

It works and does not use raw pointers. But how about swapping values of other data types?<br><br>

### 3. C generic version and call by address:

```C++
constexpr void swap(void* ptrL, void* ptrR, size_t sz)
{
	auto ptr { new byte[sz] };

	if (ptr != nullptr)
	{
		std::memcpy(ptr,  ptrL, sz);
		std::memcpy(ptrL, ptrR, sz);
		std::memcpy(ptrR, ptr,  sz);

		delete[] ptr;
	}
}
```

It works ok for different data types. But raw pointers are still scary.

```C++
class item
{
	public:
		// ......
	
	private:
		const int id;
		double    val;
};
```

What if we want to swap two objects of *item* type?<br><br>

### 4. C++ function template + call by reference + copy semantics:

```C++
template<class T>
constexpr void swap(T& lhs, T& rhs) noexcept
{
	T tmp { lhs };
	lhs = rhs;
	rhs = tmp;
}
```

What would happen if we want to swap two strings, two vectors, or two lists?<br><br>

### 5. C++ function template + call by reference + move semantics:

```C++
template<class T>
constexpr void swap(T& lhs, T& rhs) noexcept
{
	T tmp { cust::move(lhs) };
	lhs   = cust::move(rhs);
	rhs   = cust::move(tmp);
}

//C++ iterator swap
template<class FwdIt1, class FwdIt2>
constexpr void iter_swap(FwdIt1 iterL, FwdIt2 iterR)
{
	cust::swap(*iterL, *iterR);
}
```

It works fine for different data types and resource handles.<br><br>

### Observations:

* First, in this case, assembly codes of call by address and call by reference versions are ${\color{red}the}$ ${\color{red}same}$.<br><br>

* Second, let's compare the assembly codes of debug and release versions of call by address.<br>
  Does local variable *tmp* occupy any memory space?<br><br>

| debug version |
| ------------- |
```Assembly
9 : int tmp = *ptrL

		mov	 rax, QWORD PTR ptrL$[rbp]
		mov	 eax, DWORD PTR [rax]
		mov	 DWORD PTR tmp$[rbp], eax

10 : *ptrL = *ptrR

		mov	 rax, QWORD PTR ptrL$[rbp]
		mov	 rcx, QWORD PTR ptrR$[rbp]
		mov	 ecx, DWORD PTR [rcx]
		mov	 DWORD PTR [rax], ecx

11 : *ptrR = tmp

		mov	 rax, QWORD PTR ptrR$[rbp]
		mov	 ecx, DWORD PTR tmp$[rbp]
		mov	 DWORD PTR [rax], ecx
```

| release version |
| --------------- |
```Assembly
9 : int tmp = *ptrL

		mov	 r8d, DWORD PTR [rcx]

10 : *ptrL = *ptrR

		mov	 eax, DWORD PTR [rdx]
		mov	 DWORD PTR [rcx], eax

11 : *ptrR = tmp

		mov	 DWORD PTR [rdx], r8d
```
