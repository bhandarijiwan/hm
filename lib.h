#ifndef LIB_HPP
#define LIB_HPP
#include <functional>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vector>

#include "fmt/core.h"
#include "fmt/ostream.h"

namespace lite {
namespace detail {}

template <typename key, typename value, typename hash = std::hash<key>>
class hm {
  using node_type = std::pair<key, value>;
  using element_type = std::optional<node_type>;
  using container_type = std::vector<element_type>;
  using return_type = std::optional<std::reference_wrapper<const node_type>>;
  container_type m_nodes;
  size_t m_count{};

  static constexpr double expected_load_factor = 0.75;

  static_assert(std::is_copy_constructible_v<key>,
                "key should be copy constructible.\n");

private:
  size_t index(const key &k_, size_t s) const {
    size_t h = hash()(k_);
    return h % s;
  }
  template <typename k = key>
  [[nodiscard]] size_t probe(size_t i, const k &k_,
                             const container_type &nodes_) const {
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
    elem_ptr->~element_type();
    new (elem_ptr) element_type(std::in_place, std::forward<Args>(args)...);
  }

  void resize(size_t new_size) {
    container_type new_m_nodes;
    new_m_nodes.reserve(new_size);
    for (size_t i = 0; i < m_nodes.capacity(); ++i) {
      auto &node = m_nodes[i];
      if (node) {
        auto &k_ = node.value().first;
        size_t idx = probe(index(k_, new_m_nodes.capacity()), k_, new_m_nodes);
        emplace_at(&new_m_nodes[idx], std::move(node));
      }
    }
    m_nodes = std::move(new_m_nodes);
  }

  float load_factor() {
    return static_cast<float>(m_count + 1) /
           static_cast<float>(m_nodes.capacity());
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

  bool exists(const key &k_) {
    size_t idx = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    if (idx >= m_nodes.capacity() || !m_nodes[idx]) {
      return false;
    }
    return true;
  }

  template <class v = value> void insert(const key &k_, v &&v_) {
    auto l = load_factor();
    if (l > expected_load_factor) {
      resize(m_nodes.capacity() * 2);
    }
    auto idx = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    emplace_at(&m_nodes[idx], k_, std::forward<v>(v_));
    m_count += 1;
  }

  return_type get(const key &k_) const {
    auto idx = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    if (idx < m_nodes.capacity()) {
      return std::reference_wrapper(m_nodes[idx].value());
    }
    return std::nullopt;
  }

  ~hm() {
    for (size_t i = 0; i < m_nodes.capacity(); ++i) {
      m_nodes[i].~element_type();
    }
  }
};

} // namespace lite

#endif /* LIB_HPP */
