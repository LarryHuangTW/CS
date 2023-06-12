#pragma once

namespace cust
{
	namespace Container
	{
		template<class T>
		struct Element
		{
			T value {};

			auto getValueAddress() { return std::addressof(this->value); }
		};

		template<class NodeTy, unsigned int count>
		struct LinkBase
		{
			NodeTy* link[count] { nullptr };

			auto getLinkAddress() { return this->link; }

			auto getLinkCount() { return count; }
		};

		template<class T, unsigned int count = 1>
		struct NodeBase : Element<T>, LinkBase<NodeBase<T, count>, count>
		{
			using value_type   = T;
			using element_type = T;

			auto& next() { return this->link[0]; }
			auto& prev() { return this->link[1]; }
		};
	}
}
