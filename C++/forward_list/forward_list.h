#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <forward_list>
#include <initializer_list>
#include "Container.h"

namespace cust
{
	using std::forward_iterator_tag;
	using std::size_t;
	using std::ptrdiff_t;
	using std::allocator;
	using std::allocator_traits;
	using std::initializer_list;

	enum forward_list_link { next = 0 };

	template<class T, class Allocator>
	class forward_list;

	template<class T>
	class forward_list_iterator;

	/*
	 *	const forward iterator associated with forward_list data container
	 */
	template<class T>
	class forward_list_const_iterator
	{
		template<class T, class Allocator>
		friend class cust::forward_list;

		protected:
			using node_type    = Container::NodeBase<T>;
			using node_pointer = node_type*;

		public:
			using iterator_category = forward_iterator_tag;
			using value_type        = T;
			using difference_type   = ptrdiff_t;
			using pointer           = node_pointer;
			using const_pointer     = const node_type*;
			using reference         = const value_type&;

			forward_list_const_iterator() noexcept = default;

			forward_list_const_iterator(pointer p) noexcept : ptr { p }
			{
			}

			reference operator * () const noexcept { return ptr->value; }

			const_pointer operator -> () const noexcept { return ptr; }

			forward_list_const_iterator& operator ++ () noexcept
			{
				if (ptr != nullptr)
					ptr = ptr->link[forward_list_link::next];

				return *this;
			}

			forward_list_const_iterator operator ++ (int) noexcept
			{
				forward_list_const_iterator tmp = *this;

				operator++();

				return tmp;
			}

			bool operator == (forward_list_const_iterator other) const noexcept
			{
				return this->ptr == other.ptr ? true : false;
			}

			bool operator != (forward_list_const_iterator other) const noexcept
			{
				return !this->operator==(other);
			}

			operator forward_list_iterator<T>() const
			{
				return forward_list_iterator<T>(ptr);
			}

		protected:
			pointer ptr { nullptr };
	};

	/*
	 *	forward iterator associated with forward_list data container
	 */
	template<class T>
	class forward_list_iterator : public forward_list_const_iterator<T>
	{
		template<class T, class AllocTy>
		friend class cust::forward_list;

		public:
			using base_type = forward_list_const_iterator<T>;

			using iterator_category = forward_iterator_tag;
			using value_type        = T;
			using difference_type   = ptrdiff_t;
			using pointer           = base_type::node_type*;
			using reference         = value_type&;

			using base_type::base_type;

			reference operator * () const noexcept { return this->ptr->value; }

			pointer operator -> () const noexcept { return this->ptr; }

			forward_list_iterator& operator ++ () noexcept
			{
				base_type::operator++();

				return *this;
			}

			forward_list_iterator operator ++ (int) noexcept
			{
				forward_list_iterator tmp = *this;

				operator++();

				return tmp;
			}
	};

	/*
	 *	forward_list data container
	 */
	template<class T, class Allocator = allocator<T>>
	class forward_list
	{
		public:
			using value_type      = T;
			using size_type       = size_t;
			using difference_type = ptrdiff_t;
			using reference       = value_type&;
			using const_reference = const value_type&;
			using iterator        = forward_list_iterator<value_type>;
			using const_iterator  = forward_list_const_iterator<value_type>;

		private:
			using node_type       = Container::NodeBase<value_type>;
			using node_pointer    = node_type*;
			using allocator_type  = allocator_traits<Allocator>::template rebind_alloc<node_type>;

		public:
			//default constructor
			forward_list() noexcept {}

			//constructor
			explicit forward_list(size_type count)
			{
				head = new_nodes(count);
			}

			//constructor
			forward_list(size_type count, const value_type& value)
			{
				head = new_nodes(count, value);
			}

			//copy constructor
			forward_list(const forward_list& other)
			{
				head = copy_new_nodes(other.begin(), other.end());
			}

			forward_list(const std::forward_list<value_type>& std_flist)
			{
				head = copy_new_nodes(std_flist.begin(), std_flist.end());
			}

			//move constructor
			forward_list(forward_list&& other)
			{
				if (!other.empty())
				{
					auto iter{ other.begin() };

					head = new_one_node(nullptr, std::move(*iter++));

					for (auto prev{ head }; iter != other.end(); ++iter)
					{
						auto ptr = new_one_node(nullptr, std::move(*iter));

						prev->next() = ptr;
						prev = ptr;
					}
				}
			}

