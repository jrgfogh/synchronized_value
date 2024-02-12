#pragma once

#include <memory>
#include <stdexcept>

template <typename Element>
class doubly_linked_list
{
	friend class iterator;

	struct node_t
	{
		Element value;
		std::unique_ptr<node_t> next_;
		node_t *prev_;
	};

	class iterator
	{
		node_t *node_;
	public:
		explicit iterator(node_t* node)
			: node_(node)
		{
		}

		bool operator==(iterator const &other) const = default;

		iterator &operator++()
		{
			return *this;
		}

		Element operator*()
		{
			if (!node_) throw std::runtime_error{"Tried to dereference an invalid iterator."};
			return Element{node_->value};
		}
	};

	std::unique_ptr<node_t> head_;
public:
	iterator insert(iterator pos, Element const& value)
	{
		if (pos.node_)
		{
		}
		else
		{
		}
	}

	auto begin() -> iterator
	{
		return iterator{nullptr};
	}

	auto end() -> iterator
	{
		return iterator{nullptr};
	}
};
