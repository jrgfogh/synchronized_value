#pragma once

#include "../synchronized_value/synchronized_value.h"
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace example {
template <typename Element> class concurrent_doubly_linked_list {
  struct node_t {
    Element value;
    // INVARIANT: !next || next->prev == this
    // INVARIANT: next.get() != this
    std::unique_ptr<node_t> next;
    // INVARIANT: prev != this
    node_t *prev;

    [[nodiscard]] auto check_invariants() const -> bool {
      return (!next || next->prev == this) && next.get() != this &&
             prev != this;
    }
  };

public:
  class iterator {
    friend class concurrent_doubly_linked_list;
    node_t *node_;

  public:
    iterator() noexcept = default;
    explicit iterator(node_t *node) : node_(node) {}

    auto operator==(iterator const &other) const -> bool = default;

    auto operator++() -> iterator const & {
      node_ = node_->next.get();
      return *this;
    }

    auto operator*() -> Element {
      if (!node_)
        throw std::runtime_error{"Tried to dereference an invalid iterator."};
      return Element{node_->value};
    }
  };

private:
  // INVARIANT: !!head_ == !!tail_
  // INVARIANT: !head_ || !head_->prev
  mutable synchronized_value<std::unique_ptr<node_t>, std::recursive_mutex> head_;
  mutable synchronized_value<node_t *, std::recursive_mutex> tail_;

  auto insert_empty(auto &headGuard, Element const &value) -> iterator {
    *headGuard = std::make_unique<node_t>(value);
    *tail_ = headGuard->get();
    // TODO(jrgfogh): We're leaking an unguarded reference here:
    return iterator{headGuard->get()};
  }

  auto insert_head(Element const &value) -> iterator {
    update_guard headGuard{head_};
    *headGuard = std::make_unique<node_t>(value, std::move(*headGuard));
    (*headGuard)->next->prev = headGuard->get();
    return iterator{headGuard->get()};
  }

  auto insert_tail(Element const &value) -> iterator {
    if (update_guard headGuard{head_}; !*headGuard)
      return insert_empty(headGuard, value);
    update_guard tailGuard{tail_};
    (*tailGuard)->next = std::make_unique<node_t>(value, nullptr, *tailGuard);
    *tailGuard = (*tailGuard)->next.get();
    return iterator{*tailGuard};
  }

  auto erase_head(auto &headGuard) -> iterator {
    *headGuard = std::move((*headGuard)->next);
    if (*headGuard)
      (*headGuard)->prev = nullptr;
    else
      // This isn't strictly needed, since we never read tail_ in
      // insert_empty(). I assign it anyway, to maintain the invariant.
      *tail_ = nullptr;
    return iterator{headGuard->get()};
  }

  [[nodiscard]] auto check_node_invariants() const -> bool {
    for (auto it = begin(); it != end(); ++it)
      if (!it.node_->check_invariants())
        return false;
    return true;
  }

public:
  auto insert(iterator pos, Element const &value) -> iterator {
    if (!pos.node_)
      return insert_tail(value);
    if (!pos.node_->prev)
      return insert_head(value);
    auto &prev_next = pos.node_->prev->next;
    prev_next =
        std::make_unique<node_t>(value, std::move(prev_next), pos.node_->prev);
    pos.node_->prev = prev_next.get();
    return iterator{prev_next.get()};
  }

  [[nodiscard]] auto begin() const -> iterator { return iterator{head_->get()}; }

  [[nodiscard]] auto end() const -> iterator { return iterator{nullptr}; }

  [[nodiscard]] auto check_invariants() const -> bool {
    update_guard<std::unique_ptr<node_t>, std::recursive_mutex> headGuard{head_};
    update_guard<node_t *, std::recursive_mutex> tailGuard{tail_};
    return !!*headGuard == !!*tailGuard && (!*headGuard || !(*headGuard)->prev) &&
           // NOTE(jrgfogh): This will deadlock!
           check_node_invariants();
  }

  [[nodiscard]] auto serialize_as_dot() const -> std::string {
    std::ostringstream output;
    output << "digraph {\n";
    output << "    head[style=invis];\n";
    output << "    tail[style=invis];\n";
    std::map<void *, int> node_indices;
    // We can't use range-based for, since we want to be able to print broken
    // lists:
    auto i = 0;
    for (auto it = begin(); it != end(); ++it) {
      node_indices[it.node_] = i;
      output << "    node" << i << "[shape=box,label=\"" << *it << "\"];\n";
      ++i;
    }
    auto const size = node_indices.size();
    output << "    head -> node" << node_indices[head_.get()]
           << "[label=\"  head\"];\n";
    output << "    tail -> node" << node_indices[tail_]
           << "[label=\"  tail\"];\n";
    for (auto it = begin(); it != end(); ++it) {
      if (it.node_->next)
        output << "    node" << node_indices[it.node_] << " -> node"
               << node_indices[it.node_->next.get()] << ";\n";
      if (it.node_->prev)
        output << "    node" << node_indices[it.node_] << " -> node"
               << node_indices[it.node_->prev] << ";\n";
    }
    output << "    { rank=same;";
    for (size_t i = 0; i < size; ++i)
      output << " node" << i;
    output << " }\n";
    output << "}\n";
    return output.str();
  }

  auto erase(iterator where) -> iterator {
    if (update_guard headGuard{head_}; headGuard->get() == where.node_)
      return erase_head(headGuard);
    node_t *prev = where.node_->prev;
    if (update_guard tailGuard{tail_}; where.node_ == *tailGuard)
      *tailGuard = prev;
    prev->next = std::move(prev->next->next);
    if (prev->next)
      prev->next->prev = prev;
    return iterator{prev->next.get()};
  }
};
} // namespace example