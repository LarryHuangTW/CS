#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <utility>
#include <initializer_list>
#include <algorithm>
#include "Container.h"

namespace cust
{
	using std::bidirectional_iterator_tag;
	using std::size_t;
	using std::ptrdiff_t;
	using std::allocator;
	using std::allocator_traits;
	using std::initializer_list;

	template<class T, class Allocator>
	class list;

	template<class T>
	class list_iterator;

	/*
	 *	const bidirectional iterator associated with list data container
	 */
	template<class T>
	class list_const_iterator
	{
		template<class T, class Allocator>
		friend class list;

		protected:
			using node_type = Container::NodeBase<T, 2>;

		public:
			using iterator_category = bidirectional_iterator_tag;
			using value_type        = T;
			using difference_type   = std::ptrdiff_t;
			using pointer           = node_type*;
			using const_pointer     = const node_type*;
			using reference         = const value_type&;

			list_const_iterator() noexcept = default;

			list_const_iterator(pointer p) noexcept : ptr { p }
			{
			}

			reference operator * () const noexcept
			{
				return ptr->value;
			}

			const_pointer operator -> () const noexcept
			{
				return ptr;
			}

			auto& operator ++ () noexcept
			{
				if (ptr != nullptr)
					ptr = ptr->next();

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
				if (ptr != nullptr)
					ptr = ptr->prev();

				return *this;
			}

			auto operator-- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}

			bool operator == (list_const_iterator other) const noexcept
			{
				return ptr == other.ptr ? true : false;
			}

			bool operator != (list_const_iterator other) const noexcept
			{
				return !operator==(other);
			}

			operator list_iterator<value_type>() const
			{
				return list_iterator<value_type>(ptr);
			}

