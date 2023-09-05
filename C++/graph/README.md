![Graph](/Images/graph/Graph.svg)

## Fixed-size and weighted:

> *Vertices are indices or can be directly represented by indices from 0, 1, 2, ... to N-1*

![fixed-size weighted adjacency matrix](/Images/graph/01__fixed_weighted_matrix.svg)

```C++
template<std::size_t N, class EdgeWeightTy, bool is_directed = false>
class adjacency_matrix
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = EdgeWeightTy;

		void add_edge(vertex_type from, vertex_type to, edge_weight_type ew);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::array<std::array<edge_weight_type, N>, N> edges {};
};
```

## Fixed-size and unweighted:

![fixed-size unweighted adjacency matrix](/Images/graph/02__fixed_unweighted_matrix.svg)

```C++
struct unweighted_type
{
};

template<std::size_t N, bool is_directed>
class adjacency_matrix<N, unweighted_type, is_directed>
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = unweighted_type;

		void add_edge(vertex_type from, vertex_type to);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::array<std::bitset<N>, N> edges {};
};
```

## Variable-size and weighted:

![variable-size weighted adjacency matrix](/Images/graph/03__variable_weighted_matrix.svg)

```C++
template<class EdgeWeightTy, bool is_directed>
class adjacency_matrix<0, EdgeWeightTy, is_directed>
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = EdgeWeightTy;

		void add_vertex();
		void add_edge(vertex_type from, vertex_type to, edge_weight_type ew);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::vector<std::vector<edge_weight_type>> edges {};
};
```

## Variable-size and unweighted:

![variable-size unweighted adjacency matrix](/Images/graph/04__variable_unweighted_matrix.svg)

```C++
template<bool is_directed>
class adjacency_matrix<0, unweighted_type, is_directed>
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = unweighted_type;

		void add_vertex();
		void add_edge(vertex_type from, vertex_type to);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::vector<std::vector<bool>> edges {};
};
```

##  

![fixed-size adjacency list](/Images/graph/05__fixed_list.svg)

```C++
template<std::size_t N, class EdgeWeightTy, bool is_directed = false>
class adjacency_list
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = EdgeWeightTy;
		using edge_type        = std::pair<vertex_type, edge_weight_type>;

		void add_edge(vertex_type from, vertex_type to, edge_weight_type ew);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::array<std::vector<edge_type>, N> edges {};
};

template<std::size_t N, bool is_directed>
class adjacency_list<N, unweighted_type, is_directed>
{
	public:
		using vertex_type      = std::size_t;
		using edge_weight_type = unweighted_type;

		void add_edge(vertex_type from, vertex_type to);
		void remove_edge(vertex_type from, vertex_type to);

		// ......

	protected:
		std::array<std::vector<vertex_type>, N> edges {};
};
```

##  

> *Vertices are not indices or can not be directly represented by indices from 0, 1, 2, ... to N-1*

![adjacency matrix pro](/Images/graph/06__adjacency_matrix_pro.svg)

```C++
template<class VertexTy, class EdgeWeightTy, bool is_directed = false>
class adjacency_matrix_pro
{
	public:
		using vertex_type      = VertexTy;
		using edge_weight_type = EdgeWeightTy;
		using size_type        = std::size_t;
		using edge_type        = std::pair<vertex_type, edge_weight_type>;

		void add_vertex(const vertex_type& v);
		void add_edge(const vertex_type& v1, const vertex_type& v2, const edge_weight_type& ew);
		void remove_edge(const vertex_type& v1, const vertex_type& v2);

		// ......

	protected:
		std::unordered_map<vertex_type, size_type>        vertices {};
		std::vector<typename decltype(vertices)::pointer> vs {};
		std::vector<std::vector<edge_type>>               edges {};
};
```

##  

![adjacency list pro](/Images/graph/07__adjacency_list_pro.svg)

```C++
template<class VertexTy, class EdgeWeightTy, bool is_directed = false>
class adjacency_list_pro
{
	public:
		using vertex_type      = VertexTy;
		using edge_weight_type = EdgeWeightTy;
		using edge_type        = std::pair<vertex_type, edge_weight_type>;

		void add_vertex(const vertex_type& v);
		void add_edge(const vertex_type& v1, const vertex_type& v2, const edge_weight_type& ew);
		void remove_edge(const vertex_type& v1, const vertex_type& v2);

		// ......

	protected:
		std::unordered_map<vertex_type, std::vector<edge_type>> umap {};
};
```

##  

![adjacency tree](/Images/graph/08__adjacency_tree.svg)

```C++
template<class VertexTy, class EdgeWeightTy, bool is_directed = false>
class adjacency_tree
{
	public:
		using vertex_type      = VertexTy;
		using edge_weight_type = EdgeWeightTy;

		void add_vertex(const vertex_type& v);
		void add_edge(const vertex_type& v1, const vertex_type& v2, const edge_weight_type& ew);
		void remove_edge(const vertex_type& v1, const vertex_type& v2);

		// ......

	protected:
		std::map<vertex_type, std::map<vertex_type, edge_weight_type>> mp {};
};
```
