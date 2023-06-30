#pragma once

#include <iostream>
#include <cstddef>
#include <array>
#include <stack>
#include <iomanip>
#include <string>
#include <utility>
#include <type_traits>

namespace cust					//customized / non-standard
{
	//Tower of Hanoi with N rods
	template<std::size_t N>
	class Tower_of_Hanoi
	{
		// ......
	};

	template<>
	class Tower_of_Hanoi<3>
	{
		public:
			using size_type = std::size_t;

			//get the count of steps
			size_type getSteps() const noexcept
			{
				return steps;
			}

			//get the logs of steps
			const auto& getLogs() const noexcept
			{
				return logs;
			}

		protected:
			enum class rod_t : unsigned char { src, tmp, dest };		//rod types: source, temporary, destination

			//Initialize the rod stacks
			void initialize(size_type n)
			{
				//clear each rod
				for (size_type i { 0 }; i < 3; ++i)
					for ( ; !stks[i].empty(); stks[i].pop());

				//initialize source rod
				for (size_type i { n }; 0 < i; --i)
					stks[0].push(i);

				steps = 0;
				logs.clear();
			}

			void move_one_disk(rod_t from, rod_t to)
			{
				auto i { static_cast<size_type>(from) }, j { static_cast<size_type>(to) };

				std::cout << "move disk " << std::setw(3) << stks[i].top() << " from rod " << rod_symbol[i] << " to rod " << rod_symbol[j] << "\n";

				logs.append(std::to_string(stks[i].top())).append(1, rod_symbol[i]).append(1, rod_symbol[j]);

				stks[j].push(stks[i].top());
				stks[i].pop();

				++steps;
			}

			char rod_symbol[3] { 'A' , 'B' , 'C' };			//A: source , B: temporary , C: destination
			std::array<std::stack<size_type>, 3> stks {};		//rod stacks
			size_type   steps { 0 };				//count of steps
			std::string logs  {};					//logs  of steps
	};

	namespace recursive_version
	{
		class hanoi_game : public cust::Tower_of_Hanoi<3>
		{
			public:
				void move(size_type n)
				{
					if (0 < n)
					{
						initialize(n);

						move(n, rod_t::src, rod_t::tmp, rod_t::dest);
					}
				}

			private:
				void move(size_type n, rod_t source, rod_t temporary, rod_t destination)
				{
					if (1 < n) move(n - 1, source, destination, temporary);

					move_one_disk(source, destination);

					if (1 < n) move(n - 1, temporary, source, destination);
				}
		};
	}

	namespace non_recursive_version
	{
		class hanoi_game : public cust::Tower_of_Hanoi<3>
		{
			private:
				struct stk_val
				{
					size_type n;
					rod_t     src;
					rod_t     tmp;
					rod_t     dest;
				};

			public:
				void move(size_type n)
				{
					std::stack<stk_val> stk {};

					initialize(n);

					if (0 < n)
						stk.emplace(n, rod_t::src, rod_t::tmp, rod_t::dest);

					while ( !stk.empty() )
					{
						if (stk.top().n == 1)
						{
							move_one_disk(stk.top().src, stk.top().dest);

							stk.pop();
						}
						else
						{
							const auto bk { stk.top() };		//backup of stack's top value

							--stk.top().n;
							stk.top().src = bk.tmp;
							stk.top().tmp = bk.src;

							stk.emplace(1,        bk.src, bk.tmp,  bk.dest);
							stk.emplace(bk.n - 1, bk.src, bk.dest, bk.tmp);
						}
					}
				}
		};
	}

	//check if the integer is odd or not
	template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	constexpr bool isOdd(T n)
	{
		return (n & 1) == 1;
	}

	namespace iterative
	{
		class hanoi_game : public cust::Tower_of_Hanoi<3>
		{
			public:
				void move(size_type n)
				{
					const bool flag { cust::isOdd(n) };
					const std::array<std::pair<rod_t, rod_t>, 3> ary {{	{ rod_t::src , flag ? rod_t::dest : rod_t::tmp  } , 
												{ rod_t::src , flag ? rod_t::tmp  : rod_t::dest } , 
												{ rod_t::tmp , rod_t::dest } }};

					initialize(n);

					for (int i { 0 }; stks[2].size() != n; i = ++i % 3)
						legal_move(ary[i].first, ary[i].second);
				}

			private:
				void legal_move(rod_t a, rod_t b)
				{
					auto i { static_cast<size_type>(a) }, j { static_cast<size_type>(b) };

					if (stks[i].empty() || ( !stks[j].empty() && stks[j].top() < stks[i].top() ))
						std::swap(a, b);

					move_one_disk(a, b);		//move one disk from rod a to rod b
				}
		};
	}
}