		protected:
			pointer ptr { nullptr };
	};

	/*
	 *	bidirectional iterator associated with list data container
	 */
	template<class T>
	class list_iterator : public list_const_iterator<T>
	{
		template<class T, class Allocator>
		friend class list;

		public:
			using base_type = list_const_iterator<T>;

			using iterator_category = bidirectional_iterator_tag;
			using value_type        = T;
			using difference_type   = ptrdiff_t;
			using pointer           = base_type::node_type*;
			using reference         = value_type&;

			using base_type::base_type;

			reference operator * () const noexcept
			{
				return this->ptr->value;
			}

			pointer operator -> () const noexcept
			{
				return this->ptr;
			}

			auto& operator ++ () noexcept
			{
				base_type::operator++();

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
				base_type::operator--();

				return *this;
			}

			auto operator -- (int) noexcept
			{
				auto tmp { *this };

				operator--();

				return tmp;
			}
	};

	/*
	 *	list data container (with doubly-linked list data structure)
	 */
	template<class T, class Allocator = allocator<T>>
	class list
	{
		public:
			//public member types
			using value_type             = T;
			using allocator_type         = Allocator;
			using size_type              = size_t;
			using difference_type        = ptrdiff_t;
			using reference              = value_type&;
			using const_reference        = const reference;
			using pointer                = allocator_traits<allocator_type>::pointer;
			using const_pointer          = allocator_traits<allocator_type>::const_pointer;
			using iterator               = list_iterator<value_type>;
			using const_iterator         = list_const_iterator<value_type>;
			using reverse_iterator       = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		private:
			//private member types
			using node_type              = Container::NodeBase<value_type, 2>;
			using node_pointer           = node_type*;
			using node_allocator_type    = allocator_traits<allocator_type>::template rebind_alloc<node_type>;

		public:
			//default constructor
			list()
			{
				pseudo_head = new_one_node(nullptr, nullptr, value_type{});
			}

			//constructor with count default values
			explicit list(size_type count)
			{
				pseudo_head = new_one_node(nullptr, nullptr, value_type{});

				if (count != 0)
				{
					auto pair { new_nodes(count, value_type{}) };

					pseudo_head->next() = pair.first;
					pseudo_head->prev() = pair.second;

					pair.second->next() = pair.first->prev() = pseudo_head;
				}

				sz = count;
			}

			//constructor
			explicit list(size_type count, const value_type& value)
			{
				pseudo_head = new_one_node(nullptr, nullptr, value_type{});

				if (count != 0)
				{
					auto pair { new_nodes(count, value) };

					pseudo_head->next() = pair.first;
					pseudo_head->prev() = pair.second;

					pair.second->next() = pair.first->prev() = pseudo_head;
				}

				sz = count;
			}

			//copy constructor
			list(const list& other)
			{
				pseudo_head = new_one_node(nullptr, nullptr, value_type{});

				if ( !other.empty() )
				{
					auto pair { copy_new_nodes(other.begin(), other.end()) };

					pseudo_head->next() = pair.first;
					pseudo_head->prev() = pair.second;

					pair.second->next() = pair.first->prev() = pseudo_head;
				}

				sz = other.size();
			}

			//move constructor
			list(list&& other)
			{
				std::swap(pseudo_head, other.pseudo_head);
				std::swap(sz, other.sz);
			}

			//constructor with initializer list
			list(initializer_list<value_type> initList)
			{
				pseudo_head = new_one_node(nullptr, nullptr, value_type{});

				if (initList.size() != 0)
				{
					auto pair { copy_new_nodes(initList.begin(), initList.end()) };

					pseudo_head->next() = pair.first;
					pseudo_head->prev() = pair.second;

					pair.second->next() = pair.first->prev() = pseudo_head;
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
			list& operator = (list&& other)
			{
				clear();

				std::swap(pseudo_head, other.pseudo_head);
				std::swap(sz, other.sz);

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

			//return number of elements in the container
			size_type size() const noexcept
			{
				return sz;
			}

			//checks if the container is empty
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//clear all elements in the container
			void clear() noexcept
			{
				delete_nodes(pseudo_head->next());

				pseudo_head->prev() = pseudo_head->next() = nullptr;
			}

			//access the first element of the container
			reference front()
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->next()->value;
			}

			//access the first element of the container
			const_reference front() const
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->next()->value;
			}

			//access the last element of the container
			reference back()
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->prev()->value;
			}

			//access the last element of the container
			const_reference back() const
			{
				if (empty())
					std::cerr << "tried to access an empty list";

				return pseudo_head->prev()->value;
			}

			//add an element to the beginning of the container
			void push_front(const value_type& value)
			{
				emplace_front(value);
			}

			//add an element to the beginning of the container
			void push_front(value_type&& value)
			{
				emplace_front(std::move(value));
			}

			//append an element to the end of the container
			void push_back(const value_type& value)
			{
				emplace_back(value);
			}

			//append an element to the end of the container
			void push_back(value_type&& value)
			{
				emplace_back(std::move(value));
			}

			//add an element to the beginning of the container
			template<class... Args>
			reference emplace_front(Args&&... args)
			{
				auto ptr { new_one_node(empty() ? pseudo_head : pseudo_head->next(), pseudo_head, std::forward<Args>(args)...) };

				ptr->next()->prev() = pseudo_head->next() = ptr;

				++sz;

				return front();
			}

			//append an element to the end of the container
			template<class... Args>
			reference emplace_back(Args&&... args)
			{
				auto ptr { new_one_node(pseudo_head, empty() ? pseudo_head : pseudo_head->prev(), std::forward<Args>(args)...) };

				ptr->prev()->next() = pseudo_head->prev() = ptr;

				++sz;

				return back();
			}

			//remove the first element of the container
			void pop_front()
			{
				if (empty())
				{
					std::cerr << "tried to pop front an empty list";

					return;
				}

				auto ptr { delete_one_node(pseudo_head->next()).first };

				pseudo_head->next() = (empty() ? nullptr : ptr);

				if (empty())
					pseudo_head->prev() = nullptr;
				else
					pseudo_head->next()->prev() = pseudo_head;
			}

			//remove the last element of the container
			void pop_back()
			{
				if (empty())
				{
					std::cerr << "tried to pop back an empty list";

					return;
				}

				auto ptr { delete_one_node(pseudo_head->prev()).second };

				pseudo_head->prev() = (empty() ? nullptr : ptr);

				if (empty())
					pseudo_head->next() = nullptr;
				else
					pseudo_head->prev()->next() = pseudo_head;
			}

			//erase an element in the specified position
			iterator erase(const_iterator pos)
			{
				if (pos != cend())
				{
					auto pr { delete_one_node(pos.ptr) };

					if (empty())
					{
						pseudo_head->prev() = pseudo_head->next() = nullptr;
					}
					else
					{
						pr.first->prev()  = pr.second;
						pr.second->next() = pr.first;

						return iterator(pr.first);
					}
				}

				return end();
			}

			//earse elements in the specified range [first , last)
			iterator erase(const_iterator first, const_iterator last)
			{
				for ( ; first != cend() && first != last; )
					first = erase(first);

				return last;
			}

			//merge two sorted lists
			void merge(list& other)
			{
				if ( !other.empty() )
				{
					auto other_head { other.begin()->prev() };

					inplace_merge(pseudo_head, pseudo_head->prev(), other_head, other_head->prev());

					other_head->prev() = other_head->next() = nullptr;

					sz += other.sz;
					other.sz = 0;
				}
			}

			//merge two sorted lists
			void merge(list&& other)
			{
				if ( !other.empty() )
				{
					auto other_head{ other.begin()->prev() };

					inplace_merge(pseudo_head, pseudo_head->prev(), other_head, other_head->prev());

					other_head->prev() = other_head->next() = nullptr;

					sz += other.sz;
					other.sz = 0;
				}
			}

			//reverse the order of the elements in the list
			void reverse() noexcept
			{
				auto ptr { pseudo_head };

				for (auto cnt { size() + 1 }; cnt != 0; --cnt)
				{
					std::swap(ptr->next(), ptr->prev());

					ptr = ptr->prev();
				}
			}

			/*
			 *	Try to simulate std::list::splice(pos, *this, first, last)
			 *	rotate range [first, last) to next of pos
			 */
			void rotate(const_iterator pos, const_iterator first, const_iterator last) noexcept
			{
				if (pos == first || std::next(pos) == first || first == last) return;

				auto tmp { std::next(pos) };

				pos.ptr->next()           = first.ptr;
				first.ptr->prev()->next() = last.ptr;
				last.ptr->prev()->next()  = tmp.ptr;

				tmp.ptr->prev()   = last.ptr->prev();
				last.ptr->prev()  = first.ptr->prev();
				first.ptr->prev() = pos.ptr;
			}

			iterator begin() noexcept { return empty() ? pseudo_head : pseudo_head->next(); }

			const_iterator begin() const noexcept { return empty() ? pseudo_head : pseudo_head->next(); }

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
			template<class... Args>
			auto new_one_node(node_pointer ptr_next, node_pointer ptr_prev, Args&&... args)
			{
				auto ptr { allocator_traits<node_allocator_type>::allocate(alloc, 1) };

				allocator_traits<node_allocator_type>::construct(alloc, ptr->getValueAddress(), std::forward<Args>(args)...);
				allocator_traits<node_allocator_type>::construct(alloc, ptr->getLinkAddress(), ptr_next);
				allocator_traits<node_allocator_type>::construct(alloc, ptr->getLinkAddress() + 1, ptr_prev);

				return ptr;
			}

			auto new_nodes(size_type count, const value_type& value)
			{
				node_pointer head { nullptr }, tail { nullptr };

				if (count != 0)
				{
					tail = head = new_one_node(nullptr, nullptr, value);

					for (auto prev { head }; --count != 0; )
					{
						tail = new_one_node(nullptr, prev, value);

						prev = prev->next() = tail;
					}
				}

				return std::make_pair(head, tail);
			}

			template<class FwdIter>
			auto copy_new_nodes(FwdIter first, FwdIter last)
			{
				node_pointer head { nullptr }, tail { nullptr };

				if (first != last)
				{
					tail = head = new_one_node(nullptr, nullptr, *first);

					for (auto prev{ head }; ++first != last; )
					{
						tail = new_one_node(nullptr, prev, *first);

						prev = prev->next() = tail;
					}
				}

				return std::make_pair(head, tail);
			}

			auto delete_one_node(node_pointer ptr)
			{
				std::pair<node_pointer, node_pointer> pr { nullptr , nullptr };

				if (ptr != nullptr)
				{
					pr.first  = ptr->next();
					pr.second = ptr->prev();

					ptr->next() = nullptr;
					ptr->prev() = nullptr;
					allocator_traits<node_allocator_type>::destroy(alloc, ptr);
					allocator_traits<node_allocator_type>::deallocate(alloc, ptr, 1);

					--sz;
				}

				return pr;
			}

			void delete_nodes(node_pointer ptr)
			{
				while (ptr != nullptr && ptr != pseudo_head)
					ptr = delete_one_node(ptr).first;
			}

			/*
			 *	Merge the sorted range (prev2, back2] into the sorted range (prev1, back1]
			 */
			template<class Compare = std::less<>>
			node_pointer inplace_merge(node_pointer prev1, node_pointer back1, node_pointer prev2, node_pointer back2, Compare cmp = Compare{})
			{
				if (prev1 == nullptr || back1 == nullptr || prev2 == nullptr || back2 == nullptr)
					return back1;

				for (; ; )
				{
					while (prev1 != back1 && !cmp(prev2->next()->value, prev1->next()->value))
						prev1 = prev1->next();

					if (prev1 == back1)
					{
						if (back1 != prev2)
						{
							prev1 = prev1->next();

							back1->next() = prev2->next();
							prev2->next()->prev() = back1;

							prev2->next() = back2->next();
							back2->next()->prev() = prev2;

							back2->next() = prev1;
							prev1->prev() = back2;
						}

						back1 = back2;

						break;
					}

					auto ptr2 { prev2 };

					while (ptr2 != back2 && cmp(ptr2->next()->value, prev1->next()->value))
						ptr2 = ptr2->next();

					auto tmp2 { ptr2->next() };

					ptr2->next()  = prev1->next();
					prev1->next()->prev() = ptr2;

					prev1->next() = prev2->next();
					prev2->next()->prev() = prev1;

					prev2->next() = tmp2;
					tmp2->prev()  = prev2;

					if (ptr2 == back2)
						break;

					prev1 = ptr2;
				}

				return back1;
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
