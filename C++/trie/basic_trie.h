#pragma once

#include <cstddef>
#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <stdexcept>

namespace cust					//customized / non-standard
{
	using std::size_t;
	using std::array;
	using std::allocator;
	using std::allocator_traits;
	using std::string;
	using std::string_view;
	using std::vector;
	using std::pair;
	using std::out_of_range;

	template<size_t, class>
	struct basic_trie_node;

	template<size_t N>
	struct basic_trie_node<N, void>
	{
		using node_type    = basic_trie_node<N, void>;
		using node_pointer = node_type*;

		basic_trie_node()
		{
			children.fill(nullptr);
		}

		bool empty() const noexcept
		{
			for (auto ptr : children)
				if (ptr != nullptr)
					return false;

			return true;
		}

		bool hasChar(size_t idx) const noexcept
		{
			return children[idx] != nullptr;
		}

		bool hasChar(node_pointer ptr) const noexcept
		{
			return ptr != nullptr;
		}

		node_pointer addChar(size_t idx, node_pointer ptr)
		{
			return children[idx] = ptr;
		}

		node_pointer next(size_t idx) const noexcept
		{
			return children[idx];
		}

		void removeChar(size_t idx)
		{
			children[idx] = nullptr;
		}

		array<node_pointer, N> children;
		bool isEndOfWord { false };
	};

	template<size_t N, class T = void>
	struct basic_trie_node : public basic_trie_node<N, void>
	{
		using base_type    = basic_trie_node<N, void>;
		using node_type    = basic_trie_node<N, T>;
		using node_pointer = node_type*;

		node_pointer addChar(size_t idx, node_pointer ptr)
		{
			return static_cast<node_pointer>(base_type::addChar(idx, ptr));
		}

		node_pointer next(size_t idx) const noexcept
		{
			return static_cast<node_pointer>(base_type::next(idx));
		}

		T getValue() const noexcept
		{
			return value;
		}

		void setValue(T&& val)
		{
			value = std::forward<T>(val);
		}

		void setValue(auto&&... args)
		{
			value = T(std::forward<decltype(args)>(args)...);
		}

		T value {};
	};

	template<size_t N, char fst_ch, class T = void, class Allocator = allocator<char>>
	class basic_trie
	{
		public:
			using char_type         = char;
			using string_type       = string;
			using value_type        = T;
			using node_type         = basic_trie_node<N, value_type>;
			using node_pointer      = node_type*;
			using size_type         = size_t;
			using allocator_type    = typename allocator_traits<Allocator>::template rebind_alloc<node_type>;
			using alloc_traits_type = allocator_traits<allocator_type>;

		public:
			//default constructor
			basic_trie() = default;

			//destructor
			~basic_trie()
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
				for (size_type i { 0 }; i < N; ++i)
				{
					if (root.hasChar(i))
					{
						clear_node(root.next(i));

						root.removeChar(i);
					}
				}
			}

			//insert a word / string into the trie
			template<class... Args>
			bool insert(string_view word, Args&&... args)
			{
				auto ptr    { std::addressof(root) };
				bool result { false };

				for (auto ch : word)
				{
					auto idx { get_index(ch) };

					if ( !ptr->hasChar(idx) )
						ptr->addChar(idx, new_node());

					ptr = ptr->next(idx);
				}

				if ( !word.empty() )
				{
					if constexpr (sizeof...(args) > 0 && !std::is_same_v<value_type, void> && std::is_constructible_v<value_type, Args...>)
						ptr->setValue(std::forward<Args>(args)...);

					if ( !ptr->isEndOfWord )
					{
						result = ptr->isEndOfWord = true;

						++sz;
					}
				}

				return result;
			}
			
			//check if a word / string in the trie
			auto search(string_view word) const noexcept
			{
				auto      ptr { std::addressof(root) };
				size_type len { 0 };							//length

				for (auto ch : word)
				{
					auto idx { get_index(ch) };

					if ( !ptr->hasChar(idx) )
						break;

					ptr = ptr->next(idx);

					++len;
				}

				auto flag { len == word.size() && ptr->isEndOfWord };

				if constexpr (std::is_same_v<value_type, void>)
					return std::make_pair(flag, flag);
				else
					return std::make_pair(flag, ptr->getValue());
			}

