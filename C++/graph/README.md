![Graph](/Images/graph/Graph.svg)

## Code snippets:

![weighted adjacency matrix with fixed size](/Images/graph/01__fixed_weighted_matrix.svg)

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

## Example:

![unweighted adjacency matrix with fixed size](/Images/graph/02__fixed_unweighted_matrix.svg)

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

## Example:

![weighted adjacency matrix with variable size](/Images/graph/03__variable_weighted_matrix.svg)

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

## Example:

![unweighted adjacency matrix with variable size](/Images/graph/04__variable_unweighted_matrix.svg)

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

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```

## Example:

```C++

```
