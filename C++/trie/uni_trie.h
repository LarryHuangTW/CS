#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <string_view>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <vector>
#include <array>
#include <utility>
#include <type_traits>
#include <cstring>

namespace cust					//customized / non-standard
{
	using std::unordered_map;
	using std::basic_string_view;
	using std::wstring;
	using std::u8string;
	using std::size_t;
	using std::vector;
	using std::array;
	using std::pair;
	using std::size_t;

	constexpr array<char16_t, 4> utf16_surrogate { 0xD800 , 0xDBFF , 0xDC00 , 0xDFFF };

	constexpr bool isUTF16_surrogate(char16_t ch)
	{
		return utf16_surrogate.front() <= ch && ch <= utf16_surrogate.back();
	}

	//character unit
	template<class CharTy>
	struct char_unit
	{
		using char_type     = CharTy;
		using size_type     = size_t;
		using pointer       = char_type*;
		using const_pointer = const char_type*;

		//default constructor
		char_unit()
		{
			ary.fill(char_type{});
		}

		//constructor
		char_unit(const_pointer ptr, size_type cnt)
		{
			for (auto i { 0ULL }; i < ary.size(); ++i)
				ary[i] = i < cnt ? ptr[i] : char_type{};
		}

		//how many characters in this unit
		size_type count() const noexcept
		{
			size_type sz { 0 };

			for ( ; sz < ary.size() && ary[sz] != char_type{}; ++sz);

			return sz;
		}

		const_pointer data() const noexcept
		{
			return ary.data();
		}

		operator char32_t () const
		{
			return c32;
		}

		union
		{
			array<char_type, sizeof(char32_t) / sizeof(char_type)> ary;
			char32_t c32;
		};
	};

	template<class CharTy>
	struct trie_node;

	template<class CharTy>
	using trie_node_base = unordered_map<char_unit<CharTy>, trie_node<CharTy>>;

	/*
	 *	or we can use std::map
	 *
	 *	besides time complexity, results of trie traversal would be different
	 */
	//template<class CharTy>
	//using trie_node_base = std::map<char_unit<CharTy>, trie_node<CharTy>>;

	template<class CharTy>
	struct trie_node : trie_node_base<CharTy>
	{
		using char_type     = CharTy;
		using key_type      = char_unit<char_type>;
		using base_type     = trie_node_base<char_type>;
		using size_type     = size_t;
		using pointer       = char_type*;
		using const_pointer = const char_type*;

		auto insert(const_pointer ptr, size_type len)
		{
			auto iter { base_type::find(key_type(ptr, len)) };

			return iter == base_type::end() ? base_type::emplace(key_type(ptr, len), trie_node<char_type>()) : std::make_pair(iter, false);
		}

		auto find(const_pointer ptr, size_type len) const
		{
			return base_type::find(key_type(ptr, len));
		}

		auto find(const_pointer ptr, size_type len)
		{
			return base_type::find(key_type(ptr, len));
		}

		bool isEndOfWord { false };
	};

	template<class T>
	class uni_trie
	{
		public:
			using string_type    = T;
			using char_type      = typename string_type::value_type;
			using strv_type      = basic_string_view<char_type>;
			using node_type      = trie_node<char_type>;
			using size_type      = size_t;
			using pointer        = char_type*;
			using const_pointer  = const char_type*;

			using iterator       = typename node_type::base_type::iterator;
			using const_iterator = typename node_type::base_type::const_iterator;

			//default constructor
			uni_trie() = default;

			//destructor
			~uni_trie()
			{
				clear();
			}

			//is there any word / string in the trie
			bool empty() const noexcept
			{
				return size() == 0;
			}

			//return the number of words / strings in the trie
			size_type size() const noexcept
			{
				return sz;
			}

			//return the number of trie nodes in the trie
			size_type count() const noexcept
			{
				return cnt;
			}

			//clear all trie nodes
			void clear() noexcept
			{
				root.clear();

				sz  = 0;
				cnt = 1;
			}

			//insert a word / string into the trie
			bool insert(strv_type word)
			{
				auto ptr { word.data() };
				pair<iterator, bool> pr {};
				bool result { false };

				if (word.empty())
					return result;

				for (int idx { 0 }, len { 0 }; idx < word.size(); idx += len, ptr += len)
				{
					auto& node { idx == 0 ? root : pr.first->second };

					len = getLen(ptr, word.size() - idx);

					pr = node.insert(ptr, len);

					if (pr.second)
						++cnt;
				}

				if ( !pr.first->second.isEndOfWord )
				{
					result = pr.first->second.isEndOfWord = true;

					++sz;
				}

				return result;
			}

