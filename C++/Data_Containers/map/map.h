#pragma once

#include <utility>
#include <stdexcept>
#include "bbst_container.h"

namespace cust					//customized / non-standard
{
	//Function object to compare key-value pair
	template<class key_type, class value_type, class Compare = std::less<key_type>>
	struct ComPair : public Compare
	{
		using base_type = Compare;
		using pair_type = std::pair<const key_type, value_type>;

		constexpr bool operator () (const pair_type& lhs, const pair_type& rhs) const
		{
			return base_type::operator()(lhs.first, rhs.first);
		}

		constexpr bool operator () (const pair_type& lhs, const key_type& key) const
		{
			return base_type::operator()(lhs.first, key);
		}

		constexpr bool operator () (const key_type& key, const pair_type& rhs) const
		{
			return base_type::operator()(key, rhs.first);
		}
	};

	template<class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
	class map : public bbst_container<Key, std::pair<const Key, T>, ComPair<Key, T, Compare>, Allocator>
	{
		public:
			using base_type   = bbst_container<Key, std::pair<const Key, T>, ComPair<Key, T, Compare>, Allocator>;
			using key_type    = base_type::key_type;
			using value_type  = base_type::value_type;
			using mapped_type = T;
			using key_compare = Compare;
			using iterator    = base_type::iterator;

			using base_type::base_type;

			//returns the function object that compares the keys
			key_compare key_comp() const
			{
				return key_compare{};
			}

			//access a specified element with bounds checking
			mapped_type& at(const key_type& key)
			{
				auto ptr { this->bbst.find(key) };

				if (ptr == nullptr)
					throw std::out_of_range("invalid element access: no such key");

				return ptr->value.second;
			}

			const mapped_type& at(const key_type& key) const
			{
				auto ptr { this->bbst.find(key) };

				if (ptr == nullptr)
					throw std::out_of_range("invalid element access: no such key");

				return ptr->value.second;
			}

			//access or insert a specified element
			mapped_type& operator [] (const key_type& key)
			{
				return try_emplace(key).first->second;
			}

			mapped_type& operator [] (key_type&& key)
			{
				return try_emplace(std::move(key)).first->second;
			}

			//inserts in-place if the key does not exist, does nothing if the key exists
			template<class... Args>
			std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args)
			{
				auto ptr { this->bbst.find(key) };

				return ptr == nullptr ? this->bbst.insert(value_type(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...))) : std::make_pair(ptr, false);
			}

			template<class... Args>
			std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args)
			{
				auto ptr { this->bbst.find(key) };

				return ptr == nullptr ? this->bbst.insert(value_type(std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple(std::forward<Args>(args)...))) : std::make_pair(ptr, false);
			}
	};
}
