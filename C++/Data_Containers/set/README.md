> *It's not reinventing the wheel. Just for practice and for fun.*<br>

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//AVL tree node
	template<class T>
	struct AVL_tree_node
	{
		// ......

		node_pointer    left   { nullptr };		//pointer pointing to left  child node
		node_pointer    right  { nullptr };		//pointer pointing to right child node
		node_pointer    parent { nullptr };		//pointer pointing to parent      node
		difference_type height { 0 };			//height of the node
		value_type      value  {};
	};

	//AVL tree data structure
	template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
	class AVL_tree
	{
		public:
			// ......

		private:
			// ......

			//implementation type
			struct impl_type : public Compare, allocator_type
			{
				node_pointer pseudo_head { nullptr };		//a pointer pointing to pseudo head node
				size_type    sz          { 0 };			//the number of elements of the tree
			};

			impl_type impl {};					//implementation of AVL tree
	};

	//iterator base
	template<class node_type>
	class iterator_base
	{
		public:
			// ......

		protected:
			node_pointer ptr { nullptr };
	};

	//container iterator
	template<class T, template<class...> class node_type>
	class container_iterator : public iterator_base<node_type<T>>
	{
		public:
			// ......

			reference operator *  () const noexcept;
			pointer   operator -> () const noexcept;
	};

	//balanced binary search tree (BBST) iterator
	template<class T, template<class...> class NodeTy>
	class bbst_iterator : public container_iterator<std::remove_const_t<T>, NodeTy>
	{
		// ......

		public:
			auto& operator ++ ()    noexcept;
			auto  operator ++ (int) noexcept;
			auto& operator -- ()    noexcept;
			auto  operator -- (int) noexcept;
	};

	//balanced binary search tree (BBST) container
	template<class KeyTy, class ValTy, class Compare = std::less<ValTy>, class Allocator = std::allocator<ValTy>>
	class bbst_container
	{
		public:
			//public member types
			// ......
			using iterator       = bbst_iterator<std::conditional_t<std::is_same_v<key_type, value_type>, std::add_const_t<value_type>, value_type>, AVL_tree_node>;
			using const_iterator = bbst_iterator<const value_type, AVL_tree_node>;

			//constructors
			bbst_container();
			bbst_container(const bbst_container& other);
			bbst_container(bbst_container&& other) noexcept;
			bbst_container(std::initializer_list<value_type> init);
			template<class InputIter>
			bbst_container(InputIter first, InputIter last);

			//destructor
			~bbst_container();

			//assignment operators
			bbst_container& operator = (const bbst_container& other);
			bbst_container& operator = (bbst_container&& other) noexcept;
			bbst_container& operator = (std::initializer_list<value_type> init);

			allocator_type get_allocator() const noexcept;			//returns the allocator associated with the container
			key_compare key_comp() const;					//returns the function object that compares the keys
			value_compare value_comp() const;				//returns the function object that compares objects of value_type
			bool empty() const noexcept;					//checks if the container is empty
			size_type size() const noexcept;				//gets the number of elements of the container
			void clear() noexcept;						//clears all elements of the container

			std::pair<iterator, bool> insert(const value_type& value);	//inserts an element into the container (with copy semantics)
			std::pair<iterator, bool> insert(value_type&& value);		//inserts an element into the container (with move semantics)
			void insert(std::initializer_list<value_type> init);		//inserts the elements of the initializer list into the container
			template<class InputIter>
			void insert(InputIter first, InputIter last);			//inserts a range of elements [first, last) into the container
			template<class... Args>
			std::pair<iterator, bool> emplace(Args&&... args);		//inserts an element into the container with its value constructed in-place
			iterator erase(const_iterator pos);				//removes specified elements from the container
			size_type erase(const key_type& key);

			void swap(bbst_container& other) noexcept;			//exchanges the contents of the container with those of other
			size_type count(const key_type& key) const noexcept;		//returns the number of elements with the same key
			iterator find(const key_type& key);				//finds the element with the same key
			const_iterator find(const key_type& key) const;
			bool contains(const key_type& key) const;			//checks if there is an element with the same key
			iterator lower_bound(const key_type& key);			//finds the first element with its key that is not less than the function argument
			const_iterator lower_bound(const key_type& key)	const;
			iterator upper_bound(const key_type& key);			//finds the first element with its key that is greater  than the function argument
			const_iterator upper_bound(const key_type& key)	const;

			iterator       begin()  noexcept;
			const_iterator begin()  const noexcept;
			const_iterator cbegin() const noexcept;
			iterator       end()    noexcept;
			const_iterator end()    const noexcept;
			const_iterator cend()   const noexcept;

			reverse_iterator       rbegin()  noexcept;
			const_reverse_iterator rbegin()  const noexcept;
			const_reverse_iterator crbegin() const noexcept;
			reverse_iterator       rend()    noexcept;
			const_reverse_iterator rend()    const noexcept;
			const_reverse_iterator crend()   const noexcept;

		protected:
			using impl_type = AVL_tree<value_type, value_compare, allocator_type>;

			impl_type bbst {};
	};

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator == (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator != (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator < (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator <= (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator > (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator >= (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs);

	//set data container
	template<class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
	class set : public bbst_container<Key, Key, Compare, Allocator>
	{
		// ......
	};
}
```

## Example:

```C++
#include <iostream>
#include "set.h"

template<class T>
decltype(auto) operator << (std::ostream& os, const cust::set<T>& st)
{
	for (const auto& elem : st)
		os << elem << "   ";

	os << "\t\tempty ? " << st.empty() << "\t\tsize : " << st.size() << "\n\n";

	return os;
}


int main(int argc, char* argv[])
{
	cust::set<int> set1 {};					//default constructor
	cust::set<int> set2 { 1 , 3 , 5 , 7 , 9 };		//constructor with initializer list
	cust::set<int> set3 { set2 };				//copy constructor

	std::cout << std::boolalpha;

	//insertions
	for (int i { 9 }; 0 < i; --i)
		set1.insert(i);

	//duplicate insertions
	for (int i { 1 }; i < 10; ++i)
		set1.insert(i);

	std::cout << "set 1 : " << set1;			// 1  2  3  4  5  6  7  8  9
	std::cout << "set 2 : " << set2;			// 1  3  5  7  9
	std::cout << "set 3 : " << set3;			// 1  3  5  7  9

	std::cout << "set 1 (reverse) : ";
	for (auto rit { set1.rbegin() }; rit != set1.rend(); ++rit)
		std::cout << *rit << "   ";			// 9  8  7  6  5  4  3  2  1
	std::cout << "\n\n";

	cust::set<int> set4 { std::move(set2) };		//move constructor

	std::cout << "set 2 : " << set2;			//
	std::cout << "set 4 : " << set4;			// 1  3  5  7  9

	std::cout << "set 2 == set 3 ? " << (set2 == set3) << "\n\n";	// false
	std::cout << "set 3 == set 4 ? " << (set3 == set4) << "\n\n";	// true

	//insertion with initializer list
	set2.insert({ 8 , 6 , 4 , 2 });

	std::cout << "set 2 : " << set2;			// 2  4  6  8

	//comparison of set 2 and set 4
	std::cout << "set 2 < set 4 ? " << (set2 < set4) << "\n\n";	// false
	std::cout << "set 2 > set 4 ? " << (set2 > set4) << "\n\n";	// true

	//swaps the contents of set 2 and set4
	set2.swap(set4);

	std::cout << "set 2 : " << set2;			// 1  3  5  7  9
	std::cout << "set 4 : " << set4;			// 2  4  6  8

	std::cout << "set 2 < set 4 ? " << (set2 < set4) << "\n\n";	// true
	std::cout << "set 2 > set 4 ? " << (set2 > set4) << "\n\n";	// false

	//erase some elements of set 1
	for (int i { 1 }; i < 10; ++i)
	{
		if ((i & 1) == 1)
			set1.erase(i);
	}

	std::cout << "set 1 : " << set1;			// 2  4  6  8

	for (int i { 1 }; i < 10; ++i)
	{
		if ((i & 1) == 1 && set1.find(i) != set1.end())
			std::cerr << "Failed to erase the element with key " << i << "\n\n";
	}

	//clears all elements of set 1
	set1.clear();

	std::cout << "set 1 : " << set1;			//

	//copy assignment
	set1 = set4;

	std::cout << "set 1 : " << set1;			// 2  4  6  8

	//range insertions
	set1.insert(set2.begin(), set2.end());

	std::cout << "set 1 : " << set1;			// 1  2  3  4  5  6  7  8  9

	//finds lower bound
	for (int i { 0 }; i < 11; ++i)
	{
		auto iter { set1.lower_bound(i) };

		std::cout << "lower bound of " << i;

		if (iter == set1.end())
			std::cout << " not exists";
		else
			std::cout << " is " << *iter;

		std::cout << "\n";
	}
	std::cout << "\n";

	//finds upper bound
	for (int i { 0 }; i < 10; ++i)
	{
		auto iter { set1.upper_bound(i) };

		std::cout << "upper bound of " << i;

		if (iter == set1.end())
			std::cout << " not exists";
		else
			std::cout << " is " << *iter;

		std::cout << "\n";
	}

	return 0;
}
```
