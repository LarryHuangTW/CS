#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <utility>
#include <initializer_list>
#include <algorithm>
#include "iterator_base.h"

namespace cust					//customized / non-standard
{
	using std::bidirectional_iterator_tag;
	using std::size_t;
	using std::ptrdiff_t;
	using std::allocator;
	using std::allocator_traits;
	using std::initializer_list;

	/*
	 *	node data structure of list data container
	 */
	template<class T>
	struct list_node
	{
		using value_type   = T;
		using size_type    = size_t;
		using node_pointer = list_node*;

		//adds a node by allocating memory space and calling its constructor
		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_node(Allocator& alloc, Args&&... args)
		{
			auto ptr { allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->prev = ptr->next = nullptr;

			allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_one_node(Allocator& alloc, node_pointer next, node_pointer prev, Args&&... args)
		{
			auto ptr { allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->next = next;
			ptr->prev = prev;

			allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		//adds count copies of nodes with value
		template<class Allocator>
		[[nodiscard]] static auto new_nodes(Allocator& alloc, size_type count, const value_type& value)
		{
			node_pointer head { nullptr }, tail { nullptr };

			if (count != 0)
			{
				tail = head = new_node(alloc, value);

				for (auto prev { head }; --count != 0; )
				{
					tail = new_one_node(alloc, nullptr, prev, value);

					prev = prev->next = tail;
				}
			}

			return std::make_pair(head, tail);
		}

		//deletes a node by calling its destructor and deallocating memory space
		template<class Allocator>
		static auto delete_one_node(Allocator& alloc, node_pointer ptr) noexcept
		{
			std::pair<node_pointer, node_pointer> pr { nullptr , nullptr };

			if (ptr != nullptr)
			{
				pr.first  = ptr->next;
				pr.second = ptr->prev;

				allocator_traits<Allocator>::destroy(alloc, ptr);
				allocator_traits<Allocator>::deallocate(alloc, ptr, 1);
			}

			return pr;
		}

		//deletes the node pointed by ptr and those nodes after it and before end
		template<class Allocator>
		static void delete_nodes(Allocator& alloc, node_pointer ptr, node_pointer end) noexcept
		{
			for ( ; ptr != nullptr && ptr != end; ptr = delete_one_node(alloc, ptr).first);
		}

		//copies nodes with the data of the range [first, last)
		template<class Allocator, class FwdIter>
		[[nodiscard]] static auto copy_nodes(Allocator& alloc, FwdIter first, FwdIter last)
		{
			node_pointer head { nullptr }, tail { nullptr };

			if (first != last)
			{
				tail = head = new_node(alloc, *first);

				for (auto prev { head }; ++first != last; )
				{
					tail = new_one_node(alloc, nullptr, prev, *first);

					prev = prev->next = tail;
				}
			}

			return std::make_pair(head, tail);
		}

		node_pointer next  { nullptr };			//pointer pointing to next     node
		node_pointer prev  { nullptr };			//pointer pointing to previous node
		value_type   value {};				//data value
	};

	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<class T, template<class...> class NodeTy>
	class list_iterator : public container_iterator<T, NodeTy>
	{
		//friend class forward declaration
		template<class T, class Allocator>
		friend class list;

		private:
			using base_type    = container_iterator<T, NodeTy>;
			using node_pointer = typename base_type::node_pointer;
			using Ty           = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, const T>;

			node_pointer& next() const noexcept
			{
				return this->ptr->next;
			}

			node_pointer& prev() const noexcept
			{
				return this->ptr->prev;
			}

		public:
			using iterator_category = bidirectional_iterator_tag;

			using base_type::base_type;

			list_iterator& operator ++ () noexcept
			{
				if (this->ptr != nullptr)
					this->ptr = next();

				return *this;
			}

			list_iterator operator ++ (int) noexcept
			{
				auto tmp { *this };

				operator++();

				return tmp;
			}

			list_iterator& operator -- () noexcept
			{
				if (this->ptr != nullptr)
					this->ptr = prev();

				return *this;
			}

			list_iterator operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			//type conversion between iterator and const_iterator
			operator list_iterator<Ty, NodeTy>() noexcept
			{
				return list_iterator<Ty, NodeTy>(this->ptr);
			}
	};

	/*
	 *	list data container (with doubly-linked list data structure)
	 */
	template<class T, class Allocator = allocator<T>>
	class list
	{
		private:
			//private member types
			using node_type           = list_node<T>;
			using node_pointer        = node_type*;
			using node_allocator_type = typename allocator_traits<Allocator>::template rebind_alloc<node_type>;

		public:
			//public member types
			using value_type             = T;
			using allocator_type         = Allocator;
			using size_type              = size_t;
			using difference_type        = ptrdiff_t;
			using reference              = value_type&;
			using const_reference        = const reference;
			using pointer                = typename allocator_traits<allocator_type>::pointer;
			using const_pointer          = typename allocator_traits<allocator_type>::const_pointer;
			using iterator               = list_iterator<value_type,       list_node>;
			using const_iterator         = list_iterator<const value_type, list_node>;
			using reverse_iterator       = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		public:
			//default constructor
			list()
			{
				pseudo_head = node_type::new_node(alloc, value_type{});
			}

			//constructor with count default values
			explicit list(size_type count)
			{
				pseudo_head = node_type::new_node(alloc, value_type{});

				if (count != 0)
				{
					auto pair { node_type::new_nodes(alloc, count, value_type{}) };

					pseudo_head->next = pair.first;
					pseudo_head->prev = pair.second;

					pair.second->next = pair.first->prev = pseudo_head;
				}

				sz = count;
			}

			//constructor with count copies of value
			explicit list(size_type count, const value_type& value)
			{
				pseudo_head = node_type::new_node(alloc, value_type{});

				if (count != 0)
				{
					auto pair { node_type::new_nodes(alloc, count, value) };

					pseudo_head->next = pair.first;
					pseudo_head->prev = pair.second;

					pair.second->next = pair.first->prev = pseudo_head;
				}

				sz = count;
			}

			//copy constructor
			list(const list& other)
			{
				pseudo_head = node_type::new_node(alloc, value_type{});

				if ( !other.empty() )
				{
					auto pair { node_type::copy_nodes(alloc, other.begin(), other.end()) };

					pseudo_head->next = pair.first;
					pseudo_head->prev = pair.second;

					pair.second->next = pair.first->prev = pseudo_head;
				}

				sz = other.size();
			}

			//move constructor
			list(list&& other) noexcept
			{
				swap(other);
			}

			//constructor with initializer list
			list(initializer_list<value_type> initList)
			{
				pseudo_head = node_type::new_node(alloc, value_type{});

				if (initList.size() != 0)
				{
					auto pair { node_type::copy_nodes(alloc, initList.begin(), initList.end()) };

					pseudo_head->next = pair.first;
					pseudo_head->prev = pair.second;

					pair.second->next = pair.first->prev = pseudo_head;
				}
				
				sz = initList.size();
			}

			//destructor
			~list()
			{
				clear();
			}

			//copy assignment
			list& operator = (const list& other)
			{
				auto iter { begin() };

				for (const auto& item : other)
				{
					if (iter == end())
						push_back(item);
					else
						*iter++ = item;
				}

				if (iter != end())
					erase(iter, end());

				sz = other.sz;

				return *this;
			}

			//move assignment
			list& operator = (list&& other) noexcept
			{
				clear();

				swap(other);

				return *this;
			}

			//assignment operator with initializer list
			list& operator = (initializer_list<value_type> initList)
			{
				auto iter { begin() };

				for (const auto& item : initList)
				{
					if (iter == end())
						push_back(item);
					else
						*iter++ = item;
				}

				if (iter != end())
					erase(iter, end());

				sz = initList.size();

				return *this;
			}

			//returns the number of elements of the container
			size_type size() const noexcept
			{
				return sz;
			}

			//checks if the container is empty
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//clears all elements of the container
			void clear() noexcept
			{
				node_type::delete_nodes(alloc, pseudo_head->next, pseudo_head);

				pseudo_head->prev = pseudo_head->next = nullptr;

				sz = 0;
			}

			//accesses the first element of the container
			reference front()
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->next->value;
			}

			//accesses the first element of the container
			const_reference front() const
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->next->value;
			}

			//accesses the last element of the container
			reference back()
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->prev->value;
			}

			//accesses the last element of the container
			const_reference back() const
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->prev->value;
			}

