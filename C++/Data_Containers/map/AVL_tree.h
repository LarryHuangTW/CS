#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <initializer_list>
#include <queue>

namespace cust					//customized / non-standard
{
	template<class T>
	struct AVL_tree_node
	{
		using value_type      = T;
		using difference_type = long long;
		using node_pointer    = AVL_tree_node*;

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
		[[nodiscard]] static node_pointer copy_nodes(Allocator& alloc, node_pointer src)
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
		value_type      value  {};
	};

	template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
	class AVL_tree
	{
		public:
			using value_type         = T;
			using size_type          = std::size_t;
			using difference_type    = std::ptrdiff_t;
			using pointer            = value_type*;
			using const_pointer      = const value_type*;
			using reference          = value_type&;
			using const_reference    = const value_type&;
			using node_type          = AVL_tree_node<value_type>;
			using node_pointer       = node_type*;
			using const_node_pointer = const node_type*;
			using allocator_type     = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;

			//default constructor
			AVL_tree()
			{
				//creates the pseudo head node
				new_head_node();
			}

			//copy constructor
			AVL_tree(const AVL_tree& other)
			{
				new_head_node();

				if ( !other.empty() )
				{
					//root node is the left child node of the pseudo head node
					impl.pseudo_head->left = node_type::copy_nodes(get_allocator(), other.get_root());

					impl.pseudo_head->left->parent = impl.pseudo_head;		//pseudo head node is the parent node of root node
				}

				impl.sz = other.impl.sz;
			}

			//move constructor
			AVL_tree(AVL_tree&& other) noexcept
			{
				new_head_node();

				swap(other);
			}

			//range constructor
			template<class InputIter>
			AVL_tree(InputIter first, InputIter last)
			{
				new_head_node();

				for ( ; first != last; ++first)
					insert(*first);
			}

			//constructor with initializer list
			AVL_tree(std::initializer_list<value_type> init)
			{
				new_head_node();

				for (const auto& elem : init)
					insert(elem);
			}

			//destructor
			~AVL_tree()
			{
				clear();

				node_type::delete_one_node(get_allocator(), impl.pseudo_head);
			}

			//copy assignment
			AVL_tree& operator = (const AVL_tree& other)
			{
				clear();

				if ( !other.empty() )
				{
					impl.pseudo_head->left = node_type::copy_nodes(get_allocator(), other.get_root());

					impl.pseudo_head->left->parent = impl.pseudo_head;

					impl.sz = other.impl.sz;
				}

				return *this;
			}

			//move assignment
			AVL_tree& operator = (AVL_tree&& other) noexcept
			{
				clear();

				swap(other);

				return *this;
			}

			//assignment with initializer list
			AVL_tree& operator = (std::initializer_list<value_type> init)
			{
				clear();

				insert(init);

				return *this;
			}

			//gets the number of elements of the tree
			size_type size() const noexcept
			{
				return impl.sz;
			}

			//checks if the tree is empty
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//clears all elements of the tree
			void clear() noexcept
			{
				clear(get_root());

				impl.pseudo_head->left = nullptr;

				impl.sz = 0;
			}

			//gets the max value of the tree
			node_pointer max() const noexcept
			{
				return empty() ? impl.pseudo_head : node_type::max(get_root());
			}

			//gets the min value of the tree
			node_pointer min() const noexcept
			{
				return empty() ? impl.pseudo_head : node_type::min(get_root());
			}

			//gets the height of the tree / root node
			difference_type height() const noexcept
			{
				return height(get_root());
			}

			//inserts an element into the tree (with copy semantics)
			auto insert(const value_type& value)
			{
				auto pr { find_location(value) };

				if (pr.second)
				{
					pr.first = (pr.first == nullptr ? add_root(value) : append_child(pr.first, value));

					++impl.sz;

					update_height(pr.first);
				}

				return std::make_pair(pr.first, pr.second);
			}

			//inserts an element into the tree (with move semantics)
			auto insert(value_type&& value)
			{
				auto pr { find_location(value) };

				if (pr.second)
				{
					pr.first = (pr.first == nullptr ? add_root(std::move(value)) : append_child(pr.first, std::move(value)));

					++impl.sz;

					update_height(pr.first);
				}

				return std::make_pair(pr.first, pr.second);
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
				static_assert(std::is_constructible_v<value_type, Args...>, "parameter types of function emplace() are not constructible to value_type");

				auto  ptr { node_type::new_node(get_allocator(), std::forward<Args>(args)...) };
				auto  pr  { find_location(ptr->value) };
				auto& cmp { get_compare() };

				if (pr.second)
				{
					if (pr.first == nullptr)
					{
						impl.pseudo_head->left = pr.first = ptr;
						ptr->parent = impl.pseudo_head;
					}
					else
					{
						ptr->parent = pr.first;

						if (cmp(pr.first->value, ptr->value))
							pr.first->right = ptr;
						else
							pr.first->left  = ptr;

						pr.first = ptr;
					}

					++impl.sz;

					update_height(pr.first);
				}
				else
					node_type::delete_one_node(get_allocator(), ptr);

				return std::make_pair(pr.first, pr.second);
			}

