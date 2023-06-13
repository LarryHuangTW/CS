#pragma once

#include <iostream>
#include <deque>
#include <utility>
#include <compare>
#include <forward_list>

namespace cust
{
	using std::deque;
	using std::forward_list;

	/*
	 *	implementation of stack container adaptor
	 */
	template<class T, class Container>
	class stack_impl
	{
		public:
			using container_type  = Container;
			using value_type      = container_type::value_type;
			using size_type       = container_type::size_type;
			using reference       = container_type::reference;
			using const_reference = container_type::const_reference;

			//default constructor
			stack_impl()
			{
			}

			//constructor with elements copied from other container
			explicit stack_impl(const container_type& cont) : c { cont }
			{
			}

			//constructor with elements moved from other container
			explicit stack_impl(container_type&& cont) : c { std::move(cont) }
			{
			}

			//copy constructor
			explicit stack_impl(const stack_impl& other) : c { other.c }
			{
			}

			//move constructor
			explicit stack_impl(stack_impl&& other) noexcept : c { std::move(other.c) }
			{
			}

			//constructor with elements from a range [first, last)
			template<class InputIter>
			stack_impl(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					push(*first);
			}

			//destructor
			~stack_impl()
			{
			}

			//checks if the stack is empty
			[[nodiscard]] bool empty() const
			{
				return c.empty();
			}

			//return number of elements in the stack
			size_type size() const
			{
				return c.size();
			}

			//access the top element of the stack
			reference top()
			{
				if (empty())
					std::cerr << "tried to access the top element of an empty stack";

				return c.back();
			}

			const_reference top() const
			{
				if (empty())
					std::cerr << "tried to access the top element of an empty stack";

				return c.back();
			}

			//add an element at the top of the stack
			void push(const value_type& value)
			{
				c.push_back(value);
			}

			void push(value_type&& value)
			{
				c.push_back(std::move(value));
			}

			//remove the top element of the stack
			void pop()
			{
				c.pop_back();
			}

			//construct an element at the top of the stack
			template<class... Args>
			decltype(auto) emplace(Args&&... args)
			{
				c.emplace_back(std::forward<Args>(args)...);
			}

			//swap the elements with other stack
			void swap(stack_impl& other) noexcept
			{
				std::swap(c, other.c);
			}

			//get the container of the stack
			const container_type& get_container() const noexcept
			{
				return c;
			}

		protected:
			container_type c {};
	};

	/*
	 *	a container adaptor with last-in, first-out (LIFO) property
	 */
	template<class T, class Container = deque<T>>
	class stack : public stack_impl<T, Container>
	{
		public:
			using base_type = stack_impl<T, Container>;

			using base_type::base_type;
	};

	/*
	 *	specialization of class template stack<T, Container>
	 */
	template<class T>
	class stack<T, forward_list<T>> : public stack_impl<T, forward_list<T>>
	{
		public:
			using base_type = stack_impl<T, forward_list<T>>;

			using container_type  = std::forward_list<T>;
			using value_type      = container_type::value_type;
			using size_type       = container_type::size_type;
			using reference       = container_type::reference;
			using const_reference = container_type::const_reference;

			using base_type::base_type;

			//return number of elements in the stack
			size_type size() const
			{
				size_type cnt { 0 };

				for (auto iter { this->c.begin() }; iter != this->c.end(); ++cnt, ++iter);

				return cnt;
			}

			//access the top element of the stack
			reference top()
			{
				if (this->empty())
					std::cerr << "tried to access the top element of an empty stack";

				return this->c.front();
			}

			const_reference top() const
			{
				if (this->empty())
					std::cerr << "tried to access the top element of an empty stack";

				return this->c.front();
			}

			//add an element at the top of the stack
			void push(const value_type& value)
			{
				this->c.push_front(value);
			}

			void push(value_type&& value)
			{
				this->c.push_front(std::move(value));
			}

			//remove the top element of the stack
			void pop()
			{
				this->c.pop_front();
			}

			//construct an element at the top of the stack
			template<class... Args>
			decltype(auto) emplace(Args&&... args)
			{
				this->c.emplace_front(std::forward<Args>(args)...);
			}
	};

	template<class T, class Container>
	bool operator == (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.get_container() == rhs.get_container();
	}

	template<class T, class Container>
	bool operator != (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T, class Container>
	bool operator < (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.get_container() < rhs.get_container();
	}

	template<class T, class Container>
	bool operator > (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return rhs < lhs;
	}

	template<class T, class Container>
	bool operator <= (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.get_container() <= rhs.get_container();
	}

	template<class T, class Container>
	bool operator >= (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return rhs <= lhs;
	}

	template<class T, std::three_way_comparable Container>
	std::compare_three_way_result_t<Container> operator <=> (const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.get_container() <=> rhs.get_container();
	}

	template<class T, class Container>
	void swap(stack<T, Container>& lhs, stack<T, Container>& rhs)
	{
		lhs.swap(rhs);
	}
}
