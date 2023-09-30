#pragma once

#include <iostream>
#include "array.h"

namespace cust					//customized / non-standard
{
	using std::allocator;
	using std::allocator_traits;
	using std::initializer_list;

	/*
	 *	variable-size array with dynamic memory allocation
	 */
	template<class T, class Allocator = allocator<T>>
	class vector : public array_interface<T, dynamic_array_impl<T, false>>
	{
		using base_type = array_interface<T, dynamic_array_impl<T, false>>;

		using base_type::elem;

		public:
			using value_type      = typename base_type::value_type;
			using size_type       = typename base_type::size_type;
			using pointer         = typename base_type::pointer;
			using const_pointer   = typename base_type::const_pointer;
			using reference       = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using iterator        = typename base_type::iterator;
			using const_iterator  = typename base_type::const_iterator;
			using allocator_type  = typename allocator_traits<Allocator>::template rebind_alloc<value_type>;

			using base_type::empty;
			using base_type::size;
			using base_type::capacity;
			using base_type::back;
			using base_type::begin;
			using base_type::cbegin;
			using base_type::end;
			using base_type::cend;

			//default constructor
			vector() noexcept
			{
			}

			//constructs the container with count copies of value
			constexpr explicit vector(size_type count, const value_type& value = value_type())
			{
				alloc_n_elems(count);

				for ( ; count != 0; --count)
					allocator_traits<allocator_type>::construct(alloc, elem[1]++, value);
			}

			//copy constructor
			constexpr vector(const vector& other)
			{
				alloc_n_elems(other.size());

				copy_construct_elems(other.begin(), other.end());
			}

			//move constructor
			constexpr vector(vector&& other) noexcept
			{
				if ( !other.empty() )
				{
					std::swap(elem[0], other.elem[0]);
					std::swap(elem[1], other.elem[1]);
					std::swap(elem[2], other.elem[2]);
				}
			}

			//constructor with two iterators
			template<class InputIter>
			constexpr vector(InputIter first, InputIter last)
			{
				alloc_n_elems(std::distance(first, last));

				copy_construct_elems(first, last);
			}

			//constructor with initializer list
			constexpr vector(initializer_list<value_type> init)
			{
				alloc_n_elems(init.size());

				copy_construct_elems(init.begin(), init.end());
			}

			//destructor
			constexpr ~vector()
			{
				clear();

				dealloc(elem[0], capacity());

				elem[2] = elem[1] = elem[0] = nullptr;
			}

			//copy assignment operator
			constexpr vector& operator = (const vector& other)
			{
				if (other.size() <= capacity())
				{
					if (other.size() <= size())
					{
						//copy assignment
						const auto ptr { std::copy(other.begin(), other.end(), begin()) };

						//destroys remaining old elements
						for ( ; ptr != elem[1]; destroy(--elem[1]));
					}
					else
					{
						auto iter { std::next(other.begin(), size()) };

						//copy assignment
						std::copy(other.begin(), iter, begin());

						//copy construction
						for ( ; iter != other.end(); ++iter)
							allocator_traits<allocator_type>::construct(alloc, elem[1]++, *iter);
					}
				}
				else
				{
					//destroys all elements
					clear();

					//deallocates (memory) space
					dealloc(elem[0], capacity());

					//allocates new (memory) space
					alloc_n_elems(other.size());

					//copy construction
					copy_construct_elems(other.begin(), other.end());
				}

				return *this;
			}

			//move assignment operator
			constexpr vector& operator = (vector&& other) noexcept
			{
				//destroys all elements
				clear();

				//deallocates (memory) space
				dealloc(elem[0], capacity());

				elem[2] = elem[1] = elem[0] = nullptr;

				std::swap(elem[0], other.elem[0]);
				std::swap(elem[1], other.elem[1]);
				std::swap(elem[2], other.elem[2]);

				return *this;
			}

			//assignment operator with initializer list
			constexpr vector& operator = (initializer_list<value_type> init)
			{
				if (init.size() <= capacity())
				{
					if (init.size() <= size())
					{
						//copy assignment
						const auto ptr { std::copy(init.begin(), init.end(), begin()) };

						//destroys remaining old elements
						for ( ; ptr != elem[1]; destroy(--elem[1]));
					}
					else
					{
						auto iter { std::next(init.begin(), size()) };

						//copy assignment
						std::copy(init.begin(), iter, begin());

						//copy construction
						for ( ; iter != init.end(); ++iter)
							allocator_traits<allocator_type>::construct(alloc, elem[1]++, *iter);
					}
				}
				else
				{
					//destroys all elements
					clear();

					//deallocates (memory) space
					dealloc(elem[0], capacity());

					//allocates new (memory) space
					alloc_n_elems(init.size());

					//copy construction
					copy_construct_elems(init.begin(), init.end());
				}

				return *this;
			}

			//gets the allocator of the container
			constexpr allocator_type get_allocator() const noexcept
			{
				return alloc;
			}

			//returns the maximum number of elements the container can hold
			constexpr size_type max_size() const noexcept
			{
				return allocator_traits<allocator_type>::max_size(alloc);
			}

			//reserves (memory) capacity for the container
			constexpr void reserve(size_type new_cap)
			{
				if (check_capacity(new_cap))
				{
					auto ptr0 { elem[0] }, ptr1 { elem[1] }, ptr2 { elem[2] };

					//allocates new (memory) space
					alloc_n_elems(new_cap);

					//move construction
					move_construct_elems(ptr0, ptr1);

					//destroys old elements
					for (auto p { ptr0 }; p != ptr1; destroy(p++));

					//deallocates old (memory) space
					dealloc(ptr0, ptr2 - ptr0);
				}
			}

