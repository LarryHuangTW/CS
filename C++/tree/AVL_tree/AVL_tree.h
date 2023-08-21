#pragma once

#include "binary_search_tree.h"

namespace cust					//customized / non-standard
{
	//AVL tree node
	template<class T>
	struct AVL_tree_node
	{
		using value_type         = T;
		using difference_type    = long long;
		using node_pointer       = AVL_tree_node*;
		using const_node_pointer = const AVL_tree_node*;

		//adds a tree node by allocating memory space and calling its constructor
		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_node(Allocator& alloc, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->parent = ptr->right = ptr->left = nullptr;
			ptr->height = 0;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_one_node(Allocator& alloc, node_pointer l, node_pointer r, node_pointer p, difference_type h, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->left   = l;
			ptr->right  = r;
			ptr->parent = p;
			ptr->height = h;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		//appends a left child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_left(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->left == nullptr)
				ptr->left = new_one_node(alloc, nullptr, nullptr, ptr, 0, std::forward<Args>(args)...);

			return ptr == nullptr ? nullptr : ptr->left;
		}

		//appends a right child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_right(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->right == nullptr)
				ptr->right = new_one_node(alloc, nullptr, nullptr, ptr, 0, std::forward<Args>(args)...);

			return ptr == nullptr ? nullptr : ptr->right;
		}

		//deletes a tree node by calling its destructor and deallocating memory space
		template<class Allocator>
		static void delete_one_node(Allocator& alloc, node_pointer ptr) noexcept
		{
			std::allocator_traits<Allocator>::destroy(alloc, ptr);
			std::allocator_traits<Allocator>::deallocate(alloc, ptr, 1);
		}

		//copies the subtree rooted in the src node
		template<class Allocator>
		[[nodiscard]] static node_pointer copy_nodes(Allocator& alloc, const_node_pointer src)
		{
			node_pointer ptr { nullptr };

			if (src != nullptr)
			{
				ptr = new_one_node(alloc, copy_nodes(alloc, src->left), copy_nodes(alloc, src->right), nullptr, src->height, src->value);

				//has the left  child
				if (ptr->left != nullptr)
					ptr->left->parent = ptr;

				//has the right child
				if (ptr->right != nullptr)
					ptr->right->parent = ptr;
			}

			return ptr;
		}

		//finds the node with the max value of the subtree rooted in the parent node
		static node_pointer max(node_pointer ptr) noexcept
		{
			for ( ; ptr != nullptr && ptr->right != nullptr; ptr = ptr->right);

			return ptr;
		}

		//finds the node with the min value of the subtree rooted in the parent node
		static node_pointer min(node_pointer ptr) noexcept
		{
			for ( ; ptr != nullptr && ptr->left != nullptr; ptr = ptr->left);

			return ptr;
		}

		//finds the successor of the node pointed by ptr
		static node_pointer successor(node_pointer ptr) noexcept
		{
			if (ptr != nullptr)
			{
				//has the right subtree
				if (ptr->right != nullptr)
				{
					ptr = min(ptr->right);
				}
				else
				{
					auto parent { ptr->parent };

					for ( ; parent != nullptr && parent->right == ptr; )
					{
						ptr    = parent;
						parent = parent->parent;
					}

					ptr = parent;
				}
			}

			return ptr;
		}

		//finds the predecessor of the node pointed by ptr
		static node_pointer predecessor(node_pointer ptr) noexcept
		{
			if (ptr != nullptr)
			{
				//has left subtree
				if (ptr->left != nullptr)
				{
					ptr = max(ptr->left);
				}
				else
				{
					auto parent { ptr->parent };

					for ( ; parent != nullptr && parent->left == ptr; )
					{
						ptr    = parent;
						parent = parent->parent;
					}

					ptr = parent;
				}
			}

			return ptr;
		}

		node_pointer    left   { nullptr };			//pointer pointing to left  child node
		node_pointer    right  { nullptr };			//pointer pointing to right child node
		node_pointer    parent { nullptr };			//pointer pointing to parent      node
		difference_type height { 0 };				//height of the node
		value_type      value  {};					//data value
	};

	//AVL tree data structure
	template<class T, class node_type = AVL_tree_node<T>, class Compare = std::less<T>, class Allocator = std::allocator<node_type>>
	class AVL_tree : public binary_search_tree<T, node_type, Compare, Allocator>
	{
		private:
			using base_type = binary_search_tree<T, node_type, Compare, Allocator>;

		public:
			using value_type         = typename base_type::value_type;
			using size_type          = typename base_type::size_type;
			using difference_type    = typename base_type::difference_type;
			using node_pointer       = typename base_type::node_pointer;
			using const_node_pointer = typename base_type::const_node_pointer;
			using allocator_type     = typename base_type::allocator_type;

			//default constructor
			AVL_tree() noexcept = default;

			//copy constructor
			AVL_tree(const AVL_tree& other)
			{
				this->root = node_type::copy_nodes(get_allocator(), other.get_root());
				this->sz   = other.sz;
			}

			//move constructor
			AVL_tree(AVL_tree&& other) noexcept
			{
				this->swap(other);
			}

			//range constructor
			template<class InputIter>
			AVL_tree(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					insert(*first);
			}

			//constructor with initializer list
			AVL_tree(std::initializer_list<value_type> init)
			{
				for (const auto& elem : init)
					insert(elem);
			}

			//destructor
			~AVL_tree()
			{
			}

