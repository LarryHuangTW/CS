> *It's not reinventing the wheel. Just for practice and for fun.*<br>

## Code snippets:

```C++
namespace cust					//customized / non-standard
{
	//a container adaptor with first-in, first-out (FIFO) property
	template<class T, class Container = std::deque<T>>
	class queue
	{
		public:
			using container_type  = Container;
			using value_type      = typename container_type::value_type;
			using size_type       = typename container_type::size_type;
			using reference       = typename container_type::reference;
			using const_reference = typename container_type::const_reference;

			//constructors
			queue();
			explicit queue(const container_type& cont);
			explicit queue(container_type&& cont);
			explicit queue(const queue& other);
			explicit queue(queue&& other) noexcept;

			template<class InputIter>
			queue(InputIter first, InputIter last);

			//destructor
			~queue();

			[[nodiscard]] bool empty() const;			//checks if the queue is empty
			size_type size() const;					//return number of elements in the queue
			reference front();					//access the first element of the queue
			const_reference front() const;
			reference back();					//access the last  element of the queue
			const_reference back() const;
			void push(const value_type& value);			//add an element to the end of the queue
			void push(value_type&& value);
			void pop();						//remove an element from the front of the queue

			template<class... Args>
			decltype(auto) emplace(Args&&... args);			//construct an element at the end of the queue

			void swap(queue& other) noexcept;			//swap the elements with other queue

		protected:
			container_type c {};
	};
}
```

## Example:

```C++
#include <iostream>
#include <list>
#include "queue.h"

template<class T, class Container>
decltype(auto) operator << (std::ostream& os, const cust::queue<T, Container>& que)
{
	os << "empty ? " << que.empty() << "\t\tsize : " << que.size();

	if ( !que.empty() )
		os << "\t\tfront : " << que.front() << "\t\tback : " << que.back();

	os << "\n\n";

	return os;
}

template<class T, class Container>
void dump(cust::queue<T, Container>& que)
{
	for ( ; !que.empty(); que.pop())
		std::cout << que.front() << "   ";
	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	std::cout << std::boolalpha;

	cust::queue<int> que1 {};

	std::cout << "queue 1 : " << que1;

	que1.push(1);
	que1.push(2);
	que1.push(3);

	std::cout << "queue 1 : " << que1;

	cust::queue<int> que2 { que1 };

	std::cout << "queue 2 : " << que2;

	std::cout << "queue 1 == queue 2 ? " << (que1 == que2) << "\n\n";

	for ( ; !que1.empty(); que1.pop());

	std::cout << "queue 1 : " << que1;

	std::cout << "queue 1 == queue 2 ? " << (que1 == que2) << "\n\n";

	que1.push(7);
	que1.push(8);
	que1.push(9);

	std::cout << "queue 1 : " << que1;
	std::cout << "queue 2 : " << que2;

	std::cout << "swap queue 1 and 2\n\n";

	que1.swap(que2);

	std::cout << "queue 1 : ";
	dump(que1);

	std::cout << "queue 2 : ";
	dump(que2);

	cust::queue<std::string> que3 {};

	que3.emplace("Hi");
	que3.emplace("Hello");
	que3.emplace("World");

	std::cout << "queue 3 : " << que3;

	cust::queue<std::string> que4 { std::move(que3) };

	std::cout << "queue 3 : " << que3;
	std::cout << "queue 4 : " << que4;

	std::cout << "queue 4 : ";
	dump(que4);

	cust::queue<double, std::list<double>> que5 {};

	que5.push(1.1);
	que5.push(2.2);
	que5.push(3.3);

	std::cout << "queue 5 : ";
	dump(que5);

	// ......

	return 0;
}
```
