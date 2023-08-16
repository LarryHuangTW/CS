#pragma once

#include <cstddef>

namespace cust					//customized / non-standard
{
	template<class node_type>
	class iterator_base
	{
		public:
			using node_pointer = node_type*;

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

	template<class T, template<class...> class node_type>
	class container_iterator : public iterator_base<node_type<T>>
	{
		public:
			using base_type       = iterator_base<node_type<T>>;
			using value_type      = T;
			using difference_type = std::ptrdiff_t;
			using pointer         = value_type*;
			using reference       = value_type&;

			using base_type::base_type;

			reference operator * () const noexcept
			{
				return this->ptr->value;
			}

			pointer operator -> () const noexcept
			{
				return std::addressof(**this);
			}
	};
}