			forward_list(std::forward_list<value_type>&& std_flist)
			{
				if ( !std_flist.empty() )
				{
					auto iter = std_flist.begin();

					head = new_one_node(nullptr, std::move(*iter++));

					for (auto prev = this->head; iter != std_flist.end(); ++iter)
					{
						auto ptr = new_one_node(nullptr, std::move(*iter));

						prev->next() = ptr;
						prev = ptr;
					}
				}
			}

			//constructor with initializer list
			forward_list(initializer_list<value_type> initList)
			{
				head = copy_new_nodes(initList.begin(), initList.end());
			}

			//destructor
			~forward_list()
			{
				clear();
			}

			//copy assignment
			forward_list& operator = (const forward_list& other)
			{
				auto iter { begin() }, iter_prev { iter };

				for (const auto& item : other)
				{
					if (iter != end())
					{
						*iter = item;

						iter_prev = iter++;
					}
					else
					{
						if (iter == iter_prev)
						{
							push_front(item);

							iter_prev = begin();
						}
						else
						{
							iter_prev = insert_after(iter_prev, item);
						}
					}
				}

				if (iter != end())
				{
					iter_prev = erase_after(iter_prev, end());

					if (iter == begin())
						pop_front();
				}

				return *this;
			}

			//move assignment
			forward_list& operator = (forward_list&& other)
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

			//clear all elements in the container
			void clear() noexcept
			{
				delete_nodes(head);

				head = nullptr;
			}

			//add an element to the beginning
			void push_front(const_reference value)
			{
				head = new_one_node(head, value);
			}

			//add an element to the beginning
			void push_front(T&& value)
			{
				head = new_one_node(head, std::forward<T>(value));
			}

			//remove the first element
			void pop_front()
			{
				head = delete_one_node(head);
			}

			//insert an element after the specified position
			iterator insert_after(const_iterator pos, const_reference value)
			{
				if (pos == cend())
				{
					std::cerr << "tried to insert after the end of forward_list";

					return this->end();
				}

				iterator iter(pos);

				iter->next() = new_one_node(iter->next(), value);

				return ++iter;
			}

			//insert an element after the specified position
			iterator insert_after(const_iterator pos, T&& value)
			{
				if (pos == cend())
				{
					std::cerr << "tried to insert after the end of forward_list";

					return this->end();
				}

				iterator iter { pos };

				iter->next() = new_one_node(iter->next(), std::move(value));

				return ++iter;
			}

			//remove an element at the specified position
			iterator erase_after(const_iterator pos)
			{
				if (pos == cend() || std::next(pos) == cend())
					return end();

				iterator iter(pos);

				iter->next() = delete_one_node(iter->next());

				return ++iter;
			}

			//remove element(s) in the specified range
			iterator erase_after(const_iterator first, const_iterator last)
			{
				iterator iter { first == last ? last : std::next(first) };

				for ( ; iter != last; iter = erase_after(first));

				return iter;
			}

