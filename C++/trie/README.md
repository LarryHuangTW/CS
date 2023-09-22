> *It's not reinventing the wheel. Just for practice and for fun.*<br><br>
> *A much better and correct way is to use International Components for Unicode (ICU)*

### A ${\color{blue}trie}$ data structure that supports C++ standard library strings.
*including: std::string, std::wstring, std::u8string, std::u16string, std::u32string*<br><br>

| Type          | Definition                                  |
| ------------- | --------------------------------------------|
| cust::trie    | cust::basic_trie\<N, fst_ch, T, Allocator\> |
| cust::wtrie   | cust::uni_trie\<std::wstring\>              |
| cust::u8trie  | cust::uni_trie\<std::u8string\>             |
| cust::u16trie | cust::uni_trie\<std::u16string\>            |
| cust::u32trie | cust::uni_trie\<std::u32string\>            |

<br>

**Public member functions:**

(constructor)

(destructor)
```C++
bool empty() const noexcept;

size_type size() const noexcept;

size_type count() const noexcept;

void clear() noexcept;

bool insert(string_view_type word);

template<class... Args>
bool insert(std::string_view word, Args&&... args);

bool search(string_view_type word) const noexcept;

bool starts_with(string_view_type prefix) const noexcept;

bool erase(string_view_type word);

std::vector<string_type> traverse(string_view_type prefix = string_view_type()) const;
```


## Example:

```C++
#include <iostream>
#include <string>
#include <locale>
#include "trie.h"

int main(int argc, char* argv[])
{
	using namespace std::string_literals;

	cust::trie<26, 'a'> tr {};			//for std::string of 26 characters from 'a' to 'z'
	cust::wtrie  wtr  {};				//for std::wstring
	cust::u8trie u8tr {};				//for std::u8string

	//insert words into the string trie
	tr.insert("a"s);
	tr.insert("an"s);
	tr.insert("and"s);
	tr.insert("ann"s);
	tr.insert("ant"s);
	tr.insert("andy"s);
	tr.insert("anna"s);
	tr.insert("apple"s);
	tr.insert("apply"s);
	tr.insert("applicant"s);
	tr.insert("application"s);

	std::cout << "number of words : " << tr.size() << "\n";
	std::cout << "number of nodes : " << tr.count() << "\n";

	std::cout << std::boolalpha;
	std::cout << tr.search("and").first << "\n";
	std::cout << tr.starts_with("an") << "\n";
	
	//erase the word "ant" from the trie
	tr.erase("ant");

	//list all words in the trie
	auto vec { tr.traverse() };

	for (const auto& str : vec)
		std::cout << str << "\n";

	//insert words into the wstring trie
	wtr.insert(L"i"s);
	wtr.insert(L"in"s);
	wtr.insert(L"it"s);
	wtr.insert(L"ink"s);
	wtr.insert(L"inn"s);
	wtr.insert(L"inner"s);
	wtr.insert(L"tea"s);
	wtr.insert(L"tear"s);
	wtr.insert(L"teach"s);
	wtr.insert(L"teacher"s);

	//list all words prefixed by "in" in the trie
	auto wvec { wtr.traverse(L"in") };

	// ......
	
	std::setlocale(LC_ALL, "zh_TW.UTF-8");

	//insert words into the u8string trie
	u8tr.insert(u8"一"s);
	u8tr.insert(u8"二"s);
	u8tr.insert(u8"三"s);
	u8tr.insert(u8"一二"s);
	u8tr.insert(u8"一二三"s);
	u8tr.insert(u8"一三五七九"s);
	u8tr.insert(u8"二四六八十"s);

	//list all words prefixed by "一" in the trie
	auto u8vec { u8tr.traverse(u8"一")};

	// ......
}
```


## Tricks and code snippets:

```C++
namespace cust					//customized / non-standard
{
	template<std::size_t N, char fst_ch, class T = void, class Allocator = std::allocator<char>>
	using trie = basic_trie<N, fst_ch, T, Allocator>;

	using wtrie   = uni_trie<std::wstring>;
	using u8trie  = uni_trie<std::u8string>;
	using u16trie = uni_trie<std::u16string>;
	using u32trie = uni_trie<std::u32string>;

	template<std::size_t, class>
	struct basic_trie_node;

	template<std::size_t N>
	struct basic_trie_node<N, void>
	{
		using node_type    = basic_trie_node<N, void>;
		using node_pointer = node_type*;
		
		// ......
		
		std::array<node_pointer, N> children;
		bool isEndOfWord { false };
	};

	template<std::size_t N, class T = void>
	struct basic_trie_node : public basic_trie_node<N, void>
	{
		// ......
		
		T value {};
	};
	
	template<std::size_t N, char fst_ch, class T = void, class Allocator = std::allocator<char>>
	class basic_trie
	{
		public:
			// ......
			
		private:
			// ......
			
			size_type      sz    { 0 };				//number of words / strings
			size_type      cnt   { 1 };				//number of trie nodes (including root)
			node_type      root  {};				//the root trie node
			allocator_type alloc {};				//allocator object
	};

	template<class CharTy>
	struct trie_node;

	template<class CharTy>
	using trie_node_base = std::unordered_map<char_unit<CharTy>, trie_node<CharTy>>;

	/*	or we can use std::map
	template<class CharTy>
	using trie_node_base = std::map<char_unit<CharTy>, trie_node<CharTy>>;
	*/

	template<class CharTy>
	struct trie_node : trie_node_base<CharTy>
	{
		// ......
		
		bool isEndOfWord { false };
	};
	
	template<class T>
	class uni_trie
	{
		public:
			// ......
		
		private:
			size_type  sz   { 0 };				//number of words / strings
			size_type  cnt  { 1 };				//number of trie nodes (including root)
			node_type  root {};				//the root trie node
	};
}
```
