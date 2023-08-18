### *Binary Tree:*
* Recursive     version
* Non-recursive version

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//binary tree node
	template<class T>
	struct binary_tree_node
	{
		// ......

		node_pointer left  { nullptr };		//pointer pointing to left  child node
		node_pointer right { nullptr };		//pointer pointing to right child node
		value_type   value {};			//data value
	};

	namespace recursive_version
	{
		//binary tree data structure
		template<class T, class node_type = binary_tree_node<T>, class Allocator = std::allocator<node_type>>
		class binary_tree
		{
			public:
				//member types

				//constructors
				binary_tree() noexcept;
				binary_tree(const binary_tree& other);
				binary_tree(binary_tree&& other) noexcept;
				binary_tree(const_pointer preorder, const_pointer inorder, const_pointer postorder, size_type sz);

				//destructor
				~binary_tree();

				//assignment operators
				binary_tree& operator = (const binary_tree& other);
				binary_tree& operator = (binary_tree&& other) noexcept;

				size_type size() const noexcept;		//gets the number of elements of the tree
				bool empty() const noexcept;			//checks if the tree is empty
				difference_type height() const noexcept;	//gets the height of the tree, -1 if the tree is empty
				void clear() noexcept;				//clears all elements of the tree

				template<class Fn>
				void preorder(Fn func);				//performs a preorder traversal

				template<class Fn>
				void inorder(Fn func);				//performs an inorder traversal

				template<class Fn>
				void postorder(Fn func);			//performs a postorder traversal

				template<class Fn>
				void levelorder(Fn func);			//performs a levelorder traversal

				template<class... Args>
				node_pointer add_root(Args&&... args);		//adds the root node if the tree is empty

				template<class... Args>
				node_pointer append_left(node_pointer ptr, Args&&... args);	//appends a left  child node to the parent node pointed by ptr

				template<class... Args>
				node_pointer append_right(node_pointer ptr, Args&&... args);	//appends a right child node to the parent node pointed by ptr

				node_pointer get_root() noexcept;		//gets a pointer pointing to the root node

			protected:
				// ......

				node_pointer   root  { nullptr };		//pointer points to root node
				allocator_type alloc {};			//allocator object
		};
	}

	namespace non_recursive_version
	{
		//binary tree data structure
		template<class T, class node_type = binary_tree_node<T>, class Allocator = std::allocator<node_type>>
		class binary_tree
		{
			//All APIs are the same as the above except that program logic is non-recursive
		};
	}
}
```

## Example:

```C++
#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <string>
#include "binary_tree.h"

struct test_data
{
	const char* preorder   { nullptr };
	const char* inorder    { nullptr };
	const char* postorder  { nullptr };
	const char* levelorder { nullptr };
	std::size_t sz         { 0 };
	long long   height     { 0 };
};

const std::initializer_list<test_data> test_cases {	{ ""    , ""    , ""    , ""    , 0 , -1 } ,
							{ "A"   , "A"   , "A"   , "A"   , 1 ,  0 } ,
							{ "AB"  , "BA"  , "BA"  , "AB"  , 2 ,  1 } ,
							{ "AB"  , "AB"  , "BA"  , "AB"  , 2 ,  1 } ,
							{ "ABC" , "BAC" , "BCA" , "ABC" , 3 ,  1 } ,
							{ "ABC" , "CBA" , "CBA" , "ABC" , 3 ,  2 } ,
							{ "ABC" , "ABC" , "CBA" , "ABC" , 3 ,  2 } ,
							{ "ABC" , "BCA" , "CBA" , "ABC" , 3 ,  2 } ,
							{ "ABC" , "ACB" , "CBA" , "ABC" , 3 ,  2 } ,
							{ "ABDECFG"   , "DBEAFCG"   , "DEBFGCA"   , "ABCDEFG"   , 7 , 2 } ,
							{ "ABDCE"     , "DBACE"     , "DBECA"     , "ABCDE"     , 5 , 2 } ,
							{ "ABCDE"     , "BADCE"     , "BDECA"     , "ABCDE"     , 5 , 2 } ,
							{ "KACBDEFG"  , "ABCKFGED"  , "BCAGFEDK"  , "KADCEBFG"  , 8 , 4 } ,
							{ "CBADEFGHI" , "ADEBFHIGC" , "EDAIHGFBC" , "CBAFDGEHI" , 9 , 5 } ,
							{ "ABCDEFGHI" , "BCAEDGHFI" , "CBEHGIFDA" , "ABDCEFGIH" , 9 , 4 } ,
							{ "ABECDFGHI" , "EBCDAGHFI" , "EDCBHGIFA" , "ABFECGIDH" , 9 , 3 } ,
							{ "ABCDEFGHI" , "BCDAFEHIG" , "DCBFIHGEA" , "ABECFGDHI" , 9 , 4 } };

