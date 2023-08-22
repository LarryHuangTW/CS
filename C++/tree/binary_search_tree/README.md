![binary search tree](/Images/tree/binary_search_tree.svg)

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//binary search tree node
	template<class T>
	struct binary_search_tree_node
	{
		// ......

		node_pointer left   { nullptr };	//pointer pointing to left  child node
		node_pointer right  { nullptr };	//pointer pointing to right child node
		node_pointer parent { nullptr };	//pointer pointing to parent      node
		value_type   value  {};			//data value
	};

	//binary tree data structure
	template<class T, class node_type = binary_tree_node<T>, class Allocator = std::allocator<node_type>>
	class binary_tree
	{
		public:
			// ......

		protected:
			// ......

			node_pointer   root  { nullptr };		//pointer points to root node
			allocator_type alloc {};			//allocator object
	};

	//binary search tree data structure
	template<class T, class node_type = binary_search_tree_node<T>, class Compare = std::less<T>, class Allocator = std::allocator<node_type>>
	class binary_search_tree : public binary_tree<T, node_type, Allocator>
	{
		public:
			//member types

			//constructors
			binary_search_tree() noexcept;
			binary_search_tree(const binary_search_tree& other);
			binary_search_tree(binary_search_tree&& other) noexcept;
			binary_search_tree(std::initializer_list<value_type> init);
			template<class InputIter>
			binary_search_tree(InputIter first, InputIter last);

			//destructor
			~binary_search_tree();

			//assignment operators
			binary_search_tree& operator = (const binary_search_tree& other);
			binary_search_tree& operator = (binary_search_tree&& other) noexcept;
			binary_search_tree& operator = (std::initializer_list<value_type> init);

			size_type size() const noexcept;		//gets the number of elements of the tree
			bool empty() const noexcept;			//checks if the tree is empty
			difference_type height() const noexcept;	//gets the height of the tree, -1 if the tree is empty
			node_pointer max() const noexcept;		//gets the pointer pointing to the node with the max value of the tree
			node_pointer min() const noexcept;		//gets the pointer pointing to the node with the min value of the tree
			void clear() noexcept;				//clears all elements of the tree

			std::pair<node_pointer, bool> insert(const value_type& value);	//inserts an element into the tree (with copy semantics)
			std::pair<node_pointer, bool> insert(value_type&& value);	//inserts an element into the tree (with move semantics)
			void insert(std::initializer_list<value_type> init);		//inserts the elements of the initializer list into the tree
			template<class InputIter>
			void insert(InputIter first, InputIter last);			//inserts a range of elements [first, last) into the tree

			template<class... Args>
			std::pair<node_pointer, bool> emplace(Args&&... args);		//inserts an element into the tree with its value constructed in-place

			node_pointer erase(node_pointer pos);				//removes the element at pos from the tree
			node_pointer erase(const value_type& value);			//removes the element with the equivalent value from the tree

			void swap(binary_search_tree& other) noexcept;			//exchanges the elements of the tree with those of other

			node_pointer find(const value_type& value) const;		//finds the element with the equivalent value of the tree
			bool contains(const value_type& value) const;			//checks if there is an element with the equivalent value of the tree
			node_pointer lower_bound(const value_type& value) const;	//finds the first element with the value that is not less than the function argument
			node_pointer upper_bound(const value_type& value) const;	//finds the first element with the value that is greater than the function argument

			template<class Fn>
			void traverse(Fn func) const;			//performs an inorder traversal of the tree

		protected:
			// ......

			size_type sz  { 0 };		//the number of elements / nodes of the tree
			Compare   cmp {};		//element comparison function
	};
}
```

## Example:

```C++
#include <iostream>
#include <vector>
#include <initializer_list>
#include <random>
#include "binary_search_tree.h"

template<class T>
using tree_type = cust::binary_search_tree<T>;

const std::initializer_list<std::initializer_list<int>> test_cases {
			{  9 , 7 , 5 ,  3 ,  1 } ,
			{  2 , 4 , 6 ,  8 , 10 } ,
			{  1 , 9 , 7 ,  5 ,  3 } ,
			{ 10 , 2 , 4 ,  6 ,  8 } ,
			{  1 , 9 , 3 ,  7 ,  5 } ,
			{ 10 , 2 , 8 ,  4 ,  6 } ,
			{  6 , 3 , 8 ,  1 ,  5 , 7 , 10 , 2 , 4 ,  9 } ,
			{  5 , 4 , 3 ,  1 ,  2 , 6 ,  7 , 9 , 8 , 11 , 10 , 12 } ,
			{  5 , 6 , 3 ,  1 } ,
			{  5 , 6 , 3 ,  4 } ,
			{  6 , 5 , 8 , 10 } ,
			{  6 , 5 , 8 ,  7 }
};

template<class T>
decltype(auto) operator << (std::ostream& os, const std::vector<T>& vec)
{
	for (auto item : vec)
		os << item << "   ";
	os << "\n\n";

	return os;
}

template<class T>
decltype(auto) operator << (std::ostream& os, const tree_type<T>& tree)
{
	std::cout << "\t";
	std::cout << "empty  : " << tree.empty()  << "\n\t";
	std::cout << "size   : " << tree.size()   << "\n\t";
	std::cout << "height : " << tree.height() << "\n\t";

	if ( !tree.empty() )
	{
		std::cout << "min    : " << tree.min()->value << "\n\t";
		std::cout << "max    : " << tree.max()->value << "\n\t";
	}

	std::cout << "data   : ";
	tree.traverse([](const T& value) { std::cout << value << "   "; });
	std::cout << "\n\n";

	return os;
}

