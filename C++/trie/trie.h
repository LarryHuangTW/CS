#pragma once

#include "basic_trie.h"
#include "uni_trie.h"

namespace cust					//customized / non-standard
{
	template<std::size_t N, char fst_ch, class T = void, class Allocator = std::allocator<char>>
	using trie = basic_trie<N, fst_ch, T, Allocator>;

	using wtrie   = uni_trie<std::wstring>;
	using u8trie  = uni_trie<std::u8string>;
	using u16trie = uni_trie<std::u16string>;
	using u32trie = uni_trie<std::u32string>;
}
