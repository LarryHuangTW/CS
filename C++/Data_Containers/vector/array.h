#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <iterator>

namespace cust					//customized / non-standard
{
	using std::size_t;
	using std::ptrdiff_t;
	using std::initializer_list;
	using std::allocator;
	using std::allocator_traits;

	/*
	 *	implementation of fixed-size array with static memory allocation
	 */
	template<class T, size_t N>
	struct static_array_impl
	{
		using value_type    = T;
		using size_type     = size_t;
		using pointer       = value_type*;
		using const_pointer = const value_type*;

		constexpr size_type size() const noexcept
		{
			return N;
		}

		constexpr pointer data() noexcept
		{
			return elem;
		}

		constexpr const_pointer data() const noexcept
		{
			return elem;
		}

		T elem[N == 0 ? 1 : N];
	};

	/*
	 *	implementation of fixed-size or variable-size array with dynamic memory allocation
	 */
	template<class T, bool FixedSize = true>
	class dynamic_array_impl
	{
		public:
			using value_type    = T;
			using size_type     = size_t;
			using pointer       = value_type*;
			using const_pointer = const value_type*;

			constexpr size_type size() const noexcept
			{
				return static_cast<size_type>(elem[1] - elem[0]);
			}

			template<class = std::enable_if_t<!FixedSize>>
			constexpr size_type capacity() const noexcept
			{
				return static_cast<size_type>(elem[2] - elem[0]);
			}

			constexpr pointer data() noexcept
			{
				return elem[0];
			}

			constexpr const_pointer data() const noexcept
			{
				return elem[0];
			}

		protected:
			pointer elem[FixedSize ? 2 : 3] {};
	};

	/*
	 *	public interface / APIs of fixed-size array
	 */
	template<class T, class ImplTy>
	class array_interface : public ImplTy
	{
		public:
			//public member types
			using base_type       = ImplTy;

			using value_type      = T;
			using size_type       = size_t;
			using difference_type = ptrdiff_t;
			using pointer         = value_type*;
			using const_pointer   = const value_type*;
			using reference       = value_type&;
			using const_reference = const value_type&;
			using iterator        = pointer;						//oversimplified naked iterator
			using const_iterator  = const_pointer;

			using reverse_iterator       = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			//returns the number of elements in the array
			constexpr size_type size() const noexcept
			{
				return base_type::size();
			}

			//check if the array is empty
			constexpr bool empty() const noexcept
			{
				return size() == 0;
			}

			//returns the maximum possible number of elements in the array
			constexpr size_type max_size() const noexcept
			{
				return size();
			}

			//direct access to the underlying array
			constexpr pointer data() noexcept
			{
				return base_type::data();
			}

			constexpr const_pointer data() const noexcept
			{
				return base_type::data();
			}

			//access the first element in the array
			constexpr reference front()
			{
				return data()[0];
			}

			constexpr const_reference front() const
			{
				return data()[0];
			}

			//access the last element in the array
			constexpr reference back()
			{
				return data()[empty() ? 0 : size() - 1];
			}

			constexpr const_reference back() const
			{
				return data()[empty() ? 0 : size() - 1];
			}

			//access specified element in the array with bounds checking
			constexpr reference at(size_type pos)
			{
				if (size() <= pos)
					throw std::out_of_range("invalid element access: index out of range");

				return data()[pos];
			}

			constexpr const_reference at(size_type pos) const
			{
				if (size() <= pos)
					throw std::out_of_range("invalid element access: index out of range");

				return data()[pos];
			}

			//access specified element in the array
			constexpr reference operator [] (size_type pos)
			{
				return data()[pos];
			}

			constexpr const_reference operator [] (size_type pos) const
			{
				return data()[pos];
			}

			//fill the array with specified value
			constexpr void fill(const_reference value)
			{
				std::fill_n(begin(), size(), value);
			}

			//swap all elements with other array
			constexpr void swap(array_interface& other) noexcept
			{
				std::swap_ranges(begin(), end(), other.begin());
			}

			//returns an iterator to the beginning
			constexpr iterator begin() noexcept
			{
				return data();
			}

			constexpr const_iterator begin() const noexcept
			{
				return data();
			}

			//returns an iterator to the end
			constexpr iterator end() noexcept
			{
				return begin() + size();
			}

			constexpr const_iterator end() const noexcept
			{
				return begin() + size();
			}

			//returns a reverse iterator to the beginning
			constexpr reverse_iterator rbegin() noexcept
			{
				return reverse_iterator(end());
			}

			constexpr const_reverse_iterator rbegin() const noexcept
			{
				return const_reverse_iterator(end());
			}

