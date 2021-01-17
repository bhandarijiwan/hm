#ifndef LIB_HPP
#define LIB_HPP
#include "fmt/core.h"
#include <iostream>
#include <optional>
#include <tuple>
#include <vector>
namespace lite {
namespace detail {}

template <typename key, typename value, typename hash = std::hash<key>>
class hm {
  using node_type = std::pair<key, value>;
  using element_type = std::optional<node_type>;
  using container_type = std::vector<element_type>;
  container_type m_nodes;
  size_t m_count{};

  static constexpr double expected_load_factor = 0.75;

private:
  template <typename k = key> size_t index(const k &k_, size_t s) {
    size_t h = hash()(k_);
    fmt::print("hash {} for key {} index {}\n", h, k_, h % s);
    return h % s;
  }
  template <typename k = key>
  size_t probe(size_t i, k &&k_, container_type &nodes_) {
    for (size_t j = i; j < nodes_.capacity(); ++j) {
      if (!nodes_[j] || nodes_[j].value().first == k_) {
        return i + (j - i);
      }
    }
    return nodes_.capacity();
  }

  void emplace_at(element_type *elem_ptr, element_type &&e) {
    elem_ptr->~element_type();
    new (elem_ptr) element_type(std::move(e));
  }

  template <typename... Args>
  void emplace_at(element_type *elem_ptr, Args &&...args) {
    fmt::print("caling emplace args with variadic\n");
    elem_ptr->~element_type();
    new (elem_ptr) element_type(std::in_place, std::forward<Args>(args)...);
  }

  void resize(size_t new_size) {
    container_type new_m_nodes;
    new_m_nodes.reserve(new_size);
    fmt::print(" resizing to new size {}\n", new_size);
    for (size_t i = 0; i < m_nodes.capacity(); ++i) {
      auto &node = m_nodes[i];
      if (node) {
        auto &k_ = node.value().first;
        fmt::print(stderr, "key = {} index={}\n", k_,
                   index(k_, new_m_nodes.capacity()));
        auto idx = probe(index(k_, new_m_nodes.capacity()), k_, new_m_nodes);
        emplace_at(&m_nodes[i], std::move(node));
      }
    }
    m_nodes = std::move(new_m_nodes);
  }
  friend std::ostream &operator<<(std::ostream &out, const hm &m) {
    out << "size = " << m.m_nodes.size() << "[\n";
    for (size_t i = 0; i < m.m_nodes.capacity(); ++i) {
      auto &o = m.m_nodes[i];
      if (!o) {
        out << "\tNone";
      } else {
        out << "\t(" << o.value().first << ", " << o.value().second << ")";
      }
      out << ", \n";
    }
    out << "]\n";
    return out;
  }

public:
  explicit hm() : m_nodes(), m_count(0) { m_nodes.reserve(13); }
  auto size() const { return m_count; }

  template <typename k = key> bool exists(const k &&k_) {
    auto idx = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    if (idx >= m_nodes.capacity() || !m_nodes[idx]) {
      return false;
    }
    return true;
  }

  template <class k = key, class v = value> void insert(k &&k_, v &&v_) {

    auto load_factor = static_cast<double>(m_count + 1) /
                       static_cast<double>(m_nodes.capacity());
    if (load_factor > expected_load_factor) {
      resize(m_nodes.capacity() * 2);
    }
    // fmt::print("size{} load_factor={}\n", m_nodes.size(), load_factor);
    auto idx = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    // fmt::print("position =  {}\n", idx);
    emplace_at(&m_nodes[idx], std::forward<k>(k_), std::forward<v>(v_));
    m_count += 1;
    fmt::print("{}\n", *this);
  }
};

} // namespace lite

#endif /* LIB_HPP */