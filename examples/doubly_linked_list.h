#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

template <typename Element>
class doubly_linked_list
{
	struct node_t
	{
		Element value;
		// INVARIANT: !next || next->prev == this
		// INVARIANT: next.get() != this
		std::unique_ptr<node_t> next;
		// INVARIANT: prev != this
		node_t *prev;

		[[nodiscard]] bool check_invariants() const
		{
			return (!next || next->prev == this) &&
				next.get() != this &&
				prev != this;
		}
	};
public:
	class iterator
	{
		friend class doubly_linked_list;
		node_t *node_;
	public:
		iterator() noexcept = default;
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

private:
	// INVARIANT: !!head_ == !!tail_
	// INVARIANT: !head_ || !head_->prev
	std::unique_ptr<node_t> head_;
	node_t *tail_ = nullptr;

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

	iterator erase_head()
	{
		head_ = std::move(head_->next);
		if (head_)
			head_->prev = nullptr;
		else
			// This isn't strictly needed, since we never read tail_ in insert_empty().
			// I assign it anyway, to maintain the invariant.
			tail_ = nullptr;
		return iterator{head_.get()};
	}

	[[nodiscard]] bool check_node_invariants() const
	{
		for (auto it = begin(); it != end(); ++it)
			if (!it.node_->check_invariants())
				return false;
		return true;
	}
public:
	iterator insert(iterator pos, Element const &value)
	{
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

	[[nodiscard]] bool check_invariants() const
	{
		return !!head_ == !!tail_ &&
			(!head_ || !head_->prev) &&
			check_node_invariants();
	}

	[[nodiscard]] std::string serialize_as_dot() const
	{
		std::ostringstream output;
		output << "digraph {\n";
		output << "    head[style=invis];\n";
		output << "    tail[style=invis];\n";
		std::map<void*, int> node_indices;
		// We can't use range-based for, since we want to be able to print broken lists:
		auto i = 0;
		for (auto it = begin(); it != end(); ++it)
		{
			node_indices[it.node_] = i;
			output << "    node" << i << "[shape=box,label=\"" << *it << "\"];\n";
			++i;
		}
		auto const size = node_indices.size();
		output << "    head -> node" << node_indices[head_.get()] << "[label=\"  head\"];\n";
		output << "    tail -> node" << node_indices[tail_] << "[label=\"  tail\"];\n";
		for (auto it = begin(); it != end(); ++it)
		{
			if (it.node_->next)
				output << "    node" << node_indices[it.node_] << " -> node" <<
					node_indices[it.node_->next.get()] << ";\n";
			if (it.node_->prev)
				output << "    node" << node_indices[it.node_] << " -> node" <<
					node_indices[it.node_->prev] << ";\n";
		}
		output << "    { rank=same;";
		for (size_t i = 0; i < size; ++i)
			output << " node" << i;
		output << " }\n";
		output << "}\n";
		return output.str();
	}

	iterator erase(iterator where)
	{
		if (head_.get() == where.node_)
			return erase_head();
		node_t *prev = where.node_->prev;
		if (where.node_ == tail_)
			tail_ = prev;
		prev->next = std::move(prev->next->next);
		if (prev->next)
			prev->next->prev = prev;
		return iterator{prev->next.get()};
	}
};
