## Code snippets:

```C++
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

	//graph data structure (adjacency list)
	template<std::size_t N, class EdgeWeightTy, edges_are dire = edges_are::undirected>
	class adjacency_list
	{
		public:
			static constexpr bool is_undirected { dire == edges_are::undirected };

			using vertex_type      = std::size_t;						//vertex type
			using edge_weight_type = EdgeWeightTy;						//edge weight type
			using size_type        = std::size_t;						//size  type
			using edge_type        = neighbor_type<vertex_type, edge_weight_type>;		//edge  type
			using edges_type       = std::vector<edge_type>;				//edges type
			using impl_type        = std::array<edges_type, N>;				//implementation type

			//vertex iterator
			struct v_iterator;

			size_type vertex_count() const noexcept;		//returns the number of vertices
			bool empty() const noexcept;				//checks if the graph is empty
			void add_edge(vertex_type from, vertex_type to, edge_weight_type w = edge_weight_type{});	//adds    an edge
			void remove_edge(vertex_type from, vertex_type to);						//removes an edge

		protected:
			// ......
			v_iterator vbegin() const noexcept;			//iterator-like vertex begin
			v_iterator vend()   const noexcept;			//iterator-like vertex end
			auto& get_edges(vertex_type v) const noexcept;		//get edges of a specific vertex

			impl_type impl {};
	};

	//graph data structure (adjacency list pro)
	template<class VertexTy, class EdgeWeightTy, edges_are dire = edges_are::undirected>
	class adjacency_list_pro
	{
		public:
			static constexpr bool is_undirected { dire == edges_are::undirected };

			using vertex_type      = VertexTy;						//vertex type
			using edge_weight_type = EdgeWeightTy;						//edge weight type
			using size_type        = std::size_t;						//size  type
			using edge_type        = neighbor_type<vertex_type, edge_weight_type>;		//edge  type
			using edges_type       = std::vector<edge_type>;				//edges type
			using impl_type        = std::unordered_map<vertex_type, edges_type>;		//implementation type

			//vertex iterator
			struct v_iterator : public impl_type::const_iterator
			{
				// ......
			};

			size_type vertex_count() const noexcept;		//returns the number of vertices
			bool empty() const noexcept;				//checks if the graph is empty
			void add_vertex(const vertex_type& v);			//adds a vertex
			void add_edge(const vertex_type& from, const vertex_type& to, edge_weight_type w = edge_weight_type{});		//adds    an edge
			void remove_edge(const vertex_type& from, const vertex_type& to);						//removes an edge

		protected:
			// ......
			v_iterator vbegin() const noexcept;			//iterator-like vertex begin
			v_iterator vend()   const noexcept;			//iterator-like vertex end
			auto& get_edges(const vertex_type& v) const noexcept;	//get edges of a specific vertex

			impl_type impl {};
	};

	//graph data structures and algorithms
	template<class impl_type>
	class graph : public impl_type
	{
		public:
			using vertex_type      = typename impl_type::vertex_type;		//vertex type
			using edge_weight_type = typename impl_type::edge_weight_type;		//edge weight type
			using edge_type        = typename impl_type::edge_type;			//edge type
			using size_type        = typename impl_type::size_type;			//size type

			// ......

			bool has_loop()  const noexcept;		//checks if the graph has any loop
			bool has_cycle() const;				//checks if the graph has any cycle
			bool is_DAG()    const;				//checks if the graph is a directed acyclic graph (DAG)

			auto depth_first_search(const vertex_type& src,   const vertex_type& dest) const;	//depth-first   search (DFS)
			auto breadth_first_search(const vertex_type& src, const vertex_type& dest) const;	//breadth-first search (BFS)
			auto topological_sort(bool flag = true) const;						//topological sort / task scheduling
			auto shortest_path_search(const vertex_type& src, const vertex_type& dest, bool allow_negative_weight = false) const;	//shortest path search (Dijkstra and Bellman-Ford algorithm)
			auto minimum_spanning_tree() const;							//finds minimum spanning tree (Prim's algorithm)

		private:
			// ......
	};
}
```

## Example:

![POC examples](/Images/graph/POC_examples.svg)

