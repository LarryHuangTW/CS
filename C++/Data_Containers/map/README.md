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

	//a function object to compare key-value pairs
	template<class key_type, class value_type, class Compare = std::less<key_type>>
	struct ComPair : public Compare
	{
		using base_type = Compare;
		using pair_type = std::pair<const key_type, value_type>;

		constexpr bool operator () (const pair_type& lhs, const pair_type& rhs) const
		{
			return base_type::operator()(lhs.first, rhs.first);
		}

		constexpr bool operator () (const pair_type& lhs, const key_type& key) const
		{
			return base_type::operator()(lhs.first, key);
		}

		constexpr bool operator () (const key_type& key, const pair_type& rhs) const
		{
			return base_type::operator()(key, rhs.first);
		}
	};

	//map data container
	template<class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
	class map : public bbst_container<Key, std::pair<const Key, T>, ComPair<Key, T, Compare>, Allocator>
	{
		public:
			// ......

			key_compare key_comp() const;				//returns the function object that compares keys
			mapped_type& at(const key_type& key);			//access a specified element with bounds checking
			const mapped_type& at(const key_type& key) const;
			mapped_type& operator [] (const key_type& key);		//access or inserts a specified element
			mapped_type& operator [] (key_type&& key);

			//inserts in-place if the key does not exist, does nothing if the key exists
			template<class... Args>
			std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args);
			template<class... Args>
			std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args);
	};
}
```

## Example:

```C++
#include <iostream>
#include <string>
#include "map.h"

template<class T, class U>
decltype(auto) operator << (std::ostream& os, const std::pair<T, U>& pr)
{
	os << pr.first << ":\t" << pr.second;

	return os;
}

template<class Key, class T>
decltype(auto) operator << (std::ostream& os, const cust::map<Key, T>& mp)
{
	//std::cout << "{ ";
	for (const auto& pr : mp)
		os << pr << "\n";
	os << "\n";
	//os << "\b\b }\n\n";

	os << "empty ? " << mp.empty() << "\t\tsize : " << mp.size() << "\n\n";

	return os;
}


int main(int argc, char* argv[])
{
	cust::map<std::string, int> mp   {};		//default constructor
	cust::map<std::string, int> mp1  { {"Xman", 190}, {"Yoda", 80}, {"Zelda", 163} };	//constructor with initializer list
	std::pair<std::string, int> alan { std::pair("Alan", 185) };
	int  rc   { 0 };				//return code

	std::cout << std::boolalpha;

	//inserts new elements (key-value pairs)
	auto pr = mp.insert({ "Bob" , 165 });

	mp.insert(*pr.first);				//duplicate insertion and no effect

	mp.insert(std::make_pair("Ellen", 176));
	mp.insert(std::move(alan));

	std::cout << alan << "\n\n";			// : 185

	mp["Frank"] = 188;
	mp.emplace(std::string("David"), 170);
	mp.emplace("Clark", 180);

	mp.insert({ "David" , 200 });			//duplicate insertion and no effect
	mp.insert({ "Ellen" , 200 });			//duplicate insertion and no effect

	mp.try_emplace("Gary", 178);
	mp["Howard"];					//inserts the pair {"Howard", 0}

	std::cout << mp;

	mp["Howard"] = 180;				//updates the value of an existing element

	//iterates the map data container and updates values of all elements
	for (auto iter { mp.begin() }; iter != mp.end(); ++iter)
		iter->second += 10;

	std::cout << mp;

	//erases some elements of the map data container
	mp.erase("Adam");
	mp.erase(mp.begin());
	mp.erase("Bob");

	std::cout << mp;

	//finds lower bound and upper bound
	std::cout << "lower bound of Amy  is " << mp.lower_bound("Amy")->first  << "\n\n";
	std::cout << "upper bound of Gary is " << mp.upper_bound("Gary")->first << "\n\n";

	for (const auto& pr : mp1)
	{
		if (mp.find(pr.first) != mp.end())
			std::cout << "Found " << pr.first << "\n\n";
	}

	//swaps the contents of two map data containers
	mp.swap(mp1);

	std::cout << mp;

	//move assignment
	mp = std::move(mp1);

	std::cout << mp;

	if ( !mp1.empty() )
		std::cout << "mp1 is not empty!\n\n";

	try
	{
		std::cout << mp.at("Zelda") << "\n\n";		// Exception : invalid element access: no such key
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception : " << e.what() << "\n\n";

		rc = -1;
	}

	return rc;
}
```
