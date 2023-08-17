> *It's not reinventing the wheel. Just for practice and for fun.*<br>

### *Array types:*
* programming language built-in array
   * bounded or unbounded
   * static or dynamic
   * one-dimensional or multi-dimensional
* array-like data container
   * fixed size
      * std::array : static  memory allocation / on the stack
      * std::unique_ptr : dynamic memory allocation / on the free store
   * variable size
      * std::vector

![array](/Images/array.svg)

## Tricks and code snippets:

```C++
namespace cust					//customized / non-standard
{
	//implementation of fixed-size array with static memory allocation
	template<class T, size_t N>
	struct static_array_impl
	{
		// ......
    
		T elem[N == 0 ? 1 : N];
	};
  
	//implementation of fixed-size or variable-size array with dynamic memory allocation
	template<class T, bool FixedSize = true>
	class dynamic_array_impl
	{
		// ......
    
		protected:
			pointer elem[FixedSize ? 2 : 3] {};
	};
  
	//public interface / APIs of fixed-size array
	template<class T, class ImplTy>
	class array_interface : public ImplTy
	{
		public:
			constexpr size_type size() const noexcept;
			constexpr bool empty() const noexcept;
			constexpr pointer data() noexcept;
			constexpr reference front();
			constexpr reference back();
			constexpr reference at(size_type pos);
			constexpr reference operator [] (size_type pos);
			constexpr void fill(const_reference value);
			constexpr void swap(array_interface& other) noexcept;
			constexpr iterator begin() noexcept;
			constexpr iterator end() noexcept;
			// ......
	};
  
	 //fixed-size array with static memory allocation
	template<class T, size_t N>
	struct array : public array_interface<T, static_array_impl<T, N>>
	{
	};

	 //fixed-size array with dynamic memory allocation
	template<class T, class Allocator = allocator<T>>
	class dynamic_array : public array_interface<T, dynamic_array_impl<T>>
	{
		public:
			//constructors
			//destructor
			//assignment operators
      
		private:
			//allocator
			allocator_type alloc {};
	};
}
```

## Example:

```C++
#include <iostream>
#include "array.h"

template<class T, class U>
decltype(auto) operator << (std::ostream& os, const cust::array_interface<T, U>& ary)
{
	for (auto item : ary)
		os << item << "   ";
	os << "\n\n";

	return os;
}

int main(int argc, char* argv[])
{
	cust::array<int, 5> sa1 { 10 , 20 , 30 , 40 , 50 }, sa2 { 1 , 2 , 3 , 4 , 5 };

	cust::dynamic_array<double> da1 { 1.11 , 2.22 , 3.33 , 4.44 , 5.55 , 6.66 , 7.77 , 8.88 , 9.99 };

	std::cout << std::boolalpha;
	std::cout << sa1.empty() << "\t" << sa1.size() << "\t" << da1.empty() << "\t" << da1.size() << "\n";
	std::cout << sa1.front() << "\t" << sa1.back() << "\t" << da1.front() << "\t" << da1.back() << "\n\n";

	std::cout << "static  array 1 : " << sa1;
	std::cout << "static  array 2 : " << sa2;

	std::cout << "dynamic array 1 : ";
	for (auto iter { da1.cbegin() }; iter != da1.cend(); ++iter)
		std::cout << *iter << "   ";
	std::cout << "\n\n";

	sa1.swap(sa2);

	std::cout << "static  array 1 : " << sa1;
	std::cout << "static  array 2 : " << sa2;

	cust::dynamic_array<double> da2(std::move(da1));

	std::cout << "dynamic array 1 : " << da1;
	std::cout << "dynamic array 2 : " << da2;

	// ......

	return 0;
}
```
