> *It's not reinventing the wheel. Just for practice and for fun.*<br>

![list](/Images/list/list.svg)
<br>
![list node diff](/Images/list/list_node_diff.svg)
<br>
![list operations](/Images/list/list_operations.svg)

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//node data structure of list data container
	template<class T>
	struct list_node
	{
		// ......

		node_pointer next  { nullptr };			//pointer pointing to next     node
		node_pointer prev  { nullptr };			//pointer pointing to previous node
		value_type   value {};				//data value
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

	//bidirectional iterator associated with list data container
	template<class T, template<class...> class NodeTy>
	class list_iterator : public container_iterator<T, NodeTy>
	{
		// ......

		public:
			list_iterator& operator ++ ()    noexcept;
			list_iterator  operator ++ (int) noexcept;
			list_iterator& operator -- ()    noexcept;
			list_iterator  operator -- (int) noexcept;
	};

	//list data container
	template<class T, class Allocator = std::allocator<T>>
	class list
	{
		public:
			//public member types
			// ......
			using iterator       = list_iterator<value_type,       list_node>;
			using const_iterator = list_iterator<const value_type, list_node>;

			//constructors
			list();
			explicit list(size_type count);
			list(size_type count, const value_type& value);
			list(const list& other);
			list(list&& other) noexcept;
			list(std::initializer_list<value_type> initList);

			//destructor
			~list();

			//assignment operators
			list& operator = (const list& other);
			list& operator = (list&& other) noexcept;
			list& operator = (std::initializer_list<value_type> initList);

			size_type size() const noexcept;				//returns number of elements in the container
			bool empty() const noexcept;					//checks if the container is empty
			reference front();						//access the first element of the container
			const_reference front() const;
			reference back();						//access the last element of the container
			const_reference back() const;

			void clear() noexcept;						//clears all elements in the container
			void push_front(const value_type& value);			//adds an element (with copy semantics) to the beginning of the container
			void push_front(value_type&& value);				//adds an element (with move semantics) to the beginning of the container
			void push_back(const value_type& value);			//appends an element (with copy semantics) to the end of the container
			void push_back(value_type&& value);				//appends an element (with move semantics) to the end of the container
			void pop_front();						//removes the first element of the container
			void pop_back();						//removes the last  element of the container
			iterator erase(const_iterator pos);				//erases an element in the specified position
			iterator erase(const_iterator first, const_iterator last);	//earses elements in the specified range [first , last)

			template<class... Args>
			reference emplace_front(Args&&... args);			//constructs an element in-place to the beginning of the container

			template<class... Args>
			reference emplace_back(Args&&... args);				//constructs an element in-place to the end of the container

			void swap(list& other) noexcept;				//swaps all elements with other list
			void reverse() noexcept;					//reverses the order of the elements in the list
			void merge(list& other);					//merges two sorted lists
			void merge(list&& other);

			template<class Compare = std::less<>>
			void sort(Compare cmp = Compare{});				//sorts the elements of the list

			iterator       begin()  noexcept;
			const_iterator begin()  const noexcept;
			const_iterator cbegin() const noexcept;
			iterator       end()    noexcept;
			const_iterator end()    const noexcept;
			const_iterator cend()   const noexcept;

			// ......

		private:
			// ......

			node_pointer        pseudo_head { nullptr };
			size_type           sz          { 0 };
			node_allocator_type alloc       {};
	};

	template<class T, class Allocator>
	bool operator == (const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);

	template<class T, class Allocator>
	bool operator != (const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);
}
```

## Example:

```C++
#include <iostream>
#include <string>
#include "list.h"

template<class T>
decltype(auto) operator << (std::ostream& os, const cust::list<T>& lst)
{
	for (const auto& item : lst)
		os << item << "   ";

	os << "\t\tempty ? " << lst.empty() << "\t\tsize : " << lst.size() << "\n\n";

	return os;
}