			//clears all elements
			constexpr void clear() noexcept
			{
				destroy();
			}

			//adds an element (in-place) to the end of the container
			template<class... Args>
			constexpr reference emplace_back(Args&&... args)
			{
				if (size() == capacity())
					reserve(grow_cap_strategy());

				auto ptr { elem[1] };

				allocator_traits<allocator_type>::construct(alloc, elem[1]++, std::forward<Args>(args)...);

				return *ptr;
			}
			
			//adds an element (with copy semantics) to the end of the container
			constexpr void push_back(const_reference value)
			{
				emplace_back(value);
			}

			//adds an element (with move semantics) to the end of the container
			constexpr void push_back(value_type&& value)
			{
				emplace_back(std::move(value));
			}

			//removes the last element of the container
			constexpr void pop_back()
			{
				if ( !empty() )
					destroy(--elem[1]);
			}

			//inserts an element (in-place) into the container before pos
			template<class... Args>
			constexpr iterator emplace(const_iterator pos, Args&&... args)
			{
				auto ptr { const_cast<iterator>(pos) };

				if (size() == capacity())
				{
					auto ptr0 { elem[0] }, ptr1 { elem[1] }, ptr2 { elem[2] };

					//allocates new (memory) space
					alloc_n_elems(grow_cap_strategy());

					//move construction of elements before pos
					move_construct_elems(ptr0, ptr);

					//constructs the new element in-place
					emplace_back(std::forward<Args>(args)...);

					//move construction of elements at and after pos
					move_construct_elems(ptr, ptr1);

					//destroys old elements
					for (auto p { ptr0 }; p != ptr1; destroy(p++));

					//deallocates old (memory) space
					dealloc(ptr0, ptr2 - ptr0);

					ptr = elem[0] + (ptr - ptr0);
				}
				else
				{
					if (pos == cend())
						emplace_back(std::forward<Args>(args)...);
					else
					{
						//constructs the new element at first
						value_type tmp { std::forward<Args>(args)... };

						//move construction of the last element
						allocator_traits<allocator_type>::construct(alloc, elem[1], std::move(back()));

						//move assignment of elements at and after pos
						std::move_backward(ptr, elem[1] - 1, elem[1]);

						++elem[1];

						//move assignment of the new element to the pos
						*ptr = std::move(tmp);
					}
				}

				return ptr;
			}

			//inserts an element (with copy semantics) into the container before pos
			constexpr iterator insert(const_iterator pos, const_reference value)
			{
				return emplace(pos, value);
			}

			//inserts an element (with move semantics) into the container before pos
			constexpr iterator insert(const_iterator pos, value_type&& value)
			{
				return emplace(pos, std::move(value));
			}

			//erases the element at pos of the container
			constexpr iterator erase(const_iterator pos)
			{
				auto ptr { const_cast<iterator>(pos) };

				if ( !empty() && pos != cend() )
				{
					//move assignment to the left
					std::move(ptr + 1, end(), ptr);

					pop_back();
				}

				return ptr;
			}

			//erases the elements in the range [first, last) of the container
			constexpr iterator erase(const_iterator first, const_iterator last)
			{
				auto fst { const_cast<iterator>(first) };
				auto lst { const_cast<iterator>(last)  };

				if (fst != lst)
				{
					//move assignment to the left
					lst = std::move(lst, end(), fst);

					for ( ; lst != end(); pop_back());
				}
				
				return fst;
			}

		private:
			//checks new capacity and compares it with old capacity
			constexpr bool check_capacity(size_type n) const
			{
				if (max_size() < n)
					std::cerr << "required space is larger than max size";

				return capacity() < n;
			}

			//capacity growth strategy
			constexpr size_type grow_cap_strategy() const
			{
				return capacity() == 0 ? 2 : capacity() * 2;
			}

			//allocates n-element size (memory) space
			constexpr pointer alloc_n(size_type n)
			{
				check_capacity(n);

				return n == 0 ? nullptr : allocator_traits<allocator_type>::allocate(alloc, n);
			}

			//allocates (memory) space for n elements
			constexpr void alloc_n_elems(size_type n)
			{
				if (n != 0)
				{
					elem[1] = elem[0] = alloc_n(n);
					elem[2] = elem[0] + n;
				}
			}

			//copy construction of elements
			template<class InputIter>
			constexpr void copy_construct_elems(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					allocator_traits<allocator_type>::construct(alloc, elem[1]++, *first);
			}

			//move construction of elements
			template<class InputIter>
			constexpr void move_construct_elems(InputIter first, InputIter last)
			{
				for ( ; first != last; ++first)
					allocator_traits<allocator_type>::construct(alloc, elem[1]++, std::move(*first));
			}

			//destroys all elements of the container
			constexpr void destroy()
			{
				for( ; elem[1] != nullptr && elem[1] != elem[0]; )
					allocator_traits<allocator_type>::destroy(alloc, --elem[1]);
			}

			//destroys one element
			constexpr void destroy(pointer ptr)
			{
				if (ptr != nullptr)
					allocator_traits<allocator_type>::destroy(alloc, ptr);
			}

			//deallocates n-elements size (memory) space
			constexpr void dealloc(pointer ptr, size_type n)
			{
				if (ptr != nullptr && n != 0)
					allocator_traits<allocator_type>::deallocate(alloc, ptr, n);
			}

			//allocator
			allocator_type alloc {};
	};

	template<typename T>
	bool operator == (const vector<T>& lhs, const vector<T>& rhs)
	{
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<typename T>
	bool operator != (const vector<T>& lhs, const vector<T>& rhs)
	{
		return !(lhs == rhs);
	}
}
