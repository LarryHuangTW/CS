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
			constexpr size_type size() const noexcept;		//returns the number of elements in the array
			constexpr bool empty() const noexcept;			//checks if the array is empty
			constexpr pointer data() noexcept;			//direct access to the underlying array
			constexpr reference front();				//access the first element in the array
			constexpr reference back();				//access the last  element in the array
			constexpr reference at(size_type pos);			//access specified element in the array with bounds checking
			constexpr reference operator [] (size_type pos);	//access specified element in the array
			constexpr void fill(const_reference value);		//fills the array with specified value
			constexpr void swap(array_interface& other) noexcept;	//swaps all elements with other array

			constexpr iterator begin() noexcept;			//returns an iterator to the beginning of the array
			constexpr const_iterator begin()  const noexcept;
			constexpr const_iterator cbegin() const noexcept;
			constexpr iterator end() noexcept;			//returns an iterator to the end of the array
			constexpr const_iterator end()  const noexcept;
			constexpr const_iterator cend() const noexcept;

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

			constexpr allocator_type get_allocator() const noexcept;	//gets allocator of the container
			constexpr size_type max_size() const noexcept;			//returns the maximum number of elements the container can hold
			constexpr void reserve(size_type new_cap);			//reserves (memory) capacity for the container
			constexpr void clear() noexcept;				//clears all elements

			template<class... Args>
			constexpr reference emplace_back(Args&&... args);		//adds an element (in-place) to the end of the container

			constexpr void push_back(const_reference value);		//adds an element (with copy semantics) to the end of the container
			constexpr void push_back(value_type&& value);			//adds an element (with move semantics) to the end of the container
			constexpr void pop_back();					//removes the last element of the container

			template<class... Args>
			constexpr iterator emplace(const_iterator pos, Args&&... args);		//inserts an element (in-place) into the container before pos

			constexpr iterator insert(const_iterator pos, const_reference value);	//inserts an element (with copy semantics) into the container before pos
			constexpr iterator insert(const_iterator pos, value_type&& value);	//inserts an element (with move semantics) into the container before pos
			constexpr iterator erase(const_iterator pos);				//erases the element at pos of the container
			constexpr iterator erase(const_iterator first, const_iterator last);	//erases the elements in the range [first, last) of the container

		private:
			// ......

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
	std::cout << "\tis empty ? " << vec.empty() << "\t\tsize : " << vec.size() << "\t\tcapacity : " << vec.capacity() << "\n\n";
}

int main(int argc, char* argv[])
{
	cust::vector<int> vec1 {}, vec2 { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 };
	cust::vector<std::string> vec3 {};
	std::string str1 { "Hello" }, str2 { "World" };

	std::cout << std::boolalpha;

	std::cout << "vector 1 : " << vec1;		//
	std::cout << "vector 2 : " << vec2;		// 1   2   3   4   5   6   7   8   9   10
	std::cout << "vector 3 : " << vec3;		//

	std::cout << "[ vector 1 ]\n";
	print_info(vec1);				// is empty ? true	size : 0	capacity : 0

	std::cout << "[ vector 2 ]\n";
	print_info(vec2);				// is empty ? false	size : 10	capacity : 10

	std::cout << "[ vector 1 ]\n";
	for (int i { 1 }; i < 11; ++i)
	{
		std::cout << "push back " << i << "\t\t";
		vec1.push_back(i);
		print_info(vec1);
	}
	std::cout << "\n";

	std::cout << "Is vector 1 equal to vector 2 ? " << (vec1 == vec2) << "\n\n";	// true

	std::cout << "[ vector 2 ]\n";
	for (int i { 0 }; i < 10; ++i)
	{
		std::cout << "pop back\t\t";
		vec2.pop_back();
		print_info(vec2);
	}
	std::cout << "\n";

	std::cout << "Is vector 1 equal to vector 2 ? " << (vec1 == vec2) << "\n\n";	// false

	std::cout << "[ vector 3 ]\n";
	print_info(vec3);				// is empty ? true	size : 0	capacity : 0

	std::cout << "string 1 : " << str1 << "\n";	// Hello
	std::cout << "string 2 : " << str2 << "\n\n";	// World

	vec3.push_back(std::move(str1));
	vec3.push_back(std::move(str2));

	print_info(vec3);				// is empty ? false	size : 2	capacity : 2

	std::cout << "string 1 : " << str1 << "\n";	//
	std::cout << "string 2 : " << str2 << "\n\n";	//

	std::cout << "vector 1 : " << vec1;		// 1   2   3   4   5   6   7   8   9   10
	std::cout << "vector 2 : " << vec2;		//
	std::cout << "vector 3 : " << vec3;		// Hello   World

	// ......

	return 0;
}
```