			//adds an element (with copy semantics) to the beginning of the container
			void push_front(const value_type& value)
			{
				emplace_front(value);
			}

			//adds an element (with move semantics) to the beginning of the container
			void push_front(value_type&& value)
			{
				emplace_front(std::move(value));
			}

			//appends an element (with copy semantics) to the end of the container
			void push_back(const value_type& value)
			{
				emplace_back(value);
			}

			//appends an element (with move semantics) to the end of the container
			void push_back(value_type&& value)
			{
				emplace_back(std::move(value));
			}

			//adds an element (in-place) to the beginning of the container
			template<class... Args>
			reference emplace_front(Args&&... args)
			{
				auto ptr { node_type::new_one_node(alloc, empty() ? pseudo_head : pseudo_head->next, pseudo_head, std::forward<Args>(args)...) };

				++sz;

				ptr->next->prev = pseudo_head->next = ptr;

				return front();
			}

			//appends an element (in-place) to the end of the container
			template<class... Args>
			reference emplace_back(Args&&... args)
			{
				auto ptr { node_type::new_one_node(alloc, pseudo_head, empty() ? pseudo_head : pseudo_head->prev, std::forward<Args>(args)...) };

				++sz;

				ptr->prev->next = pseudo_head->prev = ptr;

				return back();
			}