			//copy assignment
			AVL_tree& operator = (const AVL_tree& other)
			{
				base_type::operator=(other);

				return *this;
			}

			//move assignment
			AVL_tree& operator = (AVL_tree&& other) noexcept
			{
				base_type::operator=(std::move(other));

				return *this;
			}

			//assignment with initializer list
			AVL_tree& operator = (std::initializer_list<value_type> init)
			{
				this->clear();

				insert(init);

				return *this;
			}

			//gets the height of the tree / root node
			difference_type height() const noexcept
			{
				return height(this->get_root());
			}

			//inserts an element into the tree (with copy semantics)
			auto insert(const value_type& value)
			{
				auto pr { base_type::insert(value) };

				if (pr.second)
					update_height(pr.first);

				return pr;
			}

			//inserts an element into the tree (with move semantics)
			auto insert(value_type&& value)
			{
				auto pr { base_type::insert(std::move(value)) };

				if (pr.second)
					update_height(pr.first);

				return pr;
			}

			//inserts a range of elements [first, last) into the tree
			template<class InputIter>
			void insert(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					insert(*first);
			}

			//inserts the elements of the initializer list into the tree
			void insert(std::initializer_list<value_type> init)
			{
				insert(init.begin(), init.end());
			}

			//inserts an element into the tree with its value constructed in-place
			template<class... Args>
			auto emplace(Args&&... args)
			{
				auto pr { base_type::emplace(std::forward<Args>(args)...) };

				if (pr.second)
					update_height(pr.first);

				return pr;
			}

			//removes the element at pos from the tree
			auto erase(node_pointer pos)
			{
				auto pr { base_type::erase_node(pos) };

				update_height(pr.second);

				return pr.first;
			}

			//removes the element with the equivalent value from the tree
			auto erase(const value_type& value)
			{
				return erase(this->find(value));
			}

			//gets the allocator
			allocator_type& get_allocator() noexcept
			{
				return this->alloc;
			}

			const allocator_type& get_allocator() const noexcept
			{
				return this->alloc;
			}

			//gets the compare function object
			Compare& get_compare() noexcept
			{
				return this->cmp;
			}

			const Compare& get_compare() const noexcept
			{
				return this->cmp;
			}

		private:
			enum class rotate_to : char { left, right, nowhere };

			//gets the height of the node pointed by ptr
			difference_type height(const_node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? -1 : ptr->height;
			}

			//calculates the balance factor of the node pointed by ptr
			difference_type get_balance_factor(const_node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? 0 : height(ptr->right) - height(ptr->left);
			}

			//checks the balance of the node pointed by ptr
			rotate_to check_balance(const_node_pointer ptr) const noexcept
			{
				auto balance_factor { get_balance_factor(ptr) };

				return balance_factor < -1 ? rotate_to::right : (balance_factor > 1 ? rotate_to::left : rotate_to::nowhere);
			}

			//updates the height of the node pointed by ptr
			void update_node_height(node_pointer ptr)
			{
				if (ptr != nullptr)
					ptr->height = 1 + std::max(height(ptr->left), height(ptr->right));
			}

			/*
			 *	1. checks the balance of the node pointed by ptr
			 *	2. rotates if it is unbalanced
			 *	3. updates the height
			 *	4. performs 1 ~ 3 actions to the parent node and ancestral nodes
			 */
			void update_height(node_pointer ptr)
			{
				if (ptr != nullptr)
				{
					ptr = rotate(check_balance(ptr), ptr);

					update_node_height(ptr);

					update_height(ptr->parent);
				}
			}

			//performs a left rotation
			node_pointer rotate_left(node_pointer ptr)
			{
				if (ptr == nullptr)
					return nullptr;

				if (ptr->parent == nullptr)
					this->root = ptr->right;
				else
				{
					if (ptr->parent->left == ptr)
						ptr->parent->left  = ptr->right;
					else
						ptr->parent->right = ptr->right;
				}

				ptr->right->parent = ptr->parent;
				ptr->parent        = ptr->right;

				ptr->right = ptr->right->left;

				ptr->parent->left = ptr;

				if (ptr->right != nullptr)
					ptr->right->parent = ptr;

				return ptr;
			}

			//performs a right rotation
			node_pointer rotate_right(node_pointer ptr)
			{
				if (ptr == nullptr)
					return nullptr;

				if (ptr->parent == nullptr)
					this->root = ptr->left;
				else
				{
					if (ptr->parent->left == ptr)
						ptr->parent->left  = ptr->left;
					else
						ptr->parent->right = ptr->left;
				}

				ptr->left->parent = ptr->parent;
				ptr->parent       = ptr->left;

				ptr->left = ptr->left->right;

				ptr->parent->right = ptr;

				if (ptr->left != nullptr)
					ptr->left->parent = ptr;

				return ptr;
			}

			//performs rotation(s)
			node_pointer rotate(rotate_to direction, node_pointer ptr)
			{
				if (direction == rotate_to::left)
				{
					if (get_balance_factor(ptr->right) < 0)
						update_node_height(rotate_right(ptr->right));

					return rotate_left(ptr);
				}
				else if (direction == rotate_to::right)
				{
					if (get_balance_factor(ptr->left) > 0)
						update_node_height(rotate_left(ptr->left));

					return rotate_right(ptr);
				}
				else			// rotate_to::nowhere
				{
					return ptr;
				}
			}
	};
}
