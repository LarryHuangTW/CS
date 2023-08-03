# The design of iterators of C++ data containers

> Take list data container and its bidirectional iterator for example

### version 1: two separate class templates with 90% code similarity

```C++
template<class T>
class list_iterator
{
	public:
		// ......
		using pointer   = value_type*;
		using reference = value_type&;

		list_iterator() noexcept;
		list_iterator(node_pointer p) noexcept;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;

		bool operator == (const list_iterator& other) const noexcept;
		bool operator != (const list_iterator& other) const noexcept;

	private:
		node_pointer ptr { nullptr };
};

template<class T>
class list_const_iterator
{
	public:
		// ......
		using pointer   = const value_type*;
		using reference = const value_type&;

		list_const_iterator() noexcept;
		list_const_iterator(node_pointer p) noexcept;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;

		bool operator == (const list_const_iterator& other) const noexcept;
		bool operator != (const list_const_iterator& other) const noexcept;

	private:
		node_pointer ptr { nullptr };
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<value_type>;
		using const_iterator = list_const_iterator<value_type>;

		// ......
};
```

### version 2: two class templates with hierarchical relationships

```C++
template<class T>
class list_iterator
{
	public:
		// ......
		using pointer   = value_type*;
		using reference = value_type&;

		list_iterator() noexcept;
		list_iterator(node_pointer p) noexcept;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;

		bool operator == (const list_iterator& other) const noexcept;
		bool operator != (const list_iterator& other) const noexcept;

	protected:
		node_pointer ptr { nullptr };
};

template<class T>
class list_const_iterator : public list_iterator<T>
{
	public:
		using base_type = list_iterator<T>;
		// ......
		using pointer   = const value_type*;
		using reference = const value_type&;

		using base_type::base_type;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<value_type>;
		using const_iterator = list_const_iterator<value_type>;

		// ......
};
```

### version 3: use one class template to implement iterator and const_iterator

```C++
template<bool const_flag, class node_type>
class list_iterator
{
	public:
		// ......
		using pointer   = std::conditional_t<const_flag, const value_type*, value_type*>;
		using reference = std::conditional_t<const_flag, const value_type&, value_type&>;

		list_iterator() noexcept;
		list_iterator(node_pointer p) noexcept;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;

		bool operator == (const list_iterator& other) const noexcept;
		bool operator != (const list_iterator& other) const noexcept;

	private:
		node_pointer ptr { nullptr };
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<false, node_type>;
		using const_iterator = list_iterator<true,  node_type>;

		// ......
};
```

### version 4: use one class template to implement iterator and const_iterator

```C++
template<class T, template<class...> class NodeTy>
class list_iterator
{
	public:
		// ......
		/*
		 *	from this version, value type may differ from previous versions or 
		 *	even C++ standard, but it makes sense to some extent.
		 */
		using value_type = T;
		using pointer    = value_type*;
		using reference  = value_type&;

		list_iterator() noexcept;
		list_iterator(node_pointer p) noexcept;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;

		bool operator == (const list_iterator& other) const noexcept;
		bool operator != (const list_iterator& other) const noexcept;

	private:
		node_pointer ptr { nullptr };
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<value_type,       list_node>;
		using const_iterator = list_iterator<const value_type, list_node>;

		// ......
};
```

### version 5: two class templates with hierarchical relationships

```C++
template<class NodePtr>
class iterator_base
{
	public:
		using node_pointer = NodePtr;

		iterator_base() noexcept;
		iterator_base(node_pointer p) noexcept;

		bool operator == (const iterator_base& other) const noexcept;
		bool operator != (const iterator_base& other) const noexcept;

	protected:
		node_pointer ptr { nullptr };
};

template<class T, template<class...> class NodeTy, class NodePtr = NodeTy<std::remove_const_t<T>>*>
class list_iterator : public iterator_base<NodePtr>
{
	public:
		using base_type         = iterator_base<NodePtr>;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type        = T;
		using difference_type   = std::ptrdiff_t;
		using pointer           = value_type*;
		using reference         = value_type&;

		using base_type::base_type;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<value_type,       list_node>;
		using const_iterator = list_iterator<const value_type, list_node>;

		// ......
};
```

### version 6: three class templates with hierarchical relationships

```C++
template<class NodePtr>
class iterator_base
{
	public:
		using node_pointer = NodePtr;

		iterator_base() noexcept;
		iterator_base(node_pointer p) noexcept;

		bool operator == (const iterator_base& other) const noexcept;
		bool operator != (const iterator_base& other) const noexcept;

	protected:
		node_pointer ptr { nullptr };
};

template<class T, template<class...> class NodeTy, class NodePtr = NodeTy<std::remove_const_t<T>>*>
class container_iterator : public iterator_base<NodePtr>
{
	public:
		using base_type       = iterator_base<NodePtr>;
		using value_type      = T;
		using difference_type = std::ptrdiff_t;
		using pointer         = value_type*;
		using reference       = value_type&;

		using base_type::base_type;

		reference operator *  () const noexcept;
		pointer   operator -> () const noexcept;
};

template<class T, template<class...> class NodeTy>
class list_iterator : public container_iterator<T, NodeTy>
{
	public:
		using base_type         = container_iterator<T, NodeTy>;
		using iterator_category = std::bidirectional_iterator_tag;

		using base_type::base_type;

		auto& operator ++ ()    noexcept;
		auto  operator ++ (int) noexcept;
		auto& operator -- ()    noexcept;
		auto  operator -- (int) noexcept;
};

template<class T, class Allocator = allocator<T>>
class list
{
	public:
		using iterator       = list_iterator<value_type,       list_node>;
		using const_iterator = list_iterator<const value_type, list_node>;

		// ......
};
```
