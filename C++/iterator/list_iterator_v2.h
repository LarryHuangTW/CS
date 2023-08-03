#pragma once

/*
 *	The design of iterators of list data container (version 2).
 *
 *	Two class templates with hierarchical relationships.
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

		protected:
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

			operator list_const_iterator<T>() const noexcept
			{
				return list_const_iterator<T>(ptr);
			}

		protected:
			node_pointer ptr { nullptr };
	};

	/*
	 *	const bidirectional iterator associated with list data container
	 */
	template<class T>
	class list_const_iterator : public list_iterator<T>
	{
		template<class T, class Allocator>
		friend class list;

		public:
			using base_type         = list_iterator<T>;
			using value_type        = base_type::value_type;
			using difference_type   = base_type::difference_type;
			using pointer           = const value_type*;
			using reference         = const value_type&;

			using base_type::base_type;

			reference operator * () const noexcept
			{
				return this->ptr->value;
			}

			pointer operator -> () const noexcept
			{
				return std::addressof(**this);;
			}

			auto& operator ++ () noexcept
			{
				//some checking here

				base_type::operator++();

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

				base_type::operator--();

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}
	};
}
