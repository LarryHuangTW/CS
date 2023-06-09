> *It's not reinventing the wheel. Just for practice and for fun.*<br>

![vector](/Images/vector.svg)

## Tricks and code snippets:

```C++
namespace cust					//customized / non-standard
{
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
			// ......
	};

	//variable-size array with dynamic memory allocation
	template<class T, class Allocator = allocator<T>>
	class vector : public array_interface<T, dynamic_array_impl<T, false>>
	{
		public:
			//constructors
			//destructor
			//assignment operators

			constexpr allocator_type get_allocator() const noexcept;
			constexpr size_type max_size() const noexcept;
			constexpr void reserve(size_type new_cap);
			constexpr void clear() noexcept;

			template<class... Args>
			constexpr reference emplace_back(Args&&... args);

			constexpr void push_back(const_reference value);
			constexpr void push_back(value_type&& value);
			constexpr void pop_back();
			// ......

		private:
			//allocator of the vector
			allocator_type alloc {};
	};
}
```

## Example:

```C++
#include <iostream>
#include <string>
#include "vector.h"

template<class T, class U>
decltype(auto) operator << (std::ostream& os, const cust::array_interface<T, U>& ary)
{
	for (auto item : ary)
		os << item << "   ";
	os << "\n\n";

	return os;
}

template<class T>
void print_info(const cust::vector<T>& vec)
{
	std::cout << "\tis empty ? " << vec.empty() << "\t\tsize : " << vec.size() << "\t\tcapacity : " << vec.capacity() << "\n";
}

int main(int argc, char* argu[])
{
	cust::vector<int> vec1 {}, vec2 { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 };
	cust::vector<std::string> vec3 {};
	std::string str1 { "Hello" }, str2 { "World" };

	std::cout << std::boolalpha;

	std::cout << "vector 1 : " << vec1;
	std::cout << "vector 2 : " << vec2;
	std::cout << "vector 3 : " << vec3;

	std::cout << "[ vector 1 ]\n";
	print_info(vec1);
	std::cout << "\n";

	std::cout << "[ vector 2 ]\n";
	print_info(vec2);
	std::cout << "\n";

	std::cout << "[ vector 1 ]\n";
	for (int i { 1 }; i < 11; ++i)
	{
		std::cout << "push back " << i << "\t\t";
		vec1.push_back(i);
		print_info(vec1);
	}
	std::cout << "\n";

	std::cout << "Is vector 1 equal to vector 2 ? " << (vec1 == vec2) << "\n\n";

	std::cout << "[ vector 2 ]\n";
	for (int i { 0 }; i < 10; ++i)
	{
		std::cout << "pop back\t\t";
		vec2.pop_back();
		print_info(vec2);
	}
	std::cout << "\n";

	std::cout << "Is vector 1 equal to vector 2 ? " << (vec1 == vec2) << "\n\n";

	std::cout << "[ vector 3 ]\n";
	print_info(vec3);
	std::cout << "\n";

	std::cout << "string 1 : " << str1 << "\n";
	std::cout << "string 2 : " << str2 << "\n\n";

	vec3.push_back(std::move(str1));
	vec3.push_back(std::move(str2));

	print_info(vec3);
	std::cout << "\n";

	std::cout << "string 1 : " << str1 << "\n";
	std::cout << "string 2 : " << str2 << "\n\n";

	std::cout << "vector 1 : " << vec1;
	std::cout << "vector 2 : " << vec2;
	std::cout << "vector 3 : " << vec3;

	// ......

	return 0;
}
```
