#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <initializer_list>
#include "iterator_base.h"

namespace cust					//customized / non-standard
{
	using std::forward_iterator_tag;
	using std::size_t;
	using std::ptrdiff_t;
	using std::allocator;
	using std::allocator_traits;
	using std::initializer_list;

	/*
	 *	node data structure of forward_list data container
	 */
	template<class T>
	struct forward_list_node
	{
		using value_type   = T;
		using size_type    = size_t;
		using node_pointer = forward_list_node*;

		//adds a node by allocating memory space and calling its constructor
		template<class Allocator, class... Args>
		[[nodiscard]] static node_pointer new_one_node(Allocator& alloc, node_pointer p, Args&&... args)
		{
			auto ptr { allocator_traits<Allocator>::allocate(alloc, 1) };

			ptr->next = p;

			allocator_traits<Allocator>::construct(alloc, std::addressof(ptr->value), std::forward<Args>(args)...);

			return ptr;
		}

		//adds count copies of nodes with value
		template<class Allocator>
		[[nodiscard]] static node_pointer new_nodes(Allocator& alloc, size_type count, const value_type& value)
		{
			node_pointer ptr { nullptr };

			for ( ; count != 0; --count)
				ptr = new_one_node(alloc, ptr, value);

			return ptr;
		}

		//deletes a node by calling its destructor and deallocating memory space
		template<class Allocator>
		static node_pointer delete_one_node(Allocator& alloc, node_pointer ptr) noexcept
		{
			node_pointer ptr_next { ptr == nullptr ? nullptr : ptr->next };

			allocator_traits<Allocator>::destroy(alloc, ptr);
			allocator_traits<Allocator>::deallocate(alloc, ptr, 1);

			return ptr_next;
		}

		//deletes the node pointed by ptr and those nodes after it
		template<class Allocator>
		static void delete_nodes(Allocator& alloc, node_pointer ptr) noexcept
		{
			for ( ; ptr != nullptr; ptr = delete_one_node(alloc, ptr));
		}

		//copies nodes with the data of the range [first, last)
		template<class Allocator, class FwdIter>
		[[nodiscard]] static node_pointer copy_nodes(Allocator& alloc, FwdIter first, FwdIter last)
		{
			node_pointer head { nullptr };

			if (first != last)
			{
				head = new_one_node(alloc, nullptr, *first++);

				for (auto prev { head }; first != last; ++first)
					prev = prev->next = new_one_node(alloc, nullptr, *first);
			}

			return head;
		}

		node_pointer next  { nullptr };
		value_type   value {};
	};

	/*
	 *	unidirectional iterator associated with forward_list data container
	 */
	template<class T, template<class...> class NodeTy>
	class forward_list_iterator : public container_iterator<T, NodeTy>
	{
		//friend class forward declaration
		template<class T, class Allocator>
		friend class forward_list;

		private:
			using base_type    = container_iterator<T, NodeTy>;
			using node_pointer = typename base_type::node_pointer;
			using Ty           = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, const T>;

			node_pointer& next() const noexcept
			{
				return this->ptr->next;
			}

		public:
			using iterator_category = forward_iterator_tag;

			using base_type::base_type;

			forward_list_iterator& operator ++ () noexcept
			{
				if (this->ptr != nullptr)
					this->ptr = next();

				return *this;
			}

			forward_list_iterator operator ++ (int) noexcept
			{
				auto tmp { *this };

				operator++();

				return tmp;
			}

			//type conversion between iterator and const_iterator
			operator forward_list_iterator<Ty, NodeTy>() noexcept
			{
				return forward_list_iterator<Ty, NodeTy>(this->ptr);
			}
	};

	/*
	 *	forward_list data container
	 */
	template<class T, class Allocator = allocator<T>>
	class forward_list
	{
		private:
			//private member types
			using node_type       = forward_list_node<T>;
			using node_pointer    = node_type*;
			using allocator_type  = typename allocator_traits<Allocator>::template rebind_alloc<node_type>;

		public:
			//public member types
			using value_type      = T;
			using size_type       = size_t;
			using difference_type = ptrdiff_t;
			using reference       = value_type&;
			using const_reference = const value_type&;
			using iterator        = forward_list_iterator<value_type,       forward_list_node>;
			using const_iterator  = forward_list_iterator<const value_type, forward_list_node>;

		public:
			//default constructor
			forward_list() noexcept = default;

			//constructor with count instances of value_type
			explicit forward_list(size_type count)
			{
				head = node_type::new_nodes(alloc, count, value_type{});
			}

			//constructor with count copies of value
			forward_list(size_type count, const value_type& value)
			{
				head = node_type::new_nodes(alloc, count, value);
			}

			//copy constructor
			forward_list(const forward_list& other)
			{
				head = node_type::copy_nodes(alloc, other.begin(), other.end());
			}