			//removes the first element of the container
			void pop_front()
			{
				if (empty())
				{
					std::cerr << "tried to pop front an empty list";

					return;
				}

				auto ptr { node_type::delete_one_node(alloc, pseudo_head->next).first };

				--sz;

				pseudo_head->next = (empty() ? nullptr : ptr);

				if (empty())
					pseudo_head->prev = nullptr;
				else
					pseudo_head->next->prev = pseudo_head;
			}

			//removes the last element of the container
			void pop_back()
			{
				if (empty())
				{
					std::cerr << "tried to pop back an empty list";

					return;
				}

				auto ptr { node_type::delete_one_node(alloc, pseudo_head->prev).second };

				--sz;

				pseudo_head->prev = (empty() ? nullptr : ptr);

				if (empty())
					pseudo_head->next = nullptr;
				else
					pseudo_head->prev->next = pseudo_head;
			}

			//erases an element at the specified position
			iterator erase(const_iterator pos)
			{
				if (pos != cend())
				{
					auto pr { node_type::delete_one_node(alloc, pos.ptr) };

					--sz;

					if (empty())
					{
						pseudo_head->prev = pseudo_head->next = nullptr;
					}
					else
					{
						pr.first->prev  = pr.second;
						pr.second->next = pr.first;

						return iterator(pr.first);
					}
				}

				return end();
			}

			//erases elements in the specified range [first, last)
			iterator erase(const_iterator first, const_iterator last)
			{
				for ( ; first != cend() && first != last; )
					first = erase(first);

				return last;
			}

			//swaps all elements with other list
			void swap(list& other) noexcept
			{
				std::swap(pseudo_head, other.pseudo_head);
				std::swap(sz, other.sz);
			}

			//merges two sorted lists
			void merge(list& other)
			{
				merge_other(other);
			}

			//merges two sorted lists
			void merge(list&& other)
			{
				merge_other(other);
			}

			/*
			 *	sorts the elements of the list
			 *
			 *	in-place merge sort (recursive, top-down version)
			 */
			template<class Compare = std::less<>>
			void sort(Compare cmp = Compare{})
			{
				size_type step { 2 };

				if (empty())
					return;

				for (auto back1 { merge_sort(pseudo_head, step, cmp) }; back1->next != pseudo_head; step <<= 1)
				{
					auto back2 { merge_sort(back1, step, cmp) };

					back1 = merge_self(pseudo_head, back1, back2, cmp);
				}
			}

			//reverses the order of the elements of the list
			void reverse() noexcept
			{
				auto ptr { pseudo_head };

				for (auto cnt { size() + 1 }; cnt != 0; --cnt)
				{
					std::swap(ptr->next, ptr->prev);

					ptr = ptr->prev;
				}
			}

