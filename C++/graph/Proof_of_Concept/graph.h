#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <utility>
#include <string>
#include <type_traits>
#include "adjacency_list.h"

namespace cust					//customized / non-standard
{
	//graph data structures and algorithms
	template<class impl_type>
	class graph : public impl_type
	{
		public:
			using vertex_type      = typename impl_type::vertex_type;		//vertex type
			using edge_weight_type = typename impl_type::edge_weight_type;		//edge weight type
			using edge_type        = typename impl_type::edge_type;			//edge type
			using size_type        = typename impl_type::size_type;			//size type

			//vertex visited state
			enum class vertex_state : unsigned char { unvisited, in_path, visited };

			struct visit_type
			{
				vertex_type  predecessor {};
				vertex_state state { vertex_state::unvisited };
			};

			//checks if the graph has any loop
			bool has_loop() const noexcept
			{
				for (auto iter { this->vbegin() }; iter != this->vend(); ++iter)
				{
					const auto& edges { this->get_edges(*iter) };

					for (const auto& edge : edges)
						if (edge.neighbor == *iter)
							return true;
				}

				return false;
			}

			//checks if the graph has any cycle
			bool has_cycle() const
			{
				std::unordered_map<vertex_type, visit_type> visited {};
				bool result { false };

				for (auto iter { this->vbegin() }; iter != this->vend(); ++iter)
				{
					if ( !visited.contains(*iter) )
					{
						visited.try_emplace(*iter, *iter, vertex_state::in_path);

						if (has_cycle(*iter, *iter, visited))
						{
							result = true;

							break;
						}
					}
				}

				return result;
			}

			//checks if the graph is a directed acyclic graph (DAG)
			bool is_DAG() const
			{
				return !this->is_undirected && !has_cycle();
			}

			//depth-first search (DFS)
			std::vector<vertex_type> depth_first_search(const vertex_type& src, const vertex_type& dest) const
			{
				std::vector<vertex_type> path {};
				std::unordered_set<vertex_type> visited {};

				if (this->check_vertex(src) && this->check_vertex(dest))
				{
					path.push_back(src);
					visited.insert(src);

					depth_first_search(dest, path, visited);
				}

				return path;
			}

			//breadth-first search (BFS)
			std::vector<vertex_type> breadth_first_search(const vertex_type& src, const vertex_type& dest) const
			{
				std::queue<vertex_type>  que  {};
				std::unordered_map<vertex_type, vertex_type> visited {};
				std::vector<vertex_type> path {};

				if ( !this->check_vertex(src) || !this->check_vertex(dest) )
					return path;

				que.push(src);
				visited.emplace(src, src);

				while ( !que.empty() )
				{
					const auto  curr  { std::move(que.front()) };		//current vertex
					const auto& edges { this->get_edges(curr)  };		//edges of current vertex

					que.pop();

					for (const auto& edge : edges)
					{
						//the neighbor of the current vertex is not visited
						if (visited.emplace(edge.neighbor, curr).second)
						{
							//the neighbor of the current vertex is the destination vertex
							if (edge.neighbor == dest)
							{
								//clears the queue to exit the outer loop
								for ( ; !que.empty(); que.pop());

								break;
							}
							else
								que.push(edge.neighbor);
						}
					}
				}

				convert_path(dest, visited, path);

				return path;
			}

			/*
			 *		topological sort / task scheduling
			 *
			 *		if flag is true,  edge e = (u , v) means vertex v is a prerequisite/precondition of vertex u, or u depends on v
			 *
			 *		if flag is false, edge e = (u , v) means vertex u is a prerequisite/precondition of vertex v, or v depends on u
			 */
			std::vector<vertex_type> topological_sort(bool flag = true) const
			{
				std::unordered_set<vertex_type> visited {};
				std::vector<vertex_type>        result  {};

				//performs topological sort if and only if the graph is a directed acyclic graph
				if (is_DAG())
				{
					result.reserve(this->vertex_count());

					for (auto iter { this->vbegin() }; iter != this->vend(); ++iter)
						if (visited.insert(*iter).second)
							topological_sort(*iter, visited, result);

					if ( !flag )
						std::reverse(result.begin(), result.end());
				}

				return result;
			}

