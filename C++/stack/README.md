> *It's not reinventing the wheel. Just for practice and for fun.*<br>

## Code snippets:

```C++
namespace cust
{
	using std::deque;
	using std::forward_list;

	//implementation of stack container adaptor
	template<class T, class Container>
	class stack_impl
	{
		public:
			using container_type  = Container;
			using value_type      = container_type::value_type;
			using size_type       = container_type::size_type;
			using reference       = container_type::reference;
			using const_reference = container_type::const_reference;

			//constructors
			stack_impl();
			explicit stack_impl(const container_type& cont);
			explicit stack_impl(container_type&& cont);
			explicit stack_impl(const stack_impl& other);
			explicit stack_impl(stack_impl&& other) noexcept;

			template<class InputIter>
			stack_impl(InputIter first, InputIter last);

			//destructor
			~stack_impl();

			[[nodiscard]] bool empty() const;				//checks if the stack is empty
			size_type size() const;						//return number of elements in the stack
			reference top();						//access the top element of the stack
			const_reference top() const;
			void push(const value_type& value);				//add an element at the top of the stack
			void push(value_type&& value);
			void pop();							//remove the top element of the stack

			template<class... Args>
			decltype(auto) emplace(Args&&... args);				//construct an element at the top of the stack

			void swap(stack_impl& other) noexcept;				//swap the elements with other stack

		protected:
			container_type c {};
	};

	//a container adaptor with last-in, first-out (LIFO) property
	template<class T, class Container = deque<T>>
	class stack : public stack_impl<T, Container>
	{
		// ......
	};

	//specialization of class template stack<T, Container>
	template<class T>
	class stack<T, forward_list<T>> : public stack_impl<T, forward_list<T>>
	{
		// ......
	};
}
```

## Example:

```C++
#include <iostream>
#include "stack.h"

template<class T, class Container>
decltype(auto) operator << (std::ostream& os, const cust::stack<T, Container>& stk)
{
	os << "empty ? " << stk.empty() << "\t\tsize : " << stk.size();

	if ( !stk.empty() )
		os << "\t\ttop : " << stk.top();

	os << "\n\n";

	return os;
}

template<class T, class Container>
void dump(cust::stack<T, Container>& stk)
{
	for ( ; !stk.empty(); stk.pop())
		std::cout << stk.top() << "   ";
	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	std::cout << std::boolalpha;

	cust::stack<int> stk1 {};

	std::cout << "stack 1 : " << stk1;

	stk1.push(3);
	stk1.push(2);
	stk1.push(1);

	std::cout << "stack 1 : " << stk1;

	cust::stack<int> stk2 { stk1 };

	std::cout << "stack 2 : " << stk2;

	std::cout << "stack 1 == stack 2 ? " << (stk1 == stk2) << "\n\n";

	stk1.pop();
	stk1.pop();
	stk1.pop();

	std::cout << "stack 1 : " << stk1;

	std::cout << "stack 1 == stack 2 ? " << (stk1 == stk2) << "\n\n";

	cust::stack<std::string> stk3 {};

	stk3.emplace("World");
	stk3.emplace("Hello");
	stk3.emplace("Hi");

	std::cout << "stack 3 : " << stk3;

	cust::stack<std::string> stk4 { std::move(stk3) };

	std::cout << "stack 3 : " << stk3;
	std::cout << "stack 4 : " << stk4;

	std::cout << "stack 1 : ";
	dump(stk1);

	std::cout << "stack 2 : ";
	dump(stk2);

	std::cout << "stack 3 : ";
	dump(stk3);

	std::cout << "stack 4 : ";
	dump(stk4);

	cust::stack<double, std::forward_list<double>> stk5 {};

	stk5.push(3.3);
	stk5.push(2.2);
	stk5.push(1.1);

	std::cout << "stack 5 : " << stk5;

	stk5.pop();
	stk5.pop();
	stk5.pop();

	std::cout << "stack 5 : " << stk5;

	// ......

	return 0;
}

```
