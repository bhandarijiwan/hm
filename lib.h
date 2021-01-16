#ifndef LIB_HPP
#define LIB_HPP
#include "fmt/core.h"
#include <optional>
#include <tuple>
#include <vector>
namespace lite {
namespace detail {}

// number of buckets vs number of elements per
// load factor

template <typename key, typename value, typename hash = std::hash<key>>
class hm {
  using container_type = std::vector<std::optional<std::pair<key, value>>>;
  container_type m_nodes;
  size_t m_count{};

  static constexpr double expected_load_factor = 0.5;

private:
  template <typename k = key> size_t index(const k &k_, size_t s) {
    size_t h = hash()(k_);
    return h % s;
  }
  template <typename k = key>
  auto probe(size_t i, k &&k_, container_type &nodes_) {
    for (size_t j = i; j < nodes_.capacity(); ++j) {
      if (!nodes_[j] || nodes_[j].value().first == k_) {
        return nodes_.begin() + j;
      }
    }
    return m_nodes.end();
  }
  void resize(size_t new_size) {
    container_type v;
    v.reserve(new_size);
    for (size_t i = 0; i < m_nodes.capacity(); ++i) {
      auto &node = m_nodes[i];
      if (node) {
        auto &k_ = node.value().first;
        auto iterator = probe(index(k_, v.capacity()), k_, v);
        *iterator = std::move(node);
      }
    }
    m_nodes = std::move(v);
  }

public:
  explicit hm() : m_nodes(), m_count(0) { m_nodes.reserve(10); }
  auto size() { return m_count; }

  template <typename k = key> bool exists(const k &&k_) {
    auto iterator = probe(index(k_, m_nodes.capacity()), k_, m_nodes);
    if (iterator == m_nodes.end() || !*iterator) {
      return false;
    }
    return true;
  }

  template <class k = key, class v = value> void insert(k &&_k, v &&_v) {

    auto load_factor =
        static_cast<double>(m_count) / static_cast<double>(m_nodes.capacity());
    if (load_factor >= expected_load_factor) {
      resize(m_nodes.capacity() * 2);
    }
    m_nodes.template emplace(m_nodes.begin(), std::in_place,
                             std::forward<k>(_k), std::forward<v>(_v));
    m_count += 1;
  }
};

} // namespace lite

#endif /* LIB_HPP */