			//shortest path search (Dijkstra and Bellman-Ford algorithm)
			auto shortest_path_search(const vertex_type& src, const vertex_type& dest, bool allow_negative_weight = false) const
			{
				std::vector<vertex_type> path {};		//the shortest path        from source vertex to destination vertex
				edge_weight_type         w    {};		//the minimum  path weight from source vertex to destination vertex

				if (this->check_vertex(src) && this->check_vertex(dest))
				{
					auto visited { allow_negative_weight ? bellman_ford(src, dest) : dijkstra(src, dest) };
					auto iter    { visited.find(dest) };

					if (iter != visited.end())
						w = iter->second.weight;

					convert_path(dest, visited, path);
				}

				return std::make_pair(std::move(path), std::move(w));
			}

			//finds minimum spanning tree (Prim's algorithm)
			edge_weight_type minimum_spanning_tree() const
			{
				using pq_value_type = std::pair<vertex_type, edge_type>;		//value type of priority queue

				auto lambda = [](const auto& lhs, const auto& rhs) { return rhs.second.weight < lhs.second.weight; };

				std::priority_queue<pq_value_type, std::vector<pq_value_type>, decltype(lambda)> pque {};		//min heap
				std::unordered_map<vertex_type, edge_type> visited {};
				edge_weight_type result {};

				if ( !this->empty() )
					pque.emplace(*this->vbegin(), *this->vbegin());

				while(visited.size() != this->vertex_count() && !pque.empty())
				{
					auto pr { visited.insert(std::move(pque.top())) };

					pque.pop();

					if (pr.second)
					{
						const auto& edges { this->get_edges(pr.first->first) };

						result += pr.first->second.weight;

						for (const auto& edge : edges)
						{
							if ( !visited.contains(edge.neighbor) )
								pque.emplace(std::piecewise_construct, std::forward_as_tuple(edge.neighbor), std::forward_as_tuple(pr.first->first, edge.weight));
						}
					}
				}

				return result;
			}

		private:
			//checks if the graph has any cycle using depth-first traversal
			bool has_cycle(const vertex_type& curr, const vertex_type& prev, std::unordered_map<vertex_type, visit_type>& visited) const
			{
				const auto& edges { this->get_edges(curr) };

				for (const auto& edge : edges)
				{
					//a loop is a cycle
					if (edge.neighbor == curr)
						return true;

					//for undirected graphs, an edge with two different vertices is not a cycle
					if (this->is_undirected && edge.neighbor == prev)
						continue;

					auto iter { visited.find(edge.neighbor) };

					if (iter == visited.end())
					{
						visited.try_emplace(edge.neighbor, curr, vertex_state::in_path);

						if (has_cycle(edge.neighbor, curr, visited))
							return true;
					}
					else
					{
						//detects a cycle
						if (iter->second.state == vertex_state::in_path)
							return true;
					}
				}

				//marks the vertex as visited
				visited.find(curr)->second.state = vertex_state::visited;

				return false;
			}

			//depth-first search (DFS)
			void depth_first_search(const vertex_type& dest, std::vector<vertex_type>& path, std::unordered_set<vertex_type>& visited) const
			{
				if (path.back() == dest)
					return;

				const auto& edges { this->get_edges(path.back()) };

				//this is not necessary but we can use the principle of locality to check if the destination vertex is the neighbor
				auto iter { std::find_if(edges.cbegin(), edges.cend(), [&dest](const auto& edge) { return edge.neighbor == dest; }) };

				if (iter != edges.cend())
				{
					path.push_back(iter->neighbor);

					return;
				}

				for (const auto& edge : edges)
				{
					if (visited.insert(edge.neighbor).second)
					{
						path.push_back(edge.neighbor);

						depth_first_search(dest, path, visited);

						if (path.back() == dest)
							break;
					}
				}

				if (path.back() != dest)
					path.pop_back();
			}