			//removes the element at pos from the tree
			node_pointer erase(node_pointer pos)
			{
				auto pr { erase_node(pos) };

				update_height(pr.second);

				return pr.first;
			}

			//removes the element with the equivalent val from the tree
			template<class Ty>
			node_pointer erase(const Ty& val)
			{
				return erase(find(val));
			}

			//exchanges the elements of the tree with those of other
			void swap(AVL_tree& other) noexcept
			{
				std::swap(impl.pseudo_head, other.impl.pseudo_head);
				std::swap(impl.sz,          other.impl.sz);
			}

			//finds the element with the equivalent val of the tree
			template<class Ty>
			node_pointer find(const Ty& val) const
			{
				auto pr { find_location(val) };

				return pr.second ? nullptr : pr.first;
			}

			//checks if there is an element with the equivalent val of the tree
			template<class Ty>
			bool contains(const Ty& val) const
			{
				return find(val) != nullptr;
			}

			//finds the first element with the key or value that is not less than val
			template<class Ty>
			node_pointer lower_bound(const Ty& val) const
			{
				node_pointer bound { nullptr };
				auto&        cmp   { get_compare() };

				for (auto ptr { get_root() }; ptr != nullptr; )
				{
					if (cmp(ptr->value, val))
						ptr = ptr->right;
					else
					{
						bound = ptr;
						ptr   = ptr->left;
					}
				}

				return bound;
			}

			//finds the first element with the key or value that is greater than val
			template<class Ty>
			node_pointer upper_bound(const Ty& val) const
			{
				node_pointer bound { nullptr };
				auto&        cmp   { get_compare() };

				for (auto ptr { get_root() }; ptr != nullptr; )
				{
					if (cmp(val, ptr->value))
					{
						bound = ptr;
						ptr   = ptr->left;
					}
					else
						ptr = ptr->right;
				}

				return bound;
			}

			//performs a preorder traversal
			template<class Fn>
			void preorder(Fn func) const
			{
				preorder(get_root(), func);
			}

			//performs an inorder traversal
			template<class Fn>
			void inorder(Fn func) const
			{
				inorder(get_root(), func);
			}

			//performs a postorder traversal
			template<class Fn>
			void postorder(Fn func) const
			{
				postorder(get_root(), func);
			}

			//performs a levelorder traversal
			template<class Fn>
			void levelorder(Fn func) const
			{
				std::queue<node_pointer> que {};

				if ( !empty() )
					que.push(get_root());

				for (node_pointer ptr { nullptr }; !que.empty(); )
				{
					ptr = que.front();

					func(ptr->value);

					que.pop();

					//has the left  child
					if (ptr->left != nullptr)
						que.push(ptr->left);

					//has the right child
					if (ptr->right != nullptr)
						que.push(ptr->right);
				}
			}

			//performs an inorder traversal of the tree
			template<class Fn>
			void traverse(Fn func) const
			{
				inorder(func);
			}

			//finds the successor of the node pointed by ptr
			node_pointer successor(node_pointer ptr) const noexcept
			{
				return node_type::successor(ptr);
			}

			//finds the predecessor of the node pointed by ptr
			node_pointer predecessor(node_pointer ptr) const noexcept
			{
				return node_type::predecessor(ptr);
			}

		private:
			enum class rotate_to : char { left, right, nowhere };

			//new a pseudo head node
			node_pointer new_head_node()
			{
				auto ptr { node_type::new_node(get_allocator()) };

				impl.pseudo_head = ptr->parent = ptr;

				return ptr;
			}

			//adds the root node if the tree is empty
			template<class... Args>
			node_pointer add_root(Args&&... args)
			{
				if (empty() && impl.pseudo_head->left == nullptr)
					impl.pseudo_head->left = node_type::new_one_node(get_allocator(), nullptr, nullptr, impl.pseudo_head, 0, std::forward<Args>(args)...);

				return impl.pseudo_head->left;
			}

			//appends a left child node to the parent node pointed by ptr
			template<class... Args>
			node_pointer append_left(node_pointer ptr, Args&&... args)
			{
				return node_type::append_left(get_allocator(), ptr, std::forward<Args>(args)...);
			}

			//appends a right child node to the parent node pointed by ptr
			template<class... Args>
			node_pointer append_right(node_pointer ptr, Args&&... args)
			{
				return node_type::append_right(get_allocator(), ptr, std::forward<Args>(args)...);
			}

			//appends a child node to the parent node (with copy semantics)
			node_pointer append_child(node_pointer parent, const value_type& value)
			{
				auto& cmp { get_compare() };

				return parent == nullptr ? nullptr : (cmp(parent->value, value) ? append_right(parent, value) : append_left(parent, value));
			}

			//appends a child node to the parent node (with move semantics)
			node_pointer append_child(node_pointer parent, value_type&& value)
			{
				auto& cmp { get_compare() };

				return parent == nullptr ? nullptr : (cmp(parent->value, value) ? append_right(parent, std::move(value)) : append_left(parent, std::move(value)));
			}

