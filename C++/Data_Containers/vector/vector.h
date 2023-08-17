#pragma once

#include <iostream>
#include "array.h"

namespace cust
{
	/*
	 *	variable-size array with dynamic memory allocation
	 */
	template<class T, class Allocator = allocator<T>>
	class vector : public array_interface<T, dynamic_array_impl<T, false>>
	{
		using base_type = array_interface<T, dynamic_array_impl<T, false>>;

		using base_type::elem;

		public:
			using value_type      = base_type::value_type;
			using size_type       = base_type::size_type;
			using pointer         = base_type::pointer;
			using const_pointer   = base_type::const_pointer;
			using reference       = base_type::reference;
			using const_reference = base_type::const_reference;
			using allocator_type  = typename allocator_traits<Allocator>::template rebind_alloc<value_type>;

			//default constructor
			vector() noexcept
			{
			}

			//construct the vector with count copies of val
			constexpr explicit vector(size_type count, const value_type& val = value_type())
			{
				alloc_n_elems(count);

				for ( ; count != 0; --count)
					allocator_traits<allocator_type>::construct(alloc, elem[1]++, val);
			}

			//copy constructor
			constexpr vector(const vector& other)
			{
				alloc_n_elems(other.size());

				construct_n_elems(other.begin(), other.end());
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

			//construct the vector with two iterators
			template<class InputIter>
			constexpr vector(InputIter first, InputIter last)
			{
				alloc_n_elems(std::distance(first, last));

				construct_n_elems(first, last);
			}

			//construct the vector with initializer list
			constexpr vector(initializer_list<value_type> init)
			{
				alloc_n_elems(init.size());

				construct_n_elems(init.begin(), init.end());
			}

			//destructor
			constexpr ~vector()
			{
				destroy();

				dealloc(elem[0], base_type::capacity());

				elem[2] = elem[1] = elem[0] = nullptr;
			}

			//copy assignment operator
			constexpr vector& operator = (const vector& other)
			{
				if (other.size() <= base_type::capacity())
					elem[1] = std::copy(other.begin(), other.end(), base_type::begin());
				else
				{
					clear();

					dealloc(elem[0], base_type::capacity());

					alloc_n_elems(other.size());

					construct_n_elems(other.begin(), other.end());
				}

				return *this;
			}

			//move assignment operator
			constexpr vector& operator = (vector&& other)
			{
				clear();

				dealloc(elem[0], base_type::capacity());

				elem[2] = elem[1] = elem[0] = nullptr;

				std::swap(elem[0], other.elem[0]);
				std::swap(elem[1], other.elem[1]);
				std::swap(elem[2], other.elem[2]);

				return *this;
			}

			//copy and replace elements with initializer list
			constexpr vector& operator = (initializer_list<value_type> init)
			{
				if (init.size() <= base_type::capacity())
					elem[1] = std::copy(init.begin(), init.end(), base_type::begin());
				else
				{
					clear();

					dealloc(elem[0], base_type::capacity());

					alloc_n_elems(init.size());

					construct_n_elems(init.begin(), init.end());
				}

				return *this;
			}

			//get allocator of the vector
			constexpr allocator_type get_allocator() const noexcept
			{
				return alloc;
			}

			//returns the maximum number of elements the vector can hold
			constexpr size_type max_size() const noexcept
			{
				return allocator_traits<allocator_type>::max_size(alloc);
			}

			//reserve the capacity of the vector
			constexpr void reserve(size_type new_cap)
			{
				if (check_capacity(new_cap))
				{
					auto ptr0 { elem[0] }, ptr1 { elem[1] }, ptr2 { elem[2] };

					alloc_n_elems(new_cap);

					construct_n_elems(ptr0, ptr1, false);

					dealloc(ptr0, ptr2 - ptr0);
				}
			}

			constexpr void clear() noexcept
			{
				destroy();
			}

			template<class... Args>
			constexpr reference emplace_back(Args&&... args)
			{
				if (base_type::size() == base_type::capacity())
					reserve(grow_cap_strategy());

				auto ptr { elem[1] };

				allocator_traits<allocator_type>::construct(alloc, elem[1]++, std::forward<Args>(args)...);

				return *ptr;
			}
			
			//append value to the end of the vector
			constexpr void push_back(const_reference value)
			{
				emplace_back(value);
			}

			//append value to the end of the vector
			constexpr void push_back(value_type&& value)
			{
				emplace_back(std::move(value));
			}

			//remove the last element of the vector
			constexpr void pop_back()
			{
				if ( !base_type::empty() )
					destroy(--elem[1]);
			}

		private:
			//check new capacity and compare to old capacity
			constexpr bool check_capacity(size_type n) const
			{
				if (max_size() < n)
					std::cerr << "required space is larger than max size";

				return base_type::capacity() < n;
			}

			//capacity growth strategy
			constexpr size_type grow_cap_strategy() const
			{
				return base_type::capacity() == 0 ? 2 : base_type::capacity() * 2;
			}

			//allocate n-element size (memory) space
			constexpr pointer alloc_n(size_type n)
			{
				check_capacity(n);

				return n == 0 ? nullptr : allocator_traits<allocator_type>::allocate(alloc, n);
			}

			//allocate (memory) space for n elements
			constexpr void alloc_n_elems(size_type n)
			{
				if (n != 0)
				{
					elem[1] = elem[0] = alloc_n(n);
					elem[2] = elem[0] + n;
				}
			}

			//construct n elements in the allocated (memory) space
			template<class InputIter>
			constexpr void construct_n_elems(InputIter first, InputIter last, bool flag = true)
			{
				for( ; first != last; ++first)
					allocator_traits<allocator_type>::construct(alloc, elem[1]++, flag ? *first : std::move(*first));
			}

			//destory all elements in the vector
			constexpr void destroy()
			{
				for( ; elem[1] != nullptr && elem[1] != elem[0]; )
					allocator_traits<allocator_type>::destroy(alloc, --elem[1]);
			}

			//destory one element
			constexpr void destroy(pointer ptr)
			{
				if (ptr != nullptr)
					allocator_traits<allocator_type>::destroy(alloc, ptr);
			}

			//deallocate n-elements size (memory) space
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
