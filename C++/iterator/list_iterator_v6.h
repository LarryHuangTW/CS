#pragma once

/*
 *	The design of iterators of list data container (version 6).
 *
 *	Three class templates with hierarchical relationships.
 */

#include <iterator>
#include "iterator_base.h"

namespace cust					//customized / non-standard
{
	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<class T, template<class...> class NodeTy>
	class list_iterator : public container_iterator<T, NodeTy>
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
			using base_type         = container_iterator<T, NodeTy>;
			using iterator_category = std::bidirectional_iterator_tag;

			using base_type::base_type;

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
