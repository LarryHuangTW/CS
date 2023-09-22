#pragma once

#include <iostream>
#include <cstddef>
#include <array>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <type_traits>

namespace cust					//customized / non-standard
{
	//directionality of graphs
	enum class edges_are : bool { directed, undirected };

	//for unweighted graphs
	struct unweighted_type
	{
	};

	template<class T, class U>
	struct neighbor_type;

	template<class T>
	struct neighbor_type<T, unweighted_type>
	{
		T neighbor {};
	};

	template<class T, class U>
	struct neighbor_type : public neighbor_type<T, unweighted_type>
	{
		using base_type = neighbor_type<T, unweighted_type>;

		neighbor_type(const T& v, U w = U{}) : base_type { v }, weight { w }
		{
		}

		U weight {};
	};

	//graph data structure (adjacency list)
	template<std::size_t N, class EdgeWeightTy, edges_are dire = edges_are::undirected>
	class adjacency_list
	{
		public:
			static constexpr bool is_undirected { dire == edges_are::undirected };

			using vertex_type      = std::size_t;										//vertex type
			using edge_weight_type = EdgeWeightTy;										//edge weight type
			using size_type        = std::size_t;										//size  type
			using edge_type        = neighbor_type<vertex_type, edge_weight_type>;		//edge  type
			using edges_type       = std::vector<edge_type>;							//edges type
			using impl_type        = std::array<edges_type, N>;							//implementation type

			//vertex iterator
			struct v_iterator
			{
				v_iterator(vertex_type i) : idx { i }
				{
				}

				vertex_type operator * () const noexcept
				{
					return idx;
				}

				auto& operator ++ () noexcept
				{
					++idx;

					return *this;
				}

				auto operator ++ (int) noexcept
				{
					auto tmp { *this };

					operator++();

					return tmp;
				}

				auto& operator -- () noexcept
				{
					--idx;

					return *this;
				}

				auto operator -- (int) noexcept
				{
					auto tmp { *this };

					operator--();

					return tmp;
				}

				bool operator == (v_iterator other) const noexcept
				{
					return idx == other.idx;
				}

				bool operator != (v_iterator other) const noexcept
				{
					return !operator==(other);
				}

				vertex_type idx {};
			};

			//returns the number of vertices
			size_type vertex_count() const noexcept
			{
				return N;
			}

			//checks if the graph is empty
			bool empty() const noexcept
			{
				return vertex_count() == 0;
			}

			//adds an edge
			void add_edge(vertex_type from, vertex_type to, edge_weight_type w = edge_weight_type{})
			{
				if (check_vertex(from) && check_vertex(to))
				{
					add_edge(impl[from], to, w);

					if (is_undirected)
						add_edge(impl[to], from, w);
				}
				else
					std::cerr << "the vertex does not exist!\n";
			}

			//removes an edge
			void remove_edge(vertex_type from, vertex_type to)
			{
				if (check_vertex(from) && check_vertex(to))
				{
					remove_edge(impl[from], to);

					if (is_undirected)
						remove_edge(impl[to], from);
				}
				else
					std::cerr << "the vertex does not exist!\n";
			}

		protected:
			//checks if the vertex exists or not
			bool check_vertex(vertex_type v) const noexcept
			{
				return v < N;
			}

			//adds an edge
			void add_edge(edges_type& edges, vertex_type v, edge_weight_type w)
			{
				auto iter { std::find_if(edges.begin(), edges.end(), [v](const auto& edge) { return edge.neighbor == v; }) };

				if (iter == edges.end())
				{
					if constexpr (std::is_same_v<edge_weight_type, unweighted_type>)
						edges.emplace_back(v);
					else
						edges.emplace_back(v, w);
				}
				else
					std::cerr << "the edge already exists!\n";
			}

			//removes an edge
			void remove_edge(edges_type& edges, vertex_type v)
			{
				auto iter { std::find_if(edges.begin(), edges.end(), [v](const auto& edge) { return edge.neighbor == v; }) };

				if (iter != edges.end())
					edges.erase(iter);
				else
					std::cerr << "the edge does not exist!\n";
			}

