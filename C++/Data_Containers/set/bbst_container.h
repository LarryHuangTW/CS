#pragma once

/*
 *	balanced binary search tree (BBST) container
 */

#include "AVL_tree.h"
#include "iterator_base.h"

namespace cust					//customized / non-standard
{
	template<class T, template<class...> class NodeTy>
	class bbst_iterator : public container_iterator<std::remove_const_t<T>, NodeTy>
	{
		//friend class forward declaration
		template<class, class, class, class>
		friend class bbst_container;

		private:
			using Ty = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, const T>;

		public:
			using base_type         = container_iterator<std::remove_const_t<T>, NodeTy>;
			using iterator_category = std::bidirectional_iterator_tag;
			using node_type         = NodeTy<std::remove_const_t<T>>;

			using base_type::base_type;

			auto& operator ++ () noexcept
			{
				//some checking here

				this->ptr = node_type::successor(this->ptr);

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

				this->ptr = node_type::predecessor(this->ptr);

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			//type conversion between iterator and const_iterator
			operator bbst_iterator<Ty, NodeTy>() noexcept
			{
				return bbst_iterator<Ty, NodeTy>(this->ptr);
			}
	};

	template<class KeyTy, class ValTy, class Compare = std::less<ValTy>, class Allocator = std::allocator<ValTy>>
	class bbst_container
	{
		public:
			using key_type        = KeyTy;
			using value_type      = ValTy;
			using size_type       = std::size_t;
			using difference_type = std::ptrdiff_t;
			using key_compare     = Compare;
			using value_compare   = Compare;
			using allocator_type  = Allocator;
			using reference       = value_type&;
			using const_reference = const value_type&;
			using pointer         = value_type*;
			using const_pointer   = const value_type*;

			using iterator               = bbst_iterator<std::conditional_t<std::is_same_v<key_type, value_type>, std::add_const_t<value_type>, value_type>, AVL_tree_node>;
			using const_iterator         = bbst_iterator<const value_type, AVL_tree_node>;
			using reverse_iterator       = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		private:
			using impl_type    = AVL_tree<value_type, value_compare, allocator_type>;
			using node_type    = typename impl_type::node_type;
			using node_pointer = typename impl_type::node_pointer;

		public:
			//default constructor
			bbst_container() = default;

			//copy constructor
			bbst_container(const bbst_container& other) : bbst { other.bbst }
			{
			}

			//move constructor
			bbst_container(bbst_container&& other) noexcept : bbst { std::move(other.bbst) }
			{
			}

			//range constructor
			template<class InputIter>
			bbst_container(InputIter first, InputIter last) : bbst(first, last)
			{
			}

			//constructor with initializer list
			bbst_container(std::initializer_list<value_type> init) : bbst { init }
			{
			}

			//destructor
			~bbst_container()
			{
			}

			//copy assignment
			bbst_container& operator = (const bbst_container& other)
			{
				bbst = other.bbst;

				return *this;
			}

			//move assignment
			bbst_container& operator = (bbst_container&& other) noexcept
			{
				bbst = std::move(other.bbst);

				return *this;
			}

			//assignment with initializer list
			bbst_container& operator = (std::initializer_list<value_type> init)
			{
				bbst = init;

				return *this;
			}

			//returns the allocator associated with the container
			allocator_type get_allocator() const noexcept
			{
				return bbst.get_allocator();
			}

			//returns the function object that compares the keys
			key_compare key_comp() const
			{
				return bbst.get_compare();
			}

			//returns the function object that compares objects of value_type
			value_compare value_comp() const
			{
				return bbst.get_compare();
			}

			//checks if the container is empty
			bool empty() const noexcept
			{
				return bbst.empty();
			}

			//gets the number of elements of the container
			size_type size() const noexcept
			{
				return bbst.size();
			}

			//clears all elements of the container
			void clear() noexcept
			{
				bbst.clear();
			}

			//inserts an element into the container (with copy semantics)
			std::pair<iterator, bool> insert(const value_type& value)
			{
				auto pr { bbst.insert(value) };

				return { pr.first , pr.second };
			}