			/*
			 *	Merge the sorted range (prev2, back2] into the sorted range (prev1, back1]
			 */
			template<class Compare = std::less<>>
			node_pointer inplace_merge(node_pointer prev1, node_pointer back1, node_pointer prev2, node_pointer back2, Compare cmp = Compare{})
			{
				if (prev1 != nullptr && back1 != nullptr && prev2 != nullptr && back2 != nullptr)
				{
					for ( ; ; )
					{
						while (prev1 != back1 && !cmp(prev2->next()->value, prev1->next()->value))
						{
							prev1 = prev1->next();
						}

						if (prev1 == back1)
						{
							if (back1 != prev2)
							{
								prev1         = prev1->next();
								back1->next() = prev2->next();
								prev2->next() = back2->next();
								back2->next() = prev1;
							}

							back1 = back2;

							break;
						}

						auto ptr2 { prev2 };

						while (ptr2 != back2 && cmp(ptr2->next()->value, prev1->next()->value))
						{
							ptr2 = ptr2->next();
						}

						auto tmp2 { ptr2->next() };

						ptr2->next()  = prev1->next();
						prev1->next() = prev2->next();
						prev2->next() = tmp2;

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
				if ( prev == nullptr || count == 0)
					return nullptr;

				if      (count == 1)
					return prev->next();
				else if (count == 2)
				{
					if (prev->next() == nullptr)
						return nullptr;
					else
					{
						auto left  { prev->next() }, right { left->next() };

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

			template<class Compare = std::less<>>
			void sort(Compare cmp = Compare{})			//Inplace merge sort (recursive , top-down version)
			{
				if (this->empty() || this->head->next() == nullptr)
					return;

				size_t step { 2 };
				auto prev   { before_head() };
				auto back1  { mergeSort(prev, step, cmp) };

				for ( ; back1->next() != nullptr; step <<= 1)
				{
					auto back2 { mergeSort(back1, step, cmp) };

					back1 = inplace_merge(prev, back1, back1, back2, cmp);
				}
			}

			void reverse() noexcept
			{
				node_pointer prev { nullptr };

				for (node_pointer curr { head }, next { nullptr }; curr != nullptr; )
				{
					next = curr->next();
					curr->next() = prev;
					prev = curr;
					curr = next;
				}

				head = prev;
			}

			/*
			 *	Try to simulate std::forward_list::splice_after(pos, prev_first, std::next(back))
			 *	rotate range (prev_first, back] to next of pos
			 */
			void rotate(const_iterator pos, const_iterator prev_first, const_iterator back) noexcept
			{
				if (pos == prev_first || prev_first == back)
					return;

				auto tmp { std::next(pos) };

				pos.ptr->next()        = prev_first.ptr->next();
				prev_first.ptr->next() = back.ptr->next();
				back.ptr->next()       = tmp.ptr;
			}

			iterator begin() noexcept { return iterator(head); }

			const_iterator begin() const noexcept { return const_iterator(head); }

			const_iterator cbegin() const noexcept { return begin(); }

			iterator end() noexcept { return iterator(nullptr); }

			const_iterator end() const noexcept { return const_iterator(nullptr); }

			const_iterator cend() const noexcept { return end(); }

			node_pointer before_head() const noexcept
			{
				node_type tmp_node{};

				auto diff = reinterpret_cast<char*>(tmp_node.getLinkAddress()) - reinterpret_cast<char*>(std::addressof(tmp_node));

				return reinterpret_cast<node_type*>(reinterpret_cast<char*>(std::addressof(const_cast<node_type*&>(this->head))) - diff);
			}

		private:
			template<class... Args>
			auto new_one_node(node_type* ptr_next, Args&&... args)
			{
				auto ptr = allocator_traits<allocator_type>::allocate(alloc, 1);

				allocator_traits<allocator_type>::construct(alloc, ptr->getValueAddress(), std::forward<Args>(args)...);
				allocator_traits<allocator_type>::construct(alloc, ptr->getLinkAddress(), ptr_next);

				return ptr;
			}

			template<class... Args>
			auto new_nodes(size_type count, Args&&... args)
			{
				node_pointer ptr { nullptr };

				for (; count != 0; --count)
					ptr = new_one_node(ptr, std::forward<Args>(args)...);

				return ptr;
			}

			template<class NodeTy>
			auto delete_one_node(NodeTy* ptr)
			{
				NodeTy* ptr_next { nullptr };

				if (ptr != nullptr)
				{
					ptr_next = ptr->next();

					ptr->next() = nullptr;
					allocator_traits<allocator_type>::destroy(alloc, ptr);
					allocator_traits<allocator_type>::deallocate(alloc, ptr, 1);
				}

				return ptr_next;
			}

			template<class NodeTy>
			void delete_nodes(NodeTy* ptr)
			{
				while (ptr != nullptr)
					ptr = delete_one_node(ptr);
			}

			template<class FwdIter>
			node_pointer copy_new_nodes(FwdIter first, FwdIter last)
			{
				node_pointer head { nullptr };

				if (first != last)
				{
					head = new_one_node(nullptr, *first++);

					for (auto prev = head; first != last; ++first)
					{
						auto ptr = new_one_node(nullptr, *first);

						prev->next() = ptr;
						prev = ptr;
					}
				}

				return head;
			}

			node_pointer   head  { nullptr };
			allocator_type alloc {};
	};
}