			//iterator-like vertex begin
			v_iterator vbegin() const noexcept
			{
				return 0;
			}

			//iterator-like vertex end
			v_iterator vend() const noexcept
			{
				return N;
			}

			//get edges of a specific vertex
			auto& get_edges(vertex_type v) const noexcept
			{
				return impl[v];
			}

			impl_type impl {};
	};

	//graph data structure (adjacency list pro)
	template<class VertexTy, class EdgeWeightTy, edges_are dire = edges_are::undirected>
	class adjacency_list_pro
	{
		public:
			static constexpr bool is_undirected { dire == edges_are::undirected };

			using vertex_type      = VertexTy;											//vertex type
			using edge_weight_type = EdgeWeightTy;										//edge weight type
			using size_type        = std::size_t;										//size  type
			using edge_type        = neighbor_type<vertex_type, edge_weight_type>;		//edge  type
			using edges_type       = std::vector<edge_type>;							//edges type
			using impl_type        = std::unordered_map<vertex_type, edges_type>;		//implementation type

			//vertex iterator
			struct v_iterator : public impl_type::const_iterator
			{
				using base_type = typename impl_type::const_iterator;

				using base_type::base_type;

				v_iterator(base_type b) : base_type { b }
				{
				}

				const vertex_type& operator * () const noexcept
				{
					return base_type::operator*().first;
				}

				const vertex_type* operator -> () const noexcept
				{
					return std::addressof(**this);
				}
			};

			//returns the number of vertices
			size_type vertex_count() const noexcept
			{
				return impl.size();
			}

			//checks if the graph is empty
			bool empty() const noexcept
			{
				return vertex_count() == 0;
			}

			//adds a vertex
			void add_vertex(const vertex_type& v)
			{
				impl.emplace(v, edges_type());
			}

			//adds an edge
			void add_edge(const vertex_type& from, const vertex_type& to, edge_weight_type w = edge_weight_type{})
			{
				auto iter_from { impl.find(from) }, iter_to { impl.find(to) };

				if (iter_from == impl.end() || iter_to == impl.end())
					std::cerr << "the vertex does not exist!\n";
				else
				{
					add_edge(iter_from->second, to, w);

					if (is_undirected)
						add_edge(iter_to->second, from, w);
				}
			}

			//removes an edge
			void remove_edge(const vertex_type& from, const vertex_type& to)
			{
				auto iter_from { impl.find(from) }, iter_to { impl.find(to) };

				if (iter_from == impl.end() || iter_to == impl.end())
					std::cerr << "the vertex does not exist!\n";
				else
				{
					remove_edge(iter_from->second, to);

					if (is_undirected)
						remove_edge(iter_to->second, from);
				}
			}

		protected:
			//checks if the vertex exists or not
			bool check_vertex(const vertex_type& v) const noexcept
			{
				return impl.contains(v);
			}

			//adds an edge
			void add_edge(edges_type& edges, const vertex_type& v, edge_weight_type w)
			{
				auto iter { std::find_if(edges.begin(), edges.end(), [&v](const auto& edge) { return edge.neighbor == v; }) };

				if (iter == edges.end())
				{
					if constexpr (std::is_same_v<edge_weight_type, unweighted_type>)
						edges.emplace_back(v);
					else
						edges.emplace_back(v, w);
				}
				else
					std::cerr << "the edge already exists!\n";
			}

			//removes an edge
			void remove_edge(edges_type& edges, const vertex_type& v)
			{
				auto iter { std::find_if(edges.begin(), edges.end(), [&v](const auto& edge) { return edge.neighbor == v; }) };

				if (iter != edges.end())
					edges.erase(iter);
				else
					std::cerr << "the edge does not exist!\n";
			}

			//iterator-like vertex begin
			v_iterator vbegin() const noexcept
			{
				return impl.begin();
			}

			//iterator-like vertex end
			v_iterator vend() const noexcept
			{
				return impl.end();
			}

			//get edges of a specific vertex
			auto& get_edges(const vertex_type& v) const noexcept
			{
				return impl.find(v)->second;
			}

			impl_type impl {};
	};
}