			//move constructor
			forward_list(forward_list&& other) noexcept
			{
				std::swap(head, other.head);
			}

			//constructor with initializer list
			forward_list(initializer_list<value_type> initList)
			{
				head = node_type::copy_nodes(alloc, initList.begin(), initList.end());
			}

			//destructor
			~forward_list()
			{
				clear();
			}

			//copy assignment
			forward_list& operator = (const forward_list& other)
			{
				auto curr { begin() }, prev { curr };

				if (other.empty())
					clear();
				else
				{
					if (empty())
						push_front(value_type{});
				}

				prev = curr = begin();

				for (const auto& item : other)
				{
					if (curr == end())
						prev = insert_after(prev, item);
					else
					{
						*curr = item;

						prev = curr++;
					}
				}

				if (curr != end())
					prev = erase_after(prev, end());

				return *this;
			}

			//move assignment
			forward_list& operator = (forward_list&& other) noexcept
			{
				clear();

				std::swap(head, other.head);

				return *this;
			}

			//assignment operator with initializer list
			forward_list& operator = (initializer_list<value_type> initList)
			{
				if ( !empty() && initList.size() == 0 )
					clear();
				else if (empty() && initList.size() != 0)
					for (auto riter { std::rbegin(initList) }; riter != std::rend(initList); push_front(*riter), ++riter);
				else if ( !empty() && initList.size() != 0 )
				{
					auto iter { begin() }, iter_prev { iter };

					for (const auto& item : initList)
					{
						if (iter != end())
						{
							*iter = item;

							iter_prev = iter++;
						}
						else
						{
							iter_prev = insert_after(iter_prev, item);
						}
					}

					if (iter != end())
						iter_prev = erase_after(iter_prev, end());
				}

				return *this;
			}

			//checks if the container is empty
			bool empty() const noexcept
			{
				return head == nullptr;
			}

			//access the first element
			reference front()
			{
				if (empty())
					std::cerr << "tried to access an empty forward_list";

				return head->value;
			}

			//access the first element
			const_reference front() const
			{
				if (empty())
					std::cerr << "tried to access an empty forward_list";

				return head->value;
			}

			//clears all elements in the container
			void clear() noexcept
			{
				node_type::delete_nodes(alloc, head);

				head = nullptr;
			}

			//adds an element (with copy semantics) to the beginning of the container
			void push_front(const_reference value)
			{
				emplace_front(value);
			}

			//adds an element (with move semantics) to the beginning of the container
			void push_front(value_type&& value)
			{
				emplace_front(std::move(value));
			}

			//removes the first element
			void pop_front()
			{
				head = node_type::delete_one_node(alloc, head);
			}

			//adds an element (in-place) to the beginning of the container
			template<class... Args>
			reference emplace_front(Args&&... args)
			{
				head = node_type::new_one_node(alloc, head, std::forward<Args>(args)...);

				return front();
			}

			//inserts an element (with copy semantics) after the specified position
			iterator insert_after(const_iterator pos, const_reference value)
			{
				if (pos == cend())
				{
					std::cerr << "tried to insert after the end of forward_list";

					return end();
				}

				iterator iter { pos };

				iter.next() = node_type::new_one_node(alloc, iter.next(), value);

				return ++iter;
			}

			//inserts an element (with move semantics) after the specified position
			iterator insert_after(const_iterator pos, value_type&& value)
			{
				if (pos == cend())
				{
					std::cerr << "tried to insert after the end of forward_list";

					return end();
				}

				iterator iter { pos };

				iter.next() = node_type::new_one_node(alloc, iter.next(), std::move(value));

				return ++iter;
			}

			//removes an element at the specified position
			iterator erase_after(const_iterator pos)
			{
				if (pos == cend() || std::next(pos) == cend())
					return end();

				iterator iter(pos);

				iter.next() = node_type::delete_one_node(alloc, iter.next());

				return ++iter;
			}

			//removes element(s) in the specified range
			iterator erase_after(const_iterator first, const_iterator last)
			{
				iterator iter { first == last ? last : std::next(first) };

				for ( ; iter != last; iter = erase_after(first));

				return iter;
			}

			//merges two sorted forward_list
			void merge(forward_list& other)
			{
				node_pointer back1 { nullptr }, back2 { nullptr };

				for (auto ptr { head };       ptr != nullptr; back1 = ptr, ptr = ptr->next);
				for (auto ptr { other.head }; ptr != nullptr; back2 = ptr, ptr = ptr->next);

				inplace_merge(before_head(), back1, other.before_head(), back2);

				other.head = nullptr;
			}

			//merges two sorted forward_list
			void merge(forward_list&& other)
			{
				node_pointer back1 { nullptr }, back2 { nullptr };

				for (auto ptr { head };       ptr != nullptr; back1 = ptr, ptr = ptr->next);
				for (auto ptr { other.head }; ptr != nullptr; back2 = ptr, ptr = ptr->next);

				inplace_merge(before_head(), back1, other.before_head(), back2);

				other.head = nullptr;
			}