template<class T>
auto show_tree_info(const tree_type<T>& tree)
{
	std::vector<T> vec {};

	vec.reserve(tree.size());

	std::cout << tree;

	tree.traverse([&vec](const T& value) { vec.push_back(value); });

	bool isSorted = std::is_sorted(vec.cbegin(), vec.cend());

	if ( !isSorted )
		std::cout << "Not sorted!\n\n";

	return vec;
}

template<class T>
void test_func(std::initializer_list<T> initList)
{
	tree_type<T>   tree {};
	std::vector<T> vec  {};

	vec.reserve(initList.size());

	for (auto item : initList)
	{
		auto pr { tree.insert(item) };

		if ( !pr.second )
			std::cout << "Failed to insert a node with value " << item << "\n\n";
	}

	vec = show_tree_info(tree);

	for (auto item : vec)
	{
		auto ptr { tree.find(item) };

		if (ptr == nullptr || ptr->value != item)
			std::cout << "Not found value " << item << "\n\n";
	}

	std::random_device rd  {};
	std::mt19937_64    gen { rd() };

	while ( !tree.empty() )
	{
		std::uniform_int_distribution<int> distrib(0, static_cast<int>(vec.size()) - 1);

		T val = vec[distrib(gen)];

		std::cout << "erase " << val;

		auto ptr { tree.erase(val) };

		if (ptr != nullptr)
			std::cout << " and return " << ptr->value;
		std::cout << "\n";

		vec.clear();

		tree.traverse([&vec](const T& item) { vec.push_back(item); });

		bool isSorted = std::is_sorted(vec.cbegin(), vec.cend());

		if ( !isSorted )
			std::cout << "Not sorted!\n\n";
	}

	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	auto      initList = { 1 , 3 , 5 , 7 , 9 };
	tree_type<int> tree1 {};				//default constructor
	tree_type<int> tree2 { initList };			//constructor with initializer list
	tree_type<int> tree3 { tree2 };				//copy constructor

	std::cout << std::boolalpha;

	//insertions
	for (int i { 9 }; 0 < i; --i)
		tree1.insert(i);

	//duplicate insertions
	for (int i { 1 }; i < 10; ++i)
		tree1.insert(i);

	std::cout << "[ tree 1 ]\n" << tree1;			// 1  2  3  4  5  6  7  8  9
	std::cout << "[ tree 2 ]\n" << tree2;			// 1  3  5  7  9
	std::cout << "[ tree 3 ]\n" << tree3;			// 1  3  5  7  9

	tree_type<int> tree4 { std::move(tree2) };		//move constructor

	std::cout << "[ tree 2 ]\n" << tree2;			//
	std::cout << "[ tree 4 ]\n" << tree4;			// 1  3  5  7  9

	//insertion with initializer list
	tree2.insert({ 8 , 6 , 4 , 2 });

	std::cout << "[ tree 2 ]\n" << tree2;			// 2  4  6  8

	//swaps the contents of tree 2 and tree4
	tree2.swap(tree4);

	std::cout << "[ tree 2 ]\n" << tree2;			// 1  3  5  7  9
	std::cout << "[ tree 4 ]\n" << tree4;			// 2  4  6  8

	//erase some elements of tree 1
	for (int i { 1 }; i < 10; ++i)
	{
		if ((i & 1) == 1)
			tree1.erase(i);
	}

	std::cout << "[ tree 1 ]\n" << tree1;			// 2  4  6  8

	for (int i { 1 }; i < 10; ++i)
	{
		if ((i & 1) == 1 && tree1.contains(i))
			std::cerr << "Failed to erase the element with key " << i << "\n\n";
	}

	//clears all elements of tree 1
	tree1.clear();

	std::cout << "[ tree 1 ]\n" << tree1;			//

	//copy assignment
	tree1 = tree4;

	std::cout << "[ tree 1 ]\n" << tree1;			// 2  4  6  8

	//range insertion
	tree1.insert(initList.begin(), initList.end());

	std::cout << "[ tree 1 ]\n" << tree1;			// 1  2  3  4  5  6  7  8  9

	//finds lower bound
	for (int i { 0 }; i < 11; ++i)
	{
		auto ptr { tree1.lower_bound(i) };

		std::cout << "lower bound of " << i;

		if (ptr == nullptr)
			std::cout << " not exists";
		else
			std::cout << " is " << ptr->value;

		std::cout << "\n";
	}
	std::cout << "\n";

	//finds upper bound
	for (int i { 0 }; i < 10; ++i)
	{
		auto ptr { tree1.upper_bound(i) };

		std::cout << "upper bound of " << i;

		if (ptr == nullptr)
			std::cout << " not exists";
		else
			std::cout << " is " << ptr->value;

		std::cout << "\n";
	}
	std::cout << "\n";

	std::cout << "==========================================================\n\n";

	int i { 1 };

	for (auto tc : test_cases)
	{
		std::cout << "[ test case " << i++ << " ]\n\n\t";
		std::cout << "insert : ";

		for (auto elem : tc)
			std::cout << elem << "   ";
		std::cout << "\n\n";

		test_func(tc);

		std::cout << "----------------------------------------------------------\n\n";
	}

	return 0;
}
```
