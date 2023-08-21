#pragma once

#include <functional>
#include <initializer_list>
#include "binary_tree.h"

namespace cust					//customized / non-standard
{
	template<class T>
	struct binary_search_tree_node
	{
		using value_type   = T;
		using node_pointer = binary_search_tree_node*;

		//adds a tree node by allocating memory space and calling its constructor
		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_node(Allocator& alloc, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->parent = ptr->right = ptr->left = nullptr;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_one_node(Allocator& alloc, node_pointer l, node_pointer r, node_pointer p, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->left   = l;
			ptr->right  = r;
			ptr->parent = p;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		//appends a left child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_left(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->left == nullptr)
				ptr->left = new_one_node(alloc, nullptr, nullptr, ptr, std::forward<Args>(args)...);

			return ptr == nullptr ? nullptr : ptr->left;
		}

		//appends a right child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_right(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->right == nullptr)
				ptr->right = new_one_node(alloc, nullptr, nullptr, ptr, std::forward<Args>(args)...);

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
		[[nodiscard]] static node_pointer copy_nodes(Allocator& alloc, node_pointer src)
		{
			node_pointer ptr { nullptr };

			if (src != nullptr)
			{
				ptr = new_one_node(alloc, copy_nodes(alloc, src->left), copy_nodes(alloc, src->right), nullptr, src->value);

				//has the left  child
				if (ptr->left != nullptr)
					ptr->left->parent = ptr;

				//has the right child
				if (ptr->right != nullptr)
					ptr->right->parent = ptr;
			}

			return ptr;
		}

		node_pointer left   { nullptr };			//pointer pointing to left  child node
		node_pointer right  { nullptr };			//pointer pointing to right child node
		node_pointer parent { nullptr };			//pointer pointing to parent      node
		value_type   value  {};				//data value
	};

	template<class T, class node_type = binary_search_tree_node<T>, class Compare = std::less<T>, class Allocator = std::allocator<node_type>>
	class binary_search_tree : public binary_tree<T, node_type, Allocator>
	{
		private:
			using base_type = binary_tree<T, node_type, Allocator>;

		public:
			using value_type         = typename base_type::value_type;
			using size_type          = typename base_type::size_type;
			using node_pointer       = typename base_type::node_pointer;
			using const_node_pointer = typename base_type::const_node_pointer;

			//default constructor
			binary_search_tree() noexcept = default;

			//copy constructor
			binary_search_tree(const binary_search_tree& other) : base_type { other }, sz { other.sz }
			{
			}

			//move constructor
			binary_search_tree(binary_search_tree&& other) noexcept : base_type { std::move(other) }
			{
				std::swap(sz, other.sz);
			}

			//range constructor
			template<class InputIter>
			binary_search_tree(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					insert(*first);
			}

			//constructor with initializer list
			binary_search_tree(std::initializer_list<value_type> init)
			{
				for (const auto& elem : init)
					insert(elem);
			}

			//destructor
			~binary_search_tree()
			{
				clear();
			}

			//copy assignment
			binary_search_tree& operator = (const binary_search_tree& other)
			{
				clear();

				base_type::operator=(other);

				sz = other.sz;

				return *this;
			}

			//move assignment
			binary_search_tree& operator = (binary_search_tree&& other) noexcept
			{
				clear();

				swap(other);

				return *this;
			}

			//assignment with initializer list
			binary_search_tree& operator = (std::initializer_list<value_type> init)
			{
				clear();

				insert(init);

				return *this;
			}

			//gets the number of elements of the tree
			size_type size() const noexcept
			{
				return sz;
			}

			//checks if the tree is empty
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//clears all elements of the tree
			void clear() noexcept
			{
				base_type::clear();

				sz = 0;
			}

			//gets the pointer pointing to the node with the max value of the tree
			node_pointer max() const noexcept
			{
				return empty() ? nullptr : max(this->get_root());
			}