			/*
			 *	Tried to simulate std::list::splice(pos, *this, first, last)
			 *	rotates range [first, last) to next of pos
			 */
			void rotate(const_iterator pos, const_iterator first, const_iterator last) noexcept
			{
				if (pos == first || std::next(pos) == first || first == last)
					return;

				auto tmp { std::next(pos) };

				pos.next()         = first.ptr;
				first.prev()->next = last.ptr;
				last.prev()->next  = tmp.ptr;

				tmp.prev()   = last.prev();
				last.prev()  = first.prev();
				first.prev() = pos.ptr;
			}

			iterator begin() noexcept { return empty() ? pseudo_head : pseudo_head->next; }

			const_iterator begin() const noexcept { return empty() ? pseudo_head : pseudo_head->next; }

			const_iterator cbegin() const noexcept { return begin(); }

			iterator end() noexcept { return pseudo_head; }

			const_iterator end() const noexcept { return pseudo_head; }

			const_iterator cend() const noexcept { return end(); }

			reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

			const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

			const_reverse_iterator crbegin() const noexcept { return rbegin(); }

			reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

			const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

			const_reverse_iterator crend() const noexcept { return rend(); }

		private:
			//merges other sorted list
			template<class Compare = std::less<>>
			void merge_other(list& other, Compare cmp = Compare{})
			{
				node_type* const prev2 { other.pseudo_head };

				if (empty())
					swap(other);

				if (other.empty())
					return;

				for (node_pointer prev1 { pseudo_head }, ptr { nullptr }, tmp { nullptr }; ; prev1 = ptr)
				{
					for ( ; prev1->next != pseudo_head && !cmp(prev2->next->value, prev1->next->value); prev1 = prev1->next);

					if (prev1->next == pseudo_head)
					{
						prev1->next = prev2->next;
						prev2->next->prev = prev1;

						prev2->prev->next = pseudo_head;
						pseudo_head->prev = prev2->prev;

						break;
					}

					for (ptr = prev2; ptr->next != prev2 && cmp(ptr->next->value, prev1->next->value); ptr = ptr->next);

					tmp = ptr->next;

					ptr->next = prev1->next;
					ptr->next->prev = ptr;

					prev1->next = prev2->next;
					prev1->next->prev = prev1;

					if (tmp == prev2)
						break;

					prev2->next = tmp;
					tmp->prev   = prev2;
				}

				prev2->prev = prev2->next = nullptr;

				sz += other.sz;
				other.sz = 0;
			}

			/*
			 *	merges two consecutiveand sorted ranges of elements of the same list
			 *
			 *	(prev, back1]
			 *	      (back1, back2]
			 */
			template<class Compare = std::less<>>
			node_pointer merge_self(node_pointer prev, node_pointer back1, node_pointer back2, Compare cmp = Compare{})
			{
				for (node_pointer ptr { nullptr }, tmp { nullptr }; back2 != ptr; prev = ptr)
				{
					for ( ; prev != back1 && !cmp(back1->next->value, prev->next->value); prev = prev->next);

					if (prev == back1)
					{
						back1 = back2;

						break;
					}

					for (ptr = back1; ptr != back2 && cmp(ptr->next->value, prev->next->value); ptr = ptr->next);

					tmp = ptr->next;

					ptr->next = prev->next;
					ptr->next->prev = ptr;

					prev->next = back1->next;
					prev->next->prev = prev;

					back1->next = tmp;
					tmp->prev   = back1;
				}

				return back1;
			}

			//merge sort
			template<class Compare = std::less<>>
			node_pointer merge_sort(node_pointer prev, size_type count, Compare cmp = Compare{})
			{
				if (prev == nullptr)
					return nullptr;

				if (count == 1)
					return prev->next == pseudo_head ? nullptr : prev->next;

				auto half  { count >> 1 };
				auto back1 { merge_sort(prev,  half,         cmp) };
				auto back2 { merge_sort(back1, count - half, cmp) };

				return merge_self(prev, back1, back2, cmp);
			}

			node_pointer        pseudo_head { nullptr };
			size_type           sz          { 0 };
			node_allocator_type alloc       {};
	};

	template<class T, class Allocator>
	bool operator == (const list<T, Allocator>& lhs, const list<T, Allocator>& rhs)
	{
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<class T, class Allocator>
	bool operator != (const list<T, Allocator>& lhs, const list<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}