			//returns a reverse iterator to the end
			constexpr reverse_iterator rend() noexcept
			{
				return reverse_iterator(begin());
			}

			constexpr const_reverse_iterator rend() const noexcept
			{
				return const_reverse_iterator(begin());
			}

			//returns a const iterator to the beginning
			constexpr const_iterator cbegin() const noexcept
			{
				return begin();
			}

			//returns a const iterator to the end
			constexpr const_iterator cend() const noexcept
			{
				return end();
			}

			//returns a const reverse iterator to the beginning
			constexpr const_reverse_iterator crbegin() const noexcept
			{
				return rbegin();
			}

			//returns a const reverse iterator to the end
			constexpr const_reverse_iterator crend() const noexcept
			{
				return rend();
			}
	};

	/*
	 *	fixed-size array with static memory allocation
	 */
	template<class T, size_t N>
	struct array : public array_interface<T, static_array_impl<T, N>>
	{
	};

	/*
	 *	fixed-size array with dynamic memory allocation
	 */
	template<class T, class Allocator = allocator<T>>
	class dynamic_array : public array_interface<T, dynamic_array_impl<T>>
	{
		using base_type = array_interface<T, dynamic_array_impl<T>>;

		using base_type::elem;

		public:
			using value_type     = base_type::value_type;
			using size_type      = base_type::size_type;
			using pointer        = base_type::pointer;
			using const_pointer  = base_type::const_pointer;
			using allocator_type = typename allocator_traits<Allocator>::template rebind_alloc<value_type>;

			//constructors
			constexpr explicit dynamic_array(size_type count, const value_type& val = value_type())
			{
				if (count != 0)
				{
					alloc_n_elems(count);

					for (auto ptr { elem[0] }; ptr != nullptr && ptr != elem[1]; ++ptr)
						allocator_traits<allocator_type>::construct(alloc, ptr, val);
				}
			}

			constexpr dynamic_array(const dynamic_array& other)
			{
				if ( !other.empty() )
				{
					alloc_n_elems(other.size());

					construct_n_elems(other.begin(), other.end());
				}
			}

			constexpr dynamic_array(dynamic_array&& other) noexcept
			{
				if ( !other.empty() )
				{
					std::swap(elem[0], other.elem[0]);
					std::swap(elem[1], other.elem[1]);
				}
			}

			template<class InputIter>
			constexpr dynamic_array(InputIter first, InputIter last)
			{
				if (first != last)
				{
					alloc_n_elems(std::distance(first, last));

					construct_n_elems(first, last);
				}
			}

			constexpr dynamic_array(initializer_list<value_type> init)
			{
				if (init.size() != 0)
				{
					alloc_n_elems(init.size());

					construct_n_elems(init.begin(), init.end());
				}
			}

			//destructor
			constexpr ~dynamic_array()
			{
				for (auto ptr { elem[0] }; ptr != nullptr && ptr != elem[1]; ++ptr)
					allocator_traits<allocator_type>::destroy(alloc, ptr);

				allocator_traits<allocator_type>::deallocate(alloc, elem[0], base_type::size());

				elem[1] = elem[0] = nullptr;
			}

			//copy assignment operator
			constexpr dynamic_array& operator = (const dynamic_array& other)
			{
				copy_or_move_assign(other.begin(), other.end(), true);

				return *this;
			}

			//move assignment operator
			constexpr dynamic_array& operator = (dynamic_array&& other) noexcept
			{
				copy_or_move_assign(other.begin(), other.end(), false);

				return *this;
			}

			//copy and replace elements with initializer list
			constexpr dynamic_array& operator = (initializer_list<value_type> init)
			{
				copy_or_move_assign(init.begin(), init.end(), true);

				return *this;
			}

		private:
			//allocate (memory) space for n elements
			constexpr void alloc_n_elems(size_type n)
			{
				elem[0] = allocator_traits<allocator_type>::allocate(alloc, n);
				elem[1] = elem[0] + n;
			}

			//construct n elements in the allocated (memory) space
			template<class InputIter>
			constexpr void construct_n_elems(InputIter first, InputIter last)
			{
				for (auto ptr { elem[0] }; first != last; ++ptr, ++first)
					allocator_traits<allocator_type>::construct(alloc, ptr, *first);
			}

			//copy (flag is true) or move (flag is false) assignment
			template<class InputIter>
			constexpr void copy_or_move_assign(InputIter first, InputIter last, bool flag)
			{
				for (auto iter { base_type::begin() }; iter != base_type::end() && first != last; ++iter, ++first)
					*iter = (flag ? *first : std::move(*first));
			}

			//allocator
			allocator_type alloc {};
	};
}