			/*
			 *	sorts the elements of the forward_list
			 *
			 *	in-place merge sort (recursive, top-down version)
			 */
			template<class Compare = std::less<>>
			void sort(Compare cmp = Compare{})
			{
				if (empty() || head->next == nullptr)
					return;

				size_t step { 2 };
				auto prev   { before_head() };
				auto back1  { mergeSort(prev, step, cmp) };

				for ( ; back1->next != nullptr; step <<= 1)
				{
					auto back2 { mergeSort(back1, step, cmp) };

					back1 = inplace_merge(prev, back1, back1, back2, cmp);
				}
			}

			//reverses the order of the elements of the forward_list
			void reverse() noexcept
			{
				node_pointer prev { nullptr };

				for (node_pointer curr { head }, next { nullptr }; curr != nullptr; )
				{
					next = curr->next;
					curr->next = prev;
					prev = curr;
					curr = next;
				}

				head = prev;
			}

			/*
			 *	Tried to simulate std::forward_list::splice_after(pos, prev_first, std::next(back))
			 *	rotates range (prev_first, back] to next of pos
			 */
			void rotate(const_iterator pos, const_iterator prev_first, const_iterator back) noexcept
			{
				if (pos == prev_first || prev_first == back)
					return;

				auto tmp { std::next(pos) };

				pos.next()        = prev_first.next();
				prev_first.next() = back.next();
				back.next()       = tmp.ptr;
			}

			iterator begin() noexcept { return iterator(head); }

			const_iterator begin() const noexcept { return const_iterator(head); }

			const_iterator cbegin() const noexcept { return begin(); }

			iterator end() noexcept { return iterator(nullptr); }

			const_iterator end() const noexcept { return const_iterator(nullptr); }

			const_iterator cend() const noexcept { return end(); }

		private:
			/*
			 *	Merges the sorted range (prev2, back2] into the sorted range (prev1, back1]
			 */
			template<class Compare = std::less<>>
			node_pointer inplace_merge(node_pointer prev1, node_pointer back1, node_pointer prev2, node_pointer back2, Compare cmp = Compare{})
			{
				if (prev1 != nullptr && back1 != nullptr && prev2 != nullptr && back2 != nullptr)
				{
					for ( ; ; )
					{
						while (prev1 != back1 && !cmp(prev2->next->value, prev1->next->value))
							prev1 = prev1->next;

						if (prev1 == back1)
						{
							if (back1 != prev2)
							{
								prev1       = prev1->next;
								back1->next = prev2->next;
								prev2->next = back2->next;
								back2->next = prev1;
							}

							back1 = back2;

							break;
						}

						auto ptr2 { prev2 };

						while (ptr2 != back2 && cmp(ptr2->next->value, prev1->next->value))
							ptr2 = ptr2->next;

						auto tmp2 { ptr2->next };

						ptr2->next  = prev1->next;
						prev1->next = prev2->next;
						prev2->next = tmp2;

						if (ptr2 == back2)
							break;

						prev1 = ptr2;
					}
				}

				return back1;
			}

			template<class Compare = std::less<>>
			node_pointer mergeSort(node_pointer prev, size_type count, Compare cmp = Compare{})
			{
				if (prev == nullptr || count == 0)
					return nullptr;

				if (count == 1)
					return prev->next;
				else if (count == 2)
				{
					if (prev->next == nullptr)
						return nullptr;
					else
					{
						auto left { prev->next }, right { left->next };

						return right == nullptr ? left : inplace_merge(prev, left, left, right, cmp);
					}
				}
				else
				{
					auto half { count >> 1 };

					auto ptr1 = mergeSort(prev, half,         cmp);
					auto ptr2 = mergeSort(ptr1, count - half, cmp);

					return inplace_merge(prev, ptr1, ptr1, ptr2, cmp);
				}
			}

			node_pointer before_head() const noexcept
			{
				return reinterpret_cast<node_pointer>(std::addressof(const_cast<node_pointer&>(head)));
			}

			node_pointer   head  { nullptr };
			allocator_type alloc {};
	};

	template<class T, class Allocator>
	bool operator == (const forward_list<T, Allocator>& lhs, const forward_list<T, Allocator>& rhs)
	{
		auto iterL { lhs.cbegin() };
		auto iterR { rhs.cbegin() };

		for ( ; iterL != lhs.cend() && iterR != rhs.cend() && *iterL == *iterR; ++iterL, ++iterR);

		return iterL == lhs.cend() && iterR == rhs.cend();
	}

	template<class T, class Allocator>
	bool operator != (const forward_list<T, Allocator>& lhs, const forward_list<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}