			//inserts an element into the container (with move semantics)
			std::pair<iterator, bool> insert(value_type&& value)
			{
				auto pr { bbst.insert(std::move(value)) };

				return { pr.first , pr.second };
			}

			//inserts a range of elements [first, last) into the container
			template<class InputIter>
			void insert(InputIter first, InputIter last)
			{
				bbst.insert(first, last);
			}

			//inserts the elements of the initializer list into the container
			void insert(std::initializer_list<value_type> init)
			{
				bbst.insert(init);
			}

			//inserts an element into the container with its value constructed in-place
			template<class... Args>
			std::pair<iterator, bool> emplace(Args&&... args)
			{
				auto pr { bbst.emplace(std::forward<Args>(args)...) };

				return { (pr.first) , pr.second };
			}

			//removes specified elements from the container
			iterator erase(const_iterator pos)
			{
				return pos == cend() ? pos : bbst.erase(pos.ptr);
			}

			size_type erase(const key_type& key)
			{
				auto ptr { bbst.find(key) };

				bbst.erase(ptr);

				return ptr == nullptr ? 0 : 1;
			}

			//exchanges the contents of the container with those of other
			void swap(bbst_container& other) noexcept
			{
				bbst.swap(other.bbst);
			}

			//returns the number of elements with the same key
			size_type count(const key_type& key) const noexcept
			{
				return find(key) == end() ? 0 : 1;
			}

			//finds the element with the same key
			iterator find(const key_type& key)
			{
				auto ptr { bbst.find(key) };

				return ptr == nullptr ? end() : ptr;
			}

			const_iterator find(const key_type& key) const
			{
				auto ptr { bbst.find(key) };

				return ptr == nullptr ? cend() : ptr;
			}

			//checks if there is an element with the same key
			bool contains(const key_type& key) const
			{
				return 0 < count(key);
			}

			//finds the first element with its key that is not less than the function argument
			iterator lower_bound(const key_type& key)
			{
				auto ptr { bbst.lower_bound(key) };

				return ptr == nullptr ? end() : ptr;
			}

			const_iterator lower_bound(const key_type& key)	const
			{
				auto ptr { bbst.lower_bound(key) };

				return ptr == nullptr ? cend() : ptr;
			}

			//finds the first element with its key that is greater than the function argument
			iterator upper_bound(const key_type& key)
			{
				auto ptr { bbst.upper_bound(key) };

				return ptr == nullptr ? end() : ptr;
			}

			const_iterator upper_bound(const key_type& key)	const
			{
				auto ptr { bbst.upper_bound(key) };

				return ptr == nullptr ? cend() : ptr;
			}

			//returns an iterator to the beginning
			iterator begin() noexcept
			{
				return bbst.min();
			}

			//returns a const iterator to the beginning
			const_iterator begin() const noexcept
			{
				return bbst.min();
			}

			const_iterator cbegin() const noexcept
			{
				return begin();
			}

			//returns an iterator to the end
			iterator end() noexcept
			{
				return bbst.successor(bbst.max());
			}

			//returns a const iterator to the end
			const_iterator end() const noexcept
			{
				return bbst.successor(bbst.max());
			}

			const_iterator cend() const noexcept
			{
				return end();
			}

			//returns a reverse iterator to the beginning
			reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

			//returns a const reverse iterator to the beginning
			const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

			const_reverse_iterator crbegin() const noexcept { return rbegin(); }

			//returns a reverse iterator to the end
			reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

			//returns a const reverse iterator to the end
			const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

			const_reverse_iterator crend() const noexcept { return rend(); }

		protected:
			impl_type bbst {};
	};

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator == (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator != (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator < (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator <= (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return !(rhs < lhs);
	}

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator > (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return rhs < lhs;
	}

	template<class KeyTy, class ValTy, class Compare, class Allocator>
	bool operator >= (const bbst_container<KeyTy, ValTy, Compare, Allocator>& lhs, const bbst_container<KeyTy, ValTy, Compare, Allocator>& rhs)
	{
		return !(lhs < rhs);
	}
}