			//topological sort / task scheduling
			void topological_sort(const vertex_type& curr, std::unordered_set<vertex_type>& visited, std::vector<vertex_type>& result) const
			{
				const auto& edges { this->get_edges(curr) };

				for (const auto& edge : edges)
					if (visited.insert(edge.neighbor).second)
						topological_sort(edge.neighbor, visited, result);

				result.push_back(curr);
			}

			//shortest path search (Dijkstra's algorithm)
			auto dijkstra(const vertex_type& src, const vertex_type& dest) const
			{
				using pq_value_type = std::pair<vertex_type, edge_type>;		//value type of priority queue

				auto lambda = [](const auto& lhs, const auto& rhs) { return rhs.second.weight < lhs.second.weight; };

				std::priority_queue<pq_value_type, std::vector<pq_value_type>, decltype(lambda)> pque {};		//min heap
				std::unordered_map<vertex_type, edge_type> visited {};

				pque.emplace(src, src);

				while ( !pque.empty() )
				{
					auto pr { visited.insert(std::move(pque.top())) };

					pque.pop();

					//reaches destination vertex
					if (pr.first->first == dest)
						break;

					//first-time visitation of the vertex
					if (pr.second)
					{
						const auto& edges { this->get_edges(pr.first->first) };

						for (const auto& edge : edges)
						{
							//the neighbor is not visited
							if ( !visited.contains(edge.neighbor) )
								pque.emplace(std::piecewise_construct, std::forward_as_tuple(edge.neighbor), std::forward_as_tuple(pr.first->first, pr.first->second.weight + edge.weight));
						}
					}
				}

				return visited;
			}

			//shortest path search (Bellman-Ford algorithm)
			auto bellman_ford(const vertex_type& src, const vertex_type& dest) const
			{
				std::unordered_map<vertex_type, edge_type> visited {};		//stores visited/reachable vertices, their predecessors, and path weights from source vertex to each of them
				std::unordered_map<vertex_type, size_type> rounds  {};		//stores which round the path weight has been updated of each vertex

				visited.try_emplace(src, src);
				rounds.try_emplace(src, 0);

				//total |V| rounds
				for (size_type round { 1 }; round <= this->vertex_count(); ++round)
				{
					bool relaxed { false };			//is there any relaxation operation of each round

					for (auto& pr : visited)
					{
						//skip the vertex if it did not perform any relaxation in the last round
						if (rounds.find(pr.first)->second + 1 < round)
							continue;

						const auto& edges { this->get_edges(pr.first) };

						for (const auto& edge : edges)
						{
							auto iter { visited.find(edge.neighbor)    };
							auto w    { pr.second.weight + edge.weight };

							//the neighbor of the current vertex is not visited
							if (iter == visited.end())
							{
								visited.try_emplace(edge.neighbor, pr.first, w);
								rounds.try_emplace(edge.neighbor, round);

								relaxed = true;
							}
							else
							{
								//the path weight from source vertex to this can be relaxed
								if (w < iter->second.weight)
								{
									//a negative weight cycle is detected
									if (round == this->vertex_count())
									{
										std::cout << "a negative weight cycle is detected!\n";

										visited.erase(dest);

										return visited;
									}

									//relax
									iter->second.neighbor = pr.first;
									iter->second.weight   = w;

									rounds.find(edge.neighbor)->second = round;

									relaxed = true;
								}
							}
						}
					}

					//early termination if there is no relaxation of this round
					if ( !relaxed )
						break;
				}

				return visited;
			}

			//converts a resulting path from data container std::unordered_map to std::vector
			template<class T>
			void convert_path(const vertex_type& dest, std::unordered_map<vertex_type, T>& visited, std::vector<vertex_type>& path) const
			{
				auto iter { visited.find(dest) };

				if constexpr (std::is_same_v<T, vertex_type>)
				{
					for ( ; iter != visited.end() && iter->first != iter->second; iter = visited.find(iter->second))
						path.push_back(std::move(iter->first));
				}
				else
				{
					for ( ; iter != visited.end() && iter->first != iter->second.neighbor; iter = visited.find(iter->second.neighbor))
						path.push_back(std::move(iter->first));
				}

				if (iter != visited.end())
					path.push_back(std::move(iter->first));

				std::reverse(path.begin(), path.end());
			}
	};
}
