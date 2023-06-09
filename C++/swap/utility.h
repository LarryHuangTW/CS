#pragma once

#include <cstring>

namespace cust					//customized / non-standard
{
	using size_t = unsigned long long;

	enum class byte : unsigned char {};

	template<class T> struct remove_reference      { using type = T; };
	template<class T> struct remove_reference<T&>  { using type = T; };
	template<class T> struct remove_reference<T&&> { using type = T; };

	template<class T> using remove_reference_t = typename remove_reference<T>::type;

	template<class T>
	constexpr decltype(auto) move(T&& argu) noexcept
	{
		return static_cast<cust::remove_reference_t<T>&&>(argu);
	}

	//call by address version
	constexpr void swap(int* ptrL, int* ptrR)
	{
		auto tmp { *ptrL };
		*ptrL = *ptrR;
		*ptrR = tmp;
	}

	//call by reference version
	constexpr void swap(int& lhs, int& rhs)
	{
		auto tmp { lhs };
		lhs = rhs;
		rhs = tmp;
	}

	//generic version and call by address
	constexpr void swap(void* ptrL, void* ptrR, size_t sz)
	{
		auto ptr { new byte[sz] };

		if (ptr != nullptr)
		{
			std::memcpy(ptr,  ptrL, sz);
			std::memcpy(ptrL, ptrR, sz);
			std::memcpy(ptrR, ptr,  sz);

			delete[] ptr;
		}
	}

	//C++ function template and call by reference
	template<class T>
	constexpr void swap(T& lhs, T& rhs) noexcept
	{
		T tmp { cust::move(lhs) };
		lhs   = cust::move(rhs);
		rhs   = cust::move(tmp);
	}

	//C++ iterator swap
	//template<class FwdIt1, class FwdIt2, std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<FwdIt1>::iterator_category> && std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<FwdIt2>::iterator_category>, int> = 0>
	template<class FwdIt1, class FwdIt2>
	constexpr void iter_swap(FwdIt1 iterL, FwdIt2 iterR)
	{
		cust::swap(*iterL, *iterR);
	}
}
