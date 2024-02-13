#pragma once

#include <memory>
#include <stdexcept>

template <typename Element>
class doubly_linked_list
{
	struct node_t
	{
		Element value;
		std::unique_ptr<node_t> next;
		node_t *prev;
	};

	class iterator
	{
		friend class doubly_linked_list;
		node_t *node_;
	public:
		explicit iterator(node_t* node)
			: node_(node)
		{
		}

		bool operator==(iterator const &other) const = default;

		iterator const &operator++()
		{
			node_ = node_->next.get();
			return *this;
		}

		Element operator*()
		{
			if (!node_) throw std::runtime_error{"Tried to dereference an invalid iterator."};
			return Element{node_->value};
		}
	};

	// INVARIANT: !!head_ == !!tail_.
	std::unique_ptr<node_t> head_;
	node_t * tail_ = nullptr;
	// INVARIANT: The list contains exactly size_ nodes.
	size_t size_ = 0;

	iterator insert_empty(Element const &value)
	{
		head_ = std::make_unique<node_t>(value);
		tail_ = head_.get();
		return iterator{head_.get()};
	}

	iterator insert_head(Element const &value)
	{
		head_ = std::make_unique<node_t>(value, std::move(head_));
		head_->next->prev = head_.get();
		return iterator{head_.get()};
	}

	iterator insert_tail(Element const &value)
	{
		if (!head_)
			return insert_empty(value);
		tail_->next = std::make_unique<node_t>(value, nullptr, tail_);
		tail_ = tail_->next.get();
		return iterator{tail_};
	}
public:
	iterator insert(iterator pos, Element const &value)
	{
		++size_;
		if (!pos.node_)
			return insert_tail(value);
		if (!pos.node_->prev)
			return insert_head(value);
		auto &prev_next = pos.node_->prev->next;
		prev_next = std::make_unique<node_t>(value, std::move(prev_next), pos.node_->prev);
		pos.node_->prev = prev_next.get();
		return iterator{prev_next.get()};
	}

	[[nodiscard]] auto begin() const -> iterator
	{
		return iterator{head_.get()};
	}

	[[nodiscard]] auto end() const -> iterator
	{
		return iterator{nullptr};
	}

	[[nodiscard]] size_t size() const
	{
		return size_;
	}
};