int main(int argc, char* argv[])
{
	cust::list<int> lst1 {}, lst2(3), lst3(3, 7);
	cust::list<int> lst4 { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10  };
	cust::list<std::string> lst5 {};

	std::string str1 { "Hi" }, str2 { "Hello" }, str3 { "World" };

	std::cout << std::boolalpha;

	std::cout << "list 1 : " << lst1;			//
	std::cout << "list 2 : " << lst2;			// 0   0   0
	std::cout << "list 3 : " << lst3;			// 7   7   7
	std::cout << "list 4 : " << lst4;			// 1   2   3   4   5   6   7   8   9   10
	std::cout << "list 5 : " << lst5;			//

	lst1.push_front(20);
	lst1.push_back(30);
	lst1.push_front(10);
	lst1.push_back(40);
	lst1.push_back(50);

	std::cout << "[ list 1 ]\n\n";
	std::cout << "front  : " << lst1.front() << "\n\n";	// 10
	std::cout << "back   : " << lst1.back()  << "\n\n";	// 50
	std::cout << "list 1 : " << lst1;			// 10   20   30   40   50

	std::cout << "Are the values of list 2 and 3 are equal? " << (lst2 == lst3) << "\n\n";	// false

	std::cout << "after copy assignment\n\n";

	lst2 = lst3;

	std::cout << "Are the values of list 2 and 3 are equal? " << (lst2 == lst3) << "\n\n";	// true

	std::cout << "list 2 : " << lst2;			// 7   7   7

	lst3 = { 3 , 9 , 1 , 6 , 4 , 2 , 5 , 8 , 7 };

	std::cout << "list 3 : " << lst3;			// 3   9   1   6   4   2   5   8   7

	std::cout << "[ sort list 3 ]\n\n";

	lst3.sort();

	std::cout << "list 3 : " << lst3;			// 1   2   3   4   5   6   7   8   9

	lst5.emplace_front(std::move(str2));
	lst5.emplace_back(std::move(str3));
	lst5.emplace_front(std::move(str1));

	std::cout << "list 5   : " << lst5;			// Hi   Hello   World
	std::cout << "string 1 : " << str1 << "\n";		//
	std::cout << "string 2 : " << str2 << "\n";		//
	std::cout << "string 3 : " << str3 << "\n\n";		//

	cust::list<std::string> lst6 { std::move(lst5) };

	std::cout << "list 5 : " << lst5;			//
	std::cout << "list 6 : " << lst6;			// Hi   Hello   World

	auto val  { 9 };
	auto iter { std::find(lst4.begin(), lst4.end(), val) };

	if (iter == lst4.end())
		std::cout << "No element with data " << val << " in the list 4\n\n";

	iter = std::find(lst4.begin(), lst4.end(), 100);

	if (iter == lst4.end())
		std::cout << "No element with data " << 100 << " in the list 4\n\n";

	std::cout << "[ reverse list 4 ]\n\n";

	lst4.reverse();

	std::cout << "list 4 : " << lst4;			// 10   9   8   7   6   5   4   3   2   1

	for (auto it { lst4.begin() }; it != lst4.end(); )
	{
		if ((*it & 1) == 0)
			it = lst4.erase(it);
		else
			++it;
	}

	lst4.reverse();

	std::cout << "list 4 : " << lst4;			// 1   3   5   7   9

	lst1.pop_front();
	lst1.pop_back();
	lst1.erase(std::next(lst1.begin()));

	for (auto& item : lst1)
		item /= 10;

	lst1.push_back(6);
	lst1.push_back(8);
	lst1.push_back(10);

	std::cout << "[merge list 1 and 4 ]\n\n";

	lst1.merge(lst4);

	std::cout << "list 1 : " << lst1;			// 1   2   3   4   5   6   7   8   9   10
	std::cout << "list 4 : " << lst4;			//

	// ......

	return 0;
}
```
