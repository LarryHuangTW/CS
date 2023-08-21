#pragma once

#include <iostream>
#include <cstddef>
#include <memory>
#include <algorithm>
#include <utility>
#include <stack>
#include <queue>
#include <type_traits>

namespace cust					//customized / non-standard
{
	template<class T>
	struct binary_tree_node
	{
		using value_type   = T;
		using node_pointer = binary_tree_node*;

		//adds a tree node by allocating memory space and calling its constructor
		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_node(Allocator& alloc, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->right = ptr->left = nullptr;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_one_node(Allocator& alloc, node_pointer l, node_pointer r, Args&&... args)
		{
			auto ptr { std::allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->left  = l;
			ptr->right = r;
			std::allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		//appends a left child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_left(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->left == nullptr)
				ptr->left = new_node(alloc, std::forward<Args>(args)...);

			return ptr == nullptr ? nullptr : ptr->left;
		}

		//appends a right child node to the node pointed by ptr
		template<class Allocator, class... Args>
		static node_pointer append_right(Allocator& alloc, node_pointer ptr, Args&&... args)
		{
			if (ptr != nullptr && ptr->right == nullptr)
				ptr->right = new_node(alloc, std::forward<Args>(args)...);

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
			return src == nullptr ? nullptr : new_one_node(alloc, copy_nodes(alloc, src->left), copy_nodes(alloc, src->right), src->value);
		}

		node_pointer left  { nullptr };			//pointer pointing to left  child node
		node_pointer right { nullptr };			//pointer pointing to right child node
		value_type   value {};				//data value
	};

	template<class T, class node_type = binary_tree_node<T>, class Allocator = std::allocator<node_type>>
	class binary_tree
	{
		public:
			using value_type         = T;
			using size_type          = std::size_t;
			using difference_type    = std::ptrdiff_t;
			using pointer            = value_type*;
			using const_pointer      = const value_type*;
			using reference          = value_type&;
			using const_reference    = const value_type&;
			using node_pointer       = node_type*;
			using const_node_pointer = const node_type*;
			using allocator_type     = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;
			using alloc_traits_type  = std::allocator_traits<allocator_type>;

			//default constructor
			binary_tree() noexcept = default;

			//copy constructor
			binary_tree(const binary_tree& other)
			{
				root = node_type::copy_nodes(alloc, other.root);
			}

			//move constructor
			binary_tree(binary_tree&& other) noexcept
			{
				std::swap(root, other.root);
			}

			//constructor with the sequences of traversal results of preorder / postorder and inorder
			binary_tree(const_pointer preorder, const_pointer inorder, const_pointer postorder, size_type sz)
			{
				if (inorder != nullptr && 0 < sz)
					root = build_tree(preorder != nullptr ? preorder : postorder, inorder, sz, preorder != nullptr);
			}

			//destructor
			~binary_tree()
			{
				clear();
			}

			//copy assignment
			binary_tree& operator = (const binary_tree& other)
			{
				clear();

				root = node_type::copy_nodes(alloc, other.root);

				return *this;
			}

			//move assignment
			binary_tree& operator = (binary_tree&& other) noexcept
			{
				clear();

				std::swap(root, other.root);

				return *this;
			}

			//gets the number of elements of the tree
			size_type size() const noexcept
			{
				return size(root);
			}

			//checks if the tree is empty
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//gets the height of the tree, -1 if the tree is empty
			difference_type height() const noexcept
			{
				return height(root);
			}
				
			//clears all elements of the tree
			void clear() noexcept
			{
				clear(root);

				root = nullptr;
			}

			//performs a preorder traversal
			template<class Fn>
			void preorder(Fn func) const
			{
				preorder(root, func);
			}

			//performs an inorder traversal
			template<class Fn>
			void inorder(Fn func) const
			{
				inorder(root, func);
			}

			//performs a postorder traversal
			template<class Fn>
			void postorder(Fn func) const
			{
				postorder(root, func);
			}

			//performs a levelorder traversal
			template<class Fn>
			void levelorder(Fn func) const
			{
				std::queue<node_pointer> que {};

				if (root != nullptr)
					que.push(root);

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

			//adds the root node if the tree is empty
			template<class... Args>
			node_pointer add_root(Args&&... args)
			{
				if (empty())
					root = node_type::new_node(alloc, std::forward<Args>(args)...);

				return root;
			}

			//appends a left child node to the parent node pointed by ptr
			template<class... Args>
			node_pointer append_left(node_pointer ptr, Args&&... args)
			{
				return node_type::append_left(alloc, ptr, std::forward<Args>(args)...);
			}

			//appends a right child node to the parent node pointed by ptr
			template<class... Args>
			node_pointer append_right(node_pointer ptr, Args&&... args)
			{
				return node_type::append_right(alloc, ptr, std::forward<Args>(args)...);
			}

			//gets a pointer pointing to the root node
			node_pointer get_root() noexcept
			{
				return root;
			}

			const_node_pointer get_root() const noexcept
			{
				return root;
			}

		protected:
			/*
				*	Given the sequences of preorder/postorder and inorder traversal, construct the unique binary tree
				*
				*	xorder:
				*			preorder,  if flag is true
				*			postorder, if flag is false
				*/
			node_pointer build_tree(const_pointer xorder, const_pointer inorder, size_type sz, bool flag = true)
			{
				auto      ptr { xorder };
				size_type i   { 0 };

				if (sz == 0)
					return nullptr;

				if ( !flag )
					ptr += sz - 1;

				for ( ; i != sz && *ptr != inorder[i]; ++i);

				return node_type::new_one_node(alloc, build_tree(xorder + flag, inorder, i, flag), build_tree(xorder + (i + flag), inorder + i + 1, sz - i - 1, flag), *ptr);
			}

			//gets the number of elements of the subtree rooted in the ptr node
			size_type size(node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? 0 : 1 + size(ptr->left) + size(ptr->right);
			}

			//gets the height of the ptr node
			difference_type height(node_pointer ptr) const noexcept
			{
				return ptr == nullptr ? -1 : 1 + std::max(height(ptr->left), height(ptr->right));
			}

			//clears all elements of the subtree rooted in the ptr node
			void clear(node_pointer ptr) noexcept
			{
				if (ptr != nullptr)
				{
					clear(ptr->left);
					clear(ptr->right);

					node_type::delete_one_node(alloc, ptr);
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

			node_pointer   root  { nullptr };		//pointer points to root node
			allocator_type alloc {};			//allocator object
	};
}
