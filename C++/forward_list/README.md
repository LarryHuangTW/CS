> *It's not reinventing the wheel. Just for practice and for fun.*<br>

![forward list](/Images/forward_list/forward_list.svg)
![forward list possible implementations](/Images/forward_list/forward_list_possible_implementations.svg)
<br>
![forward list operations](/Images/forward_list/forward_list_operations.svg)

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//node data strucure of forward_list data container
	template<class T>
	struct forward_list_node
	{
		// ......

		node_pointer next  { nullptr };
		value_type   value {};
	};

	//iterator base
	template<class NodePtr>
	class iterator_base
	{
		public:
			// ......

		protected:
			node_pointer ptr { nullptr };
	};

	//container iterator
	template<class T, template<class...> class NodeTy, class NodePtr = NodeTy<std::remove_const_t<T>>*>
	class container_iterator : public iterator_base<NodePtr>
	{
		public:
			// ......

			reference operator *  () const noexcept;
			pointer   operator -> () const noexcept;
	};

	//unidirectional iterator associated with forward_list data container
	template<class T, template<class...> class NodeTy>
	class forward_list_iterator : public container_iterator<T, NodeTy>
	{
		// ......

		public:
			forward_list_iterator& operator ++ ()    noexcept;
			forward_list_iterator  operator ++ (int) noexcept;
	};

	//forward_list data container
	template<class T, class Allocator = std::allocator<T>>
	class forward_list
	{
		public:
			//public member types
			// ......
			using iterator       = forward_list_iterator<value_type,       forward_list_node>;
			using const_iterator = forward_list_iterator<const value_type, forward_list_node>;

			//constructors
			forward_list() noexcept;
			explicit forward_list(size_type count);
			forward_list(size_type count, const value_type& value);
			forward_list(const forward_list& other);
			forward_list(forward_list&& other) noexcept;
			forward_list(std::initializer_list<value_type> initList);

			//destructor
			~forward_list();

			//assignment operators
			forward_list& operator = (const forward_list& other);
			forward_list& operator = (forward_list&& other);
			forward_list& operator = (std::initializer_list<value_type> initList);

			bool empty() const noexcept;						//checks if the container is empty
			reference front();							//access the first element of the container
			const_reference front() const;

			void clear() noexcept;							//clear all elements in the container
			void push_front(const_reference value);					//add an element to the beginning of the container
			void push_front(T&& value);
			void pop_front();							//remove the first element of the container

			template<class... Args>
			reference emplace_front(Args&&... args);				//add an element to the beginning of the container

			iterator insert_after(const_iterator pos, const_reference value);	//insert an element after the specified position
			iterator insert_after(const_iterator pos, T&& value);
			iterator erase_after(const_iterator pos);				//remove an element at the specified position
			iterator erase_after(const_iterator first, const_iterator last);	//remove element(s) in the specified range

			void reverse() noexcept;						//reverse the order of the elements in the forward_list
			void merge(forward_list& other);					//merge two sorted forward_list
			void merge(forward_list&& other);

			template<class Compare = std::less<>>
			void sort(Compare cmp = Compare{});					//sort the elements in the forward_list

			iterator       begin()  noexcept;
			const_iterator begin()  const noexcept;
			iterator       end()    noexcept;
			const_iterator end()    const noexcept;

			const_iterator cbegin() const noexcept;
			const_iterator cend()   const noexcept;

			// ......

		private:
			// ......
			node_pointer   head  { nullptr };
			allocator_type alloc {};
	};

	template<class T, class Allocator>
	bool operator == (const forward_list<T, Allocator>& lhs, const forward_list<T, Allocator>& rhs);

	template<class T, class Allocator>
	bool operator != (const forward_list<T, Allocator>& lhs, const forward_list<T, Allocator>& rhs);
}
```

## Example:

```C++
#include <iostream>
#include <string>
#include "forward_list.h"

template<class T>
decltype(auto) operator << (std::ostream& os, const cust::forward_list<T>& lst)
{
	for (const auto& item : lst)
		os << item << "   ";

	os << "\n\n";

	return os;
}

int main(int argc, char* argv[])
{
	cust::forward_list<int> lst1 {}, lst2(3), lst3(3, 7);
	cust::forward_list<int> lst4 { 2 , 4 , 6 , 8 , 10  };
	cust::forward_list<std::string> lst5 {};

	std::string str1 { "Hi" }, str2 { "Hello" }, str3 { "World" };

	std::cout << std::boolalpha;

	std::cout << "list 1 : " << lst1;
	std::cout << "list 2 : " << lst2;
	std::cout << "list 3 : " << lst3;
	std::cout << "list 4 : " << lst4;
	std::cout << "list 5 : " << lst5;

	lst1.push_front(1);
	lst1.push_front(3);
	lst1.push_front(5);
	lst1.push_front(7);
	lst1.push_front(9);

	std::cout << "[ list 1 ]\n\n";
	std::cout << "front  : " << lst1.front() << "\n\n";
	std::cout << "list 1 : " << lst1;

	std::cout << "[ reverse list 1 ]\n\n";

	lst1.reverse();

	std::cout << "list 1 : " << lst1;

	std::cout << "Are the values of list 2 and 3 are equal? " << (lst2 == lst3) << "\n\n";

	std::cout << "after copy assignment\n\n";

	lst2 = lst3;

	std::cout << "Are the values of list 2 and 3 are equal? " << (lst2 == lst3) << "\n\n";

	std::cout << "list 2 : " << lst2;

	lst3 = { 7 , 77 , 777 };

	std::cout << "list 3 : " << lst3;

	lst5.emplace_front(std::move(str3));
	lst5.emplace_front(std::move(str2));
	lst5.emplace_front(std::move(str1));

	std::cout << "list 5   : " << lst5;
	std::cout << "string 1 : " << str1 << "\n";
	std::cout << "string 2 : " << str2 << "\n";
	std::cout << "string 3 : " << str3 << "\n\n";

	cust::forward_list<std::string> lst6 { std::move(lst5) };

	std::cout << "list 5 : " << lst5;
	std::cout << "list 6 : " << lst6;

	auto iter { std::find(lst4.begin(), lst4.end(), 6) };

	if (iter == lst4.end())
		std::cout << "No element with data " << 6 << " in the list 4\n\n";

	iter = std::find(lst4.begin(), lst4.end(), 100);

	if (iter == lst4.end())
		std::cout << "No element with data " << 100 << " in the list 4\n\n";

	std::cout << "[merge list 1 and 4 ]\n\n";

	lst1.merge(lst4);

	std::cout << "list 1 : " << lst1;
	std::cout << "list 4 : " << lst4;

	// ......

	return 0;
}

```
