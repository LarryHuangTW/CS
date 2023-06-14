#pragma once

#include <iostream>
#include <deque>
#include <utility>
#include <compare>

namespace cust					//customized / non-standard
{
	/*
	 *	a container adaptor with fast-in, first-out (FIFO) property
	 */
	template<class T, class Container = std::deque<T>>
	class queue
	{
		public:
			using container_type  = Container;
			using value_type      = typename container_type::value_type;
			using size_type       = typename container_type::size_type;
			using reference       = typename container_type::reference;
			using const_reference = typename container_type::const_reference;

			//default constructor
			queue()
			{
			}

			//constructor with elements copied from other container
			explicit queue(const container_type& cont) : c { cont }
			{
			}

			//constructor with elements moved from other container
			explicit queue(container_type&& cont) : c { std::move(cont) }
			{
			}

			//copy constructor
			explicit queue(const queue& other) : c { other.c }
			{
			}

			//move constructor
			explicit queue(queue&& other) noexcept : c { std::move(other.c) }
			{
			}

			//constructor with elements from a range [first, last)
			template<class InputIter>
			queue(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					push(*first);
			}

			//destructor
			~queue()
			{
			}

			//checks if the queue is empty
			[[nodiscard]] bool empty() const
			{
				return c.empty();
			}

			//return number of elements in the queue
			size_type size() const
			{
				return c.size();
			}

			//access the first element of the queue
			reference front()
			{
				if (empty())
					std::cerr << "tried to access the first element of an empty queue";

				return c.front();
			}

			const_reference front() const
			{
				if (empty())
					std::cerr << "tried to access the first element of an empty queue";

				return c.front();
			}

			//access the last element of the queue
			reference back()
			{
				if (empty())
					std::cerr << "tried to access the last element of an empty queue";

				return c.back();
			}

			const_reference back() const
			{
				if (empty())
					std::cerr << "tried to access the last element of an empty queue";

				return c.back();
			}

			//add an element to the end of the queue
			void push(const value_type& value)
			{
				c.push_back(value);
			}

			void push(value_type&& value)
			{
				c.push_back(std::move(value));
			}

			//remove an element from the front of the queue
			void pop()
			{
				c.pop_front();
			}

			//construct an element at the end of the queue
			template<class... Args>
			decltype(auto) emplace(Args&&... args)
			{
				c.emplace_back(std::forward<Args>(args)...);
			}

			//swap the elements with other queue
			void swap(queue& other) noexcept
			{
				std::swap(c, other.c);
			}

			//get the container of the queue
			const container_type& get_container() const noexcept
			{
				return c;
			}

		protected:
			container_type c {};
	};

	template<class T, class Container>
	bool operator == (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return lhs.get_container() == rhs.get_container();
	}

	template<class T, class Container>
	bool operator != (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T, class Container>
	bool operator < (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return lhs.get_container() < rhs.get_container();
	}

	template<class T, class Container>
	bool operator > (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return rhs < lhs;
	}

	template<class T, class Container>
	bool operator <= (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return lhs.get_container() <= rhs.get_container();
	}

	template<class T, class Container>
	bool operator >= (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return rhs <= lhs;
	}

	template<class T, std::three_way_comparable Container>
	std::compare_three_way_result_t<Container> operator <=> (const queue<T, Container>& lhs, const queue<T, Container>& rhs)
	{
		return lhs.get_container() <=> rhs.get_container();
	}

	template<class T, class Container>
	void swap(queue<T, Container>& lhs, queue<T, Container>& rhs)
	{
		lhs.swap(rhs);
	}
}
