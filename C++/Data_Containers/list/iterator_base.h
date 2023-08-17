#pragma once

#include <type_traits>

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

	template<class T, template<class...> class NodeTy, class NodePtr = NodeTy<std::remove_const_t<T>>*>
	class container_iterator : public iterator_base<NodePtr>
	{
		public:
			using base_type       = iterator_base<NodePtr>;
			using value_type      = T;
			using difference_type = ptrdiff_t;
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
