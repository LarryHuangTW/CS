#pragma once

namespace cust					//customized / non-standard
{
	namespace Container
	{
		template<class T>
		struct Element
		{
			T value {};

			auto getValueAddress() { return std::addressof(this->value); }
		};

		template<class NodeTy, unsigned int size>
		struct LinkBase
		{
			NodeTy* link[size] { nullptr };

			auto getLinkAddress() { return this->link; }

			unsigned int size() { return size; }
		};

		template<class T, unsigned int size = 1>
		struct NodeBase : Element<T> , LinkBase<NodeBase<T>, size>
		{
			using value_type   = T;
			using element_type = T;

			auto& next() { return this->link[0]; }
		};
	}
}
