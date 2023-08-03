#pragma once

/*
 *	The design of iterators of list data container (version 3).
 *
 *	Use one class template to implement iterator and const_iterator.
 */

#include <iterator>
#include <cstddef>

namespace cust					//customized / non-standard
{
	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<bool const_flag, class node_type>
	class list_iterator
	{
		template<class T, class Allocator>
		friend class list;

		private:
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
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type        = typename node_type::value_type;
			using difference_type   = std::ptrdiff_t;
			using pointer           = std::conditional_t<const_flag, const value_type*, value_type*>;
			using reference         = std::conditional_t<const_flag, const value_type&, value_type&>;

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

			//type conversion between iterator and const_iterator
			operator list_iterator<!const_flag, node_type>() noexcept
			{
				return list_iterator<!const_flag, node_type>(ptr);
			}

		private:
			node_pointer ptr { nullptr };
	};
}