```C++
#include <iostream>
#include <array>
#include <initializer_list>
#include <string>
#include <type_traits>
#include "graph.h"

struct weighted_edge
{
	char from   {};		//from vertex
	char to     {};		//to   vertex
	int  weight {};		//edge weight
};

template<class impl_type>
decltype(auto) operator << (std::ostream& os, const cust::graph<impl_type>& g)
{
	os << "the number of vertices : " << g.vertex_count() << "\n";
	os << "has loop  ? " << g.has_loop()  << "\n";
	os << "has cycle ? " << g.has_cycle() << "\n";
	os << "is DAG    ? " << g.is_DAG()    << "\n\n";

	auto result { g.topological_sort() };

	os << "Topological sort : ";

	for (auto elem : result)
	{
		if constexpr (std::is_same_v<decltype(elem), std::size_t>)
			os << static_cast<char>(elem + 'A');
		else
			os << elem;

		os << (elem == result.back() ? "" : " , ");
	}

	os << "\n\n";

	return os;
}

void print_path(auto from, auto to, const auto& path)
{
	std::cout << "from " << from << " to " << to << " : ";

	for (auto elem : path)
	{
		if constexpr (std::is_same_v<decltype(elem), std::size_t>)
			std::cout << static_cast<char>(elem + 'A'); 
		else
			std::cout << elem;

		std::cout << (elem == path.back() ? "" : " --> ");
	}

	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	using cust::graph;
	using cust::adjacency_list;
	using cust::adjacency_list_pro;
	using cust::edges_are;

	const std::array<char, 9> vertices { 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' };
	const std::initializer_list<weighted_edge> edges { { 'A', 'B', 2 }, { 'A', 'E', 5 }, { 'A', 'D', 1 }, { 'B', 'E', 3 }, { 'C', 'I', 2 }, { 'D', 'G', 1 }, { 'D', 'H', 3 }, { 'E', 'F', 1 }, { 'F', 'C', 1 }, { 'G', 'H', 1 }, { 'H', 'E', 1 }, { 'H', 'F', 3 }, { 'H', 'I', 6 } };

	std::cout << std::boolalpha;

	graph<adjacency_list<vertices.size(), int, edges_are::directed>> g1 {};		//adjacency list
	graph<adjacency_list_pro<char,        int, edges_are::directed>> g2 {};		//adjacency list pro (directed graph and vertex type is char)
	graph<adjacency_list_pro<std::string, int, edges_are::directed>> g3 {};		//adjacency list pro (directed graph and vertex type is string)
	graph<adjacency_list_pro<char,        int>>                      g4 {};		//adjacency list pro (undirected graph)

	for (auto vertex : vertices)
		g2.add_vertex(vertex);

	for (auto edge : edges)
		g1.add_edge(edge.from - vertices.front(), edge.to - vertices.front(), edge.weight);

	for (auto edge : edges)
		g2.add_edge(edge.from, edge.to, edge.weight);

	std::cout << g1;						// Topological sort : I , C , F , E , B , H , G , D , A
	std::cout << g2;

	std::cout << "[ Depth-first Search ]\n\n";

	print_path('A', 'I', g1.depth_first_search(0, 8));		// from A to I : A --> B --> E --> F --> C --> I
	print_path('A', 'I', g2.depth_first_search('A', 'I'));

	print_path('B', 'D', g1.depth_first_search(1, 3));		// from B to D :
	print_path('B', 'D', g2.depth_first_search('B', 'D'));

	std::cout << "[ Breadth-first Search ]\n\n";

	print_path('A', 'I', g1.breadth_first_search(0, 8));		// from A to I : A --> D --> H --> I
	print_path('A', 'I', g2.breadth_first_search('A', 'I'));

	std::cout << "[ Shortest Path Search ]\n\n";

	auto pr1 { g1.shortest_path_search(0, 8)     };
	auto pr2 { g2.shortest_path_search('A', 'I') };

	print_path('A', 'I', pr1.first);				// from A to I : A --> D --> G --> H --> E --> F --> C --> I
	print_path('A', 'I', pr2.first);

	std::cout << "path weight " << pr1.second << "\n\n";		// path weight 8
	std::cout << "path weight " << pr2.second << "\n\n";

	std::cout << "------------------------------------------------------------------\n\n";

	g3.add_vertex("src");		//source      vertex
	g3.add_vertex("dest");		//destination vertex
	g3.add_vertex("A");
	g3.add_vertex("B");
	g3.add_vertex("C");

	g3.add_edge("src", "A",    1);
	g3.add_edge("src", "dest", 5);
	g3.add_edge("src", "B",    3);
	g3.add_edge("A", "dest",   2);
	g3.add_edge("B", "C",      6);
	g3.add_edge("C", "dest",  -8);

	std::cout << g3;						// Topological sort : dest , A , C , B , src

	std::cout << "[ Shortest Path Search with negative weight ]\n\n";

	auto pr3 { g3.shortest_path_search("src", "dest", true)};

	print_path("src", "dest", pr3.first);				// from src to dest : src --> B --> C --> dest

	std::cout << "path weight " << pr3.second << "\n\n";		// path weight 1

	std::cout << "------------------------------------------------------------------\n\n";

	const std::array<char, 6> nodes { 'A' , 'B' , 'C' , 'D' , 'E' , 'F' };
	const std::initializer_list<weighted_edge> links { { 'A', 'B', 1 } , { 'A', 'D', 3 } , { 'B', 'C', 6 } , { 'B', 'D', 5 } , { 'B', 'E', 1 } , { 'C', 'E', 5 } , { 'C', 'F', 2 } , { 'D', 'E', 1 } , { 'E', 'F', 4 } };

	for (auto node : nodes)
		g4.add_vertex(node);

	for (auto link : links)
		g4.add_edge(link.from, link.to, link.weight);

	std::cout << g4;						// has cycles and it's not a DAG

	std::cout << "Total weight of Minimum Spanning Tree : " << g4.minimum_spanning_tree() << "\n\n";	// Total weight 9

	return 0;
}
```
