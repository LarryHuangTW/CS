#pragma once

/*
 *	The design of iterators of list data container (version 4).
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
	template<class T, template<class...> class NodeTy>
	class list_iterator
	{
		template<class T, class Allocator>
		friend class list;

		private:
			using node_type    = NodeTy<std::remove_const_t<T>>;
			using node_pointer = node_type*;
			using Ty           = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, const T>;

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
			/*
			 *	from this version, value type may differ from previous versions or 
			 *	even C++ standard, but it makes sense to some extent.
			 */
			using value_type        = T;
			using difference_type   = std::ptrdiff_t;
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

			//type conversion between iterator and const_iterator
			operator list_iterator<Ty, NodeTy>() noexcept
			{
				return list_iterator<Ty, NodeTy>(ptr);
			}

		private:
			node_pointer ptr { nullptr };
	};
}