			//check if a word / string in the trie
			bool search(strv_type word) const noexcept
			{
				auto ptr { word.data() };
				const_iterator iter {};
				int  idx { 0 }, len { 0 };

				for (auto it { iter }; idx < word.size(); idx += len, ptr += len, iter = it)
				{
					auto& node { idx == 0 ? root : iter->second };

					len = getLen(ptr, word.size() - idx);

					it = node.find(ptr, len);

					if (it == node.end())
						break;
				}

				return !word.empty() && idx == word.size() && iter->second.isEndOfWord;
			}

			//check if there is any word / string in the trie prefixed by the function argument
			bool starts_with(strv_type prefix) const noexcept
			{
				auto ptr { prefix.data() };
				const_iterator iter {};
				int  idx { 0 }, len{ 0 };

				for (auto it { iter }; idx < prefix.size(); idx += len, ptr += len, iter = it)
				{
					auto& node { idx == 0 ? root : iter->second };

					len = getLen(ptr, prefix.size() - idx);

					it = node.find(ptr, len);

					if (it == node.end())
						break;
				}

				return !prefix.empty() && idx == prefix.size();
			}

			//erase the word / string if it is in the trie
			bool erase(strv_type word)
			{
				bool result { false };

				if ( !word.empty() )
				{
					auto len  { getLen(word.data(), word.size()) };
					auto iter { root.find(word.data(), len) };

					if (iter != root.end())
					{
						auto pr { erase(word, len, iter) };

						if (pr.first)
						{
							root.erase(iter);

							--cnt;
						}

						result = pr.second;
					}
				}

				return result;
			}

			//use depth-first traversal and return all words / strings in the trie that prefixed by the function argument
			vector<string_type> traverse(strv_type prefix = strv_type()) const
			{
				string_type         str    {};
				vector<string_type> result {};

				if (prefix.empty())
				{
					for (auto iter { root.begin() }; iter != root.end(); ++iter)
					{
						auto count { iter->first.count() };

						str.append(iter->first.data(), count);

						traverse(iter, str, result);

						str.resize(str.size() - count);
					}
				}
				else
				{
					const_pointer  ptr  { prefix.data() };
					const_iterator iter {};
					int  idx { 0 }, len{ 0 };

					for (auto it { iter }; idx < prefix.size(); idx += len, ptr += len, iter = it)
					{
						auto& node { idx == 0 ? root : iter->second };

						len = getLen(ptr, prefix.size() - idx);

						it = node.find(ptr, len);

						if (it == node.end())
							break;
					}

					if (idx == prefix.size())
						traverse(iter, str = prefix, result);
				}

				return result;
			}

		private:
			//how many characters make it a valid character unit (especially for variable-width encoding form, ex: UTF-8 and UTF-16)
			int getLen(const_pointer ptr, size_type max_count) const
			{
				if (ptr == nullptr)
					return 0;
				else if (std::is_same_v<char_type, char> || std::is_same_v<char_type, char32_t>)
					return 1;
				else if (std::is_same_v<char_type, wchar_t> || std::is_same_v<char_type, char16_t>)			//Windows platform
				{
					return isUTF16_surrogate(*ptr) ? 2 : 1;
				}
				else			// char8_t
				{
					auto len = std::mblen(reinterpret_cast<const char*>(ptr), max_count);

					if (len <= 0)
						throw std::runtime_error("failed to call std::mblen()");

					return len;
				}
			}

			pair<bool, bool> erase(auto word, size_type idx, auto iter)
			{
				pair<bool, bool> result {};

				if (idx == word.size())
				{
					if (iter->second.isEndOfWord)
					{
						iter->second.isEndOfWord = false;

						--sz;

						result = std::make_pair(iter->second.empty(), true);
					}
				}
				else
				{
					auto ptr { word.data() + idx };
					auto len { getLen(ptr, word.size() - idx) };
					auto it  { iter->second.find(ptr, len) };

					if (it != iter->second.end())
					{
						result = erase(word, idx + len, it);

						if (result.first)
						{
							iter->second.erase(it);

							--cnt;

							result.first = !iter->second.isEndOfWord && iter->second.empty();
						}
					}
				}

				return result;
			}

			void traverse(auto iter, auto& str, auto& result) const
			{
				if (iter->second.isEndOfWord)
					result.push_back(str);

				for (auto it { iter->second.begin() }; it != iter->second.end(); ++it)
				{
					auto count { it->first.count() };

					str.append(it->first.data(), count);

					traverse(it, str, result);

					str.resize(str.size() - count);
				}
			}

			size_type  sz   { 0 };				//number of words / strings
			size_type  cnt  { 1 };				//number of trie nodes (including root)
			node_type  root {};					//the root trie node
	};

	template<class CharTy>
	bool operator == (const cust::char_unit<CharTy>& lhs, const cust::char_unit<CharTy>& rhs)
	{
		return lhs.c32 == rhs.c32;
	}
}

template<class CharTy>
struct std::hash<cust::char_unit<CharTy>>
{
	auto operator () (const cust::char_unit<CharTy>& cu) const noexcept
	{
		return std::hash<char32_t>{}(cu.c32);
	}
};
