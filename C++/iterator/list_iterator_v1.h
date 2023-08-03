#pragma once

/*
 *	The design of iterators of list data container (version 1).
 *
 *	Two separate class templates with 90% code similarity.
 */

#include <iterator>
#include <cstddef>

namespace cust					//customized / non-standard
{
	using std::bidirectional_iterator_tag;
	using std::ptrdiff_t;

	//forward declaration
	template<class T>
	struct list_node;

	template<class T>
	class list_const_iterator;

	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<class T>
	class list_iterator
	{
		template<class T, class Allocator>
		friend class list;

		private:
			using node_type    = list_node<T>;
			using node_pointer = node_type*;

			node_pointer& next() noexcept
			{
				return ptr->next;
			}

			node_pointer& prev() noexcept
			{
				return ptr->prev;
			}

		public:
			using iterator_category = bidirectional_iterator_tag;
			using value_type        = T;
			using difference_type   = ptrdiff_t;
			using pointer           = value_type*;
			using reference         = value_type&;

			list_iterator() noexcept = default;

			list_iterator(node_pointer p) noexcept : ptr { p }
			{
			}

			reference operator * () const noexcept
			{
				return ptr->value;
			}

			pointer operator -> () const noexcept
			{
				return std::addressof(**this);;
			}

			auto& operator ++ () noexcept
			{
				//some checking here

				ptr = next();

				return *this;
			}

			auto operator ++ (int) noexcept
			{
				auto tmp { *this };

				operator++();

				return tmp;
			}

			auto& operator -- () noexcept
			{
				//some checking here

				ptr = prev();

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			bool operator == (const list_iterator& other) const noexcept
			{
				return ptr == other.ptr;
			}

			bool operator != (const list_iterator& other) const noexcept
			{
				return !operator==(other);
			}

			//type conversion, depends on your need
			operator list_const_iterator<value_type>() const noexcept
			{
				return list_const_iterator<value_type>(ptr);
			}

		private:
			node_pointer ptr { nullptr };
	};

	/*
	 *	const bidirectional iterator associated with list data container
	 */
	template<class T>
	class list_const_iterator
	{
		template<class T, class Allocator>
		friend class list;

		private:
			using node_type    = list_node<T>;
			using node_pointer = node_type*;

			node_pointer& next() noexcept
			{
				return ptr->next;
			}

			node_pointer& prev() noexcept
			{
				return ptr->prev;
			}

		public:
			using iterator_category = bidirectional_iterator_tag;
			using value_type        = T;
			using difference_type   = ptrdiff_t;
			using pointer           = const value_type*;
			using reference         = const value_type&;

			list_const_iterator() noexcept = default;

			list_const_iterator(node_pointer p) noexcept : ptr { p }
			{
			}

			reference operator * () const noexcept
			{
				return ptr->value;
			}

			pointer operator -> () const noexcept
			{
				return std::addressof(**this);;
			}

			auto& operator ++ () noexcept
			{
				//some checking here

				ptr = next();

				return *this;
			}

			auto operator ++ (int) noexcept
			{
				auto tmp { *this };

				operator++();

				return tmp;
			}

			auto& operator -- () noexcept
			{
				//some checking here

				ptr = prev();

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			bool operator == (const list_const_iterator& other) const noexcept
			{
				return ptr == other.ptr;
			}

			bool operator != (const list_const_iterator& other) const noexcept
			{
				return !operator==(other);
			}

			//type conversion, depends on your need
			operator list_iterator<value_type>() const noexcept
			{
				return list_iterator<value_type>(ptr);
			}

		private:
			node_pointer ptr { nullptr };
	};
}