			//removes the element at pos from the tree
			std::pair<node_pointer, node_pointer> erase_node(node_pointer pos)
			{
				node_pointer ptr  { nullptr };		//pointer pointing to the node that will replace the position of the node to erase
				node_pointer next { nullptr };		//pointer pointing to the node that is the successor of the node to erase
				node_pointer tmp  { nullptr };		//pointer pointing to the node that its height could be changed

				if (pos == nullptr)
					return std::make_pair(nullptr, nullptr);

				//the node to erase has the right subtree
				if (pos->right != nullptr)
				{
					//finds the successor (in the right subtree) of the node to erase
					ptr = next = tmp = node_type::min(pos->right);

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
					//the node to erase has no right subtree but has the left subtree
					if (pos->left != nullptr)
					{
						ptr = pos->left;

						//deals with the parent node of the predecessor
						ptr->parent = pos->parent;
					}

					next = node_type::successor(pos);

					tmp = pos->parent;
				}

				//the node to erase is the root node
				if (pos == get_root())
				{
					//ptr->parent = impl.pseudo_head;
					impl.pseudo_head->left = ptr;
				}
				else
				{
					if (pos == pos->parent->left)
						pos->parent->left  = ptr;
					else
						pos->parent->right = ptr;
				}

				node_type::delete_one_node(get_allocator(), pos);

				--impl.sz;

				return std::make_pair(next, tmp);
			}

			//clear all elements of the subtree rooted in the ptr node
			void clear(node_pointer ptr) noexcept
			{
				if (ptr != nullptr)
				{
					clear(ptr->left);
					clear(ptr->right);

					node_type::delete_one_node(get_allocator(), ptr);
				}
			}

			/*
			 *	finds the location / position to insert a new element according to its key or value
			 *
			 *	return value:
			 *					(nullptr if the tree is empty,                                 true)
			 *					(pointer pointing to the parent node of the possible new node, true)
			 *					(pointer pointing to the node with duplicate value,            false)
			 */
			template<class Ty>
			std::pair<node_pointer, bool> find_location(const Ty& val) const
			{
				auto  ptr  { get_root() }, prev { ptr };
				bool  flag { true };
				auto& cmp  { get_compare() };

				for ( ; flag && ptr != nullptr; )
				{
					prev = ptr;

					if (cmp(ptr->value, val))
						ptr = ptr->right;
					else if (cmp(val, ptr->value))
						ptr = ptr->left;
					else
						flag = false;
				}

				return std::make_pair(flag ? prev : ptr, flag);
			}

			//gets the height of the ptr node
			difference_type height(node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? -1 : ptr->height;
			}

			//calculates the balance factor of the node pointed by ptr
			difference_type get_balance_factor(node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? 0 : height(ptr->right) - height(ptr->left);
			}

			//checks the balance of the node pointed by ptr
			rotate_to check_balance(node_pointer ptr) const noexcept
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
				if (ptr != nullptr && ptr != impl.pseudo_head)
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

				if (ptr->parent->left == ptr)
					ptr->parent->left  = ptr->right;
				else
					ptr->parent->right = ptr->right;

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

				if (ptr->parent->left == ptr)
					ptr->parent->left  = ptr->left;
				else
					ptr->parent->right = ptr->left;

				ptr->left->parent  = ptr->parent;
				ptr->parent        = ptr->left;

				ptr->left = ptr->left->right;

				ptr->parent->right = ptr;

				if (ptr->left != nullptr)
					ptr->left->parent = ptr;

				return ptr;
			}

			//performs rotation(s)
			node_pointer rotate(rotate_to direction, node_pointer ptr) noexcept
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

			//recursive version of preorder traversal
			template<class Fn>
			void preorder(node_pointer ptr, Fn func) const
			{
				if (ptr != nullptr)
				{
					func(ptr->value);
					preorder(ptr->left,  func);
					preorder(ptr->right, func);
				}
			}

			//recursive version of inorder traversal
			template<class Fn>
			void inorder(node_pointer ptr, Fn func) const
			{
				if (ptr != nullptr)
				{
					inorder(ptr->left,  func);
					func(ptr->value);
					inorder(ptr->right, func);
				}
			}

			//recursive version of postorder traversal
			template<class Fn>
			void postorder(node_pointer ptr, Fn func) const
			{
				if (ptr != nullptr)
				{
					postorder(ptr->left,  func);
					postorder(ptr->right, func);
					func(ptr->value);
				}
			}

			//gets the allocator
			allocator_type& get_allocator() noexcept
			{
				return impl;
			}

			const allocator_type& get_allocator() const noexcept
			{
				return impl;
			}

			//gets the compare function object
			Compare& get_compare() noexcept
			{
				return impl;
			}

			const Compare& get_compare() const noexcept
			{
				return impl;
			}

			//gets the pointer pointing to the root node
			node_pointer get_root() const noexcept
			{
				return impl.pseudo_head->left;
			}

			//implementation type
			struct impl_type : public Compare, allocator_type
			{
				node_pointer pseudo_head { nullptr };			//a pointer pointing to pseudo head node
				size_type    sz          { 0 };				//the number of elements of the tree
			};

			impl_type impl {};						//implementation of AVL tree
	};
}
