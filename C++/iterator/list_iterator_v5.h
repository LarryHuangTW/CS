#pragma once

/*
 *	The design of iterators of list data container (version 5).
 *
 *	Two class templates with hierarchical relationships.
 */

#include <iterator>
#include <cstddef>

namespace cust					//customized / non-standard
{
	template<class NodePtr>
	class iterator_base
	{
		public:
			using node_pointer = NodePtr;

			iterator_base() noexcept = default;

			iterator_base(node_pointer p) : ptr { p }
			{
			}

			bool operator == (const iterator_base& other) const noexcept
			{
				return ptr == other.ptr;
			}

			bool operator != (const iterator_base& other) const noexcept
			{
				return !operator==(other);
			}

		protected:
			node_pointer ptr { nullptr };
	};

	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<class T, template<class...> class NodeTy, class NodePtr = NodeTy<std::remove_const_t<T>>*>
	class list_iterator : public iterator_base<NodePtr>
	{
		template<class T, class Allocator>
		friend class list;

		private:
			using Ty = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, const T>;

			//returns a reference of pointer pointing to current node
			auto& curr() noexcept
			{
				return this->ptr;
			}

			//returns a reference of pointer pointing to next node
			auto& next() noexcept
			{
				return this->ptr->next;
			}

			//returns a reference of pointer pointing to previous node
			auto& prev() noexcept
			{
				return this->ptr->prev;
			}

		public:
			using base_type         = iterator_base<NodePtr>;
			using iterator_category = std::bidirectional_iterator_tag;
			/*
			 *	value type of version 4 and 5 may differ from previous versions or 
			 *	even C++ standard, but it makes sense to some extent.
			 */
			using value_type        = T;
			using difference_type   = std::ptrdiff_t;
			using pointer           = value_type*;
			using reference         = value_type&;

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

				curr() = next();

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

				curr() = prev();

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			//type conversion between iterator and const_iterator
			operator list_iterator<Ty, NodeTy>() noexcept
			{
				return list_iterator<Ty, NodeTy>(curr());
			}
	};
}
