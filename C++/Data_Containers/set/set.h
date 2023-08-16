#pragma once

#include "bbst_container.h"

namespace cust					//customized / non-standard
{
	template<class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
	class set : public bbst_container<Key, Key, Compare, Allocator>
	{
		public:
			using base_type = bbst_container<Key, Key, Compare, Allocator>;

			using base_type::base_type;
	};
}