void test_func(const auto& tree, const test_data& tc)
{
	auto        sz     { tree.size()   };
	auto        height { tree.height() };
	std::string str    {};

	auto lambda = [&str](char ch) { str.append(1, ch); };

	std::cout << "empty      : " << tree.empty() << "\n";
	std::cout << "size       : " << sz;

	if (sz != tc.sz)
		std::cout << " != " << tc.sz;

	std::cout << "\n";

	std::cout << "height     : " << height;

	if (height != tc.height)
		std::cout << " != " << tc.height;

	std::cout << "\n\n";

	tree.preorder(lambda);

	std::cout << "Preorder   : " << str;

	if (str.compare(tc.preorder) != 0)
		std::cout << " != " << tc.preorder;

	std::cout << "\n\n";

	str.clear();

	tree.inorder(lambda);

	std::cout << "Inorder    : " << str;

	if (str.compare(tc.inorder) != 0)
		std::cout << " != " << tc.inorder;

	std::cout << "\n\n";

	str.clear();

	tree.postorder(lambda);

	std::cout << "Postorder  : " << str;

	if (str.compare(tc.postorder) != 0)
		std::cout << " != " << tc.postorder;

	std::cout << "\n\n";

	str.clear();

	tree.levelorder(lambda);

	std::cout << "Levelorder : " << str;

	if (str.compare(tc.levelorder) != 0)
		std::cout << " != " << tc.levelorder;

	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	std::cout << std::boolalpha;

	{
		using cust::recursive_version::binary_tree;

		std::cout << "[ Recursive version ]\n\n";

		std::cout << "Using preorder and inorder to build a binary tree:\n\n";

		for (const auto& tc : test_cases)
		{
			binary_tree<char> bt { tc.preorder , tc.inorder , nullptr , tc.sz };

			test_func(bt, tc);

			std::cout << "---------------------------------\n\n";
		}

		std::cout << "=================================\n\n";

		std::cout << "Using inorder and postorder to build a binary tree:\n\n";

		for (const auto& tc : test_cases)
		{
			binary_tree<char> bt { nullptr , tc.inorder , tc.postorder , tc.sz };

			test_func(bt, tc);

			std::cout << "---------------------------------\n\n";
		}
	}

	std::cout << "=================================\n\n";

	{
		using cust::non_recursive_version::binary_tree;

		std::cout << "[ Non-recursive version ]\n\n";

		std::cout << "Using preorder and inorder to build a binary tree:\n\n";

		for (const auto& tc : test_cases)
		{
			binary_tree<char> bt { tc.preorder , tc.inorder , nullptr , tc.sz };

			test_func(bt, tc);

			std::cout << "---------------------------------\n\n";
		}

		std::cout << "=================================\n\n";

		std::cout << "Using inorder and postorder to build a binary tree:\n\n";

		for (const auto& tc : test_cases)
		{
			binary_tree<char> bt { nullptr , tc.inorder , tc.postorder , tc.sz };

			test_func(bt, tc);

			std::cout << "---------------------------------\n\n";
		}
	}

	return 0;
}
```