			//check if there is any word / string in the trie prefixed by the function argument
			bool starts_with(string_view prefix) const noexcept
			{
				auto      ptr { std::addressof(root) };
				size_type len { 0 };							//length

				for (auto ch : prefix)
				{
					auto idx { get_index(ch) };

					if ( !ptr->hasChar(idx) )
						break;

					ptr = ptr->next(idx);

					++len;
				}

				return !prefix.empty() && len == prefix.size();
			}

			//erase the word / string if it is in the trie
			bool erase(string_view word)
			{
				bool result { false };

				if ( !word.empty() )
				{
					auto idx { get_index(word.front()) };

					if (root.hasChar(idx))
					{
						auto pr { erase(word, 1, root.next(idx)) };

						if (pr.first)
						{
							delete_node(root.next(idx));

							root.removeChar(idx);
						}

						result = pr.second;
					}
				}

				return result;
			}

			//use depth-first traversal and return all words / strings in the trie that prefixed by the function argument
			auto traverse(string_view prefix = string_view()) const
			{
				auto        ptr { std::addressof(root) };
				string_type str {};
				vector<std::conditional_t<std::is_same_v<value_type, void>, string_type, pair<string_type, value_type>>> result {};

				for (auto ch : prefix)
				{
					auto idx { get_index(ch) };

					if ( !ptr->hasChar(idx) )
						break;

					ptr = ptr->next(idx);
					str.push_back(ch);
				}

				if (str == prefix)
					traverse(ptr, str, result);

				return result;
			}

		private:
			//get the array index of the character
			size_type get_index(char_type ch) const
			{
				auto idx { static_cast<size_type>(ch - fst_ch) };

				if (N <= idx)
					throw out_of_range(string("index of character ").append(1, ch).append(" is out of range!"));

				return idx;
			}

			//new a trie node by allocating memory space and call its constructor
			node_pointer new_node()
			{
				auto ptr { alloc_traits_type::allocate(alloc, 1) };

				alloc_traits_type::construct(alloc, ptr);

				++cnt;

				return ptr;
			}

			//clear a trie node and all its child nodes
			void clear_node(node_pointer ptr) noexcept
			{
				for (auto p : ptr->children)
					if (p != nullptr)
						clear_node(p);

				if (ptr->isEndOfWord)
					--sz;

				delete_node(ptr);
			}

			//delete a trie node by call its destructor and deallocating memory space
			void delete_node(node_pointer ptr) noexcept
			{
				alloc_traits_type::destroy(alloc, ptr);
				alloc_traits_type::deallocate(alloc, ptr, 1);

				--cnt;
			}

			/*
			 *		erase the word / string and clear unused trie nodes
			 *
			 *		pair.first  : if deleted a trie node or not
			 *		pair.second : if erased the word / string successfully or not
			 */
			pair<bool, bool> erase(string_view word, size_type i, node_pointer ptr)
			{
				pair<bool, bool> result {};

				if (i == word.size())
				{
					if (ptr->isEndOfWord)
					{
						ptr->isEndOfWord = false;

						--sz;

						result = std::make_pair(ptr->empty(), true);
					}
				}
				else
				{
					auto idx { get_index(word[i]) };

					if (ptr->hasChar(idx))
					{
						result = erase(word, i + 1, ptr->next(idx));

						if (result.first)
						{
							delete_node(ptr->next(idx));

							ptr->removeChar(idx);

							result.first = !ptr->isEndOfWord && ptr->empty();
						}
					}
				}

				return result;
			}

			//get all words / strings from the trie node which pointed by the pointer ptr
			void traverse(auto ptr, string& str, auto& result) const
			{
				if (ptr->isEndOfWord)
				{
					if constexpr (std::is_same_v<value_type, void>)
						result.push_back(str);
					else
						result.push_back(std::make_pair(str, ptr->getValue()));
				}

				for (int i { 0 }; i < N; ++i)
				{
					if (ptr->children[i] != nullptr)
					{
						str.push_back(static_cast<char_type>(fst_ch + i));

						traverse(ptr->next(i), str, result);

						str.pop_back();
					}
				}
			}

			size_type      sz    { 0 };				//number of words / strings
			size_type      cnt   { 1 };				//number of trie nodes (including root)
			node_type      root  {};				//the root trie node
			allocator_type alloc {};				//allocator object
	};
}