			//gets the pointer pointing to the node with the min value of the tree
			node_pointer min() const noexcept
			{
				return empty() ? nullptr : min(this->get_root());
			}

			//inserts an element into the tree (with copy semantics)
			std::pair<node_pointer, bool> insert(const value_type& value)
			{
				auto pr { find_location(value) };

				if (pr.second)
				{
					pr.first = (pr.first == nullptr ? this->add_root(value) : append_child(pr.first, value));

					++sz;
				}

				return std::make_pair(pr.first, pr.second);
			}

			//inserts an element into the tree (with move semantics)
			std::pair<node_pointer, bool> insert(value_type&& value)
			{
				auto pr { find_location(value) };

				if (pr.second)
				{
					pr.first = (pr.first == nullptr ? this->add_root(std::move(value)) : append_child(pr.first, std::move(value)));

					++sz;
				}

				return std::make_pair(pr.first , pr.second);
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
			std::pair<node_pointer, bool> emplace(Args&&... args)
			{
				static_assert(std::is_constructible_v<value_type, Args...>, "parameter types of function emplace() are not constructible to value_type");

				auto ptr { node_type::new_node(this->alloc, std::forward<Args>(args)...) };
				auto pr  { find_location(ptr->value) };

				if (pr.second)
				{
					if (pr.first == nullptr)
						this->root = pr.first = ptr;
					else
					{
						ptr->parent = pr.first;

						if (cmp(pr.first->value, ptr->value))
							pr.first->right = ptr;
						else
							pr.first->left  = ptr;

						pr.first = ptr;
					}

					++sz;
				}
				else
					node_type::delete_one_node(this->alloc, ptr);

				return std::make_pair(pr.first, pr.second);
			}

			//removes the element at pos from the tree
			node_pointer erase(node_pointer pos)
			{
				return erase_node(pos).first;
			}

			//removes the element with the equivalent value from the tree
			node_pointer erase(const value_type& value)
			{
				return erase(find(value));
			}

			//exchanges the elements of the tree with those of other
			void swap(binary_search_tree& other) noexcept
			{
				std::swap(this->root, other.root);
				std::swap(sz,         other.sz);
			}

			//finds the element with the equivalent value of the tree
			node_pointer find(const value_type& value) const
			{
				auto pr { find_location(value) };

				return pr.second ? nullptr : pr.first;
			}

			//checks if there is an element with the equivalent value of the tree
			bool contains(const value_type& value) const
			{
				return find(value) != nullptr;
			}

			//finds the first element with the value that is not less than the function argument
			node_pointer lower_bound(const value_type& value) const
			{
				node_pointer bound { nullptr };

				for (auto ptr { this->root }; ptr != nullptr; )
				{
					if (cmp(ptr->value, value))
						ptr = ptr->right;
					else
					{
						bound = ptr;
						ptr   = ptr->left;
					}
				}

				return bound;
			}

			//finds the first element with the value that is greater than the function argument
			node_pointer upper_bound(const value_type& value) const
			{
				node_pointer bound { nullptr };

				for (auto ptr { this->root }; ptr != nullptr; )
				{
					if (cmp(value, ptr->value))
					{
						bound = ptr;
						ptr   = ptr->left;
					}
					else
						ptr = ptr->right;
				}

				return bound;
			}

			//performs an inorder traversal of the tree
			template<class Fn>
			void traverse(Fn func) const
			{
				this->inorder(func);
			}

		protected:
			//appends a child node to the parent node (with copy semantics)
			node_pointer append_child(node_pointer parent, const value_type& value)
			{
				return parent == nullptr ? nullptr : (cmp(parent->value, value) ? this->append_right(parent, value) : this->append_left(parent, value));
			}

			//appends a child node to the parent node (with move semantics)
			node_pointer append_child(node_pointer parent, value_type&& value)
			{
				return parent == nullptr ? nullptr : (cmp(parent->value, value) ? this->append_right(parent, std::move(value)) : this->append_left(parent, std::move(value)));
			}

			//removes the element at pos from the tree
			std::pair<node_pointer, node_pointer> erase_node(node_pointer pos)
			{
				node_pointer ptr  { nullptr };		//pointer pointing to the node that will replace the position of the node to erase
				node_pointer next { nullptr };		//pointer pointing to the node that is the successor of the node to erase
				node_pointer tmp  { nullptr };		//pointer pointing to the node that its height could be changed

				if (pos == nullptr)
					return std::make_pair(nullptr, nullptr);

				//the node to erase has right subtree
				if (pos->right != nullptr)
				{
					//finds the successor (in the right subtree) of the node to erase
					ptr = next = tmp = min(pos->right);

					//deals with the left child node of the node to erase
					if (pos->left != nullptr)
					{
						ptr->left = pos->left;
						pos->left->parent = ptr;
					}

					//deals with the right child node of the successor
					if (ptr->parent != pos)
					{
						ptr->parent->left = ptr->right;

						if (ptr->right != nullptr)
							ptr->right->parent = ptr->parent;

						ptr->right = pos->right;
						pos->right->parent = ptr;

						tmp = ptr->parent;
					}

					//deals with the parent node of the successor
					ptr->parent = pos->parent;
				}
				else
				{
					//the node to erase has no right subtree but has left subtree
					if (pos->left != nullptr)
					{
						ptr = pos->left;

						//deals with the parent node of the predecessor
						ptr->parent = pos->parent;
					}

					next = successor(pos);

					tmp  = pos->parent;
				}

				//the node to erase is the root node
				if (pos == this->root)
					this->root = ptr;
				else
				{
					if (pos == pos->parent->left)
						pos->parent->left  = ptr;
					else
						pos->parent->right = ptr;
				}

				node_type::delete_one_node(this->alloc, pos);

				--sz;

				return std::make_pair(next, tmp);
			}

			/*
			 *	finds the location / position to insert a new element with the value
			 *
			 *	return value:
			 *					(nullptr if the tree is empty,                                 true)
			 *					(pointer pointing to the parent node of the possible new node, true)
			 *					(pointer pointing to the node with duplicate value,            false)
			 */
			std::pair<node_pointer, bool> find_location(const value_type& value) const
			{
				auto ptr  { this->root }, prev { ptr };
				bool flag { true };

				for ( ; flag && ptr != nullptr; )
				{
					prev = ptr;

					if (cmp(ptr->value, value))
						ptr = ptr->right;
					else if (cmp(value, ptr->value))
						ptr = ptr->left;
					else
						flag = false;
				}

				return std::make_pair(flag ? prev : ptr, flag);
			}

			//finds the node with the max value of the subtree rooted in the parent node
			node_pointer max(const_node_pointer parent) const noexcept
			{
				for ( ; parent != nullptr && parent->right != nullptr; parent = parent->right);

				return const_cast<node_pointer>(parent);
			}

			//finds the node with the min value of the subtree rooted in the parent node
			node_pointer min(const_node_pointer parent) const noexcept
			{
				for ( ; parent != nullptr && parent->left != nullptr; parent = parent->left);

				return const_cast<node_pointer>(parent);
			}

			//finds the successor of the node pointed by ptr
			node_pointer successor(const_node_pointer ptr) const noexcept
			{
				if (ptr != nullptr)
				{
					//has right subtree
					if (ptr->right != nullptr)
						ptr = min(ptr->right);
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

				return const_cast<node_pointer>(ptr);
			}

			//finds the predecessor of the node pointed by ptr
			node_pointer predecessor(const_node_pointer ptr) const noexcept
			{
				if (ptr != nullptr)
				{
					//has left subtree
					if (ptr->left != nullptr)
						ptr = max(ptr->left);
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

				return const_cast<node_pointer>(ptr);
			}

			size_type sz  { 0 };		//the number of elements / nodes of the tree
			Compare   cmp {};			//element comparison function
	};
}
