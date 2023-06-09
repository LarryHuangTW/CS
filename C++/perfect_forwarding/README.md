**Rvalue references solve two problems:**

1. Implementing move semantics

2. Perfect forwarding

<br>

## Examples:

```C++
template<class T>
void func(T&& param)
{
	// ......

	someFunc(std::forward<T>(param);

	// ......
}

template<class... Args>
std::vector<T>.emplace(const_iterator pos, Args&&... args);

template<class... Args>
std::vector<T>.emplace_back(Args&&... args);

template<class... Args>
std::forward_list<T>.emplace_after(const_iterator pos, Args&&... args);

template<class... Args>
std::forward_list<T>.emplace_front(Args&&... args);

template<class... Args>
std::list<T>.emplace(const_iterator pos, Args&&... args);

template<class... Args>
std::list<T>.emplace_front(Args&&... args);

template<class... Args>
std::list<T>.emplace_back(Args&&... args);

template<class... Args>
std::deque<T>.emplace(const_iterator pos, Args&&... args);

template<class... Args>
std::deque<T>.emplace_front(Args&&... args);

template<class... Args>
std::deque<T>.emplace_back(Args&&... args);

template<class T, class... Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
unique_ptr<T> make_unique(Args&&... args)
{
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T, class... Args>
shared_ptr<T> make_shared(Args&&... args);
```

![perfect forwarding](/Images/perfect_forwarding.svg)
