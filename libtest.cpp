#include <random>
#include <tuple>
#include <unordered_map>

#include "fmt/core.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

#include "lib.h"

struct key_a {
  explicit key_a(const char *str) : m_str(str) {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
  }
  std::string m_str;
};

TEST(hm, Insert) {
  lite::hm<int, int> hm_{};
  hm_.insert(20, 90);
  ASSERT_EQ(hm_.exists(20), true);
}

TEST(hm, PreservesOnResize) {
  lite::hm<int, int> hm_{};
  hm_.insert(10, 10);
  hm_.insert(20, 20);
  hm_.insert(30, 30);
  hm_.insert(40, 40);
  hm_.insert(50, 50);
  hm_.insert(60, 60);
  hm_.insert(70, 70);
  hm_.insert(80, 80);
  hm_.insert(90, 90);
  hm_.insert(100, 100);
  hm_.insert(110, 110);
  hm_.insert(120, 120);
  hm_.insert(130, 130);
  hm_.insert(140, 140);
  fmt::print("{}\n", hm_);
  ASSERT_EQ(hm_.exists(10), true);
  ASSERT_EQ(hm_.exists(20), true);
  ASSERT_EQ(hm_.exists(30), true);
  ASSERT_EQ(hm_.exists(40), true);
  ASSERT_EQ(hm_.exists(50), true);
  ASSERT_EQ(hm_.exists(60), true);
  ASSERT_EQ(hm_.exists(70), true);
  ASSERT_EQ(hm_.exists(80), true);
  ASSERT_EQ(hm_.exists(90), true);
  ASSERT_EQ(hm_.exists(100), true);
  ASSERT_EQ(hm_.exists(110), true);
  ASSERT_EQ(hm_.exists(120), true);
  ASSERT_EQ(hm_.exists(130), true);
  ASSERT_EQ(hm_.exists(140), true);
  fmt::print("-----------------\n");
  std::mt19937 gen;
  gen.seed(11);
  std::uniform_int_distribution<> distrib(200, 300);
  for (size_t i = 0; i < 40; ++i) {
    ASSERT_EQ(hm_.exists(distrib(gen)), false);
  }
}

TEST(hm, CustomKey) {
  struct A {
    A() {}
    A(const char *first) : m_first(first) {}
    bool operator==(const A &other) const {
      if (m_first == other.m_first) {
        return true;
      }
      return false;
    }
    ~A() {}
    std::string m_first{};
  };
  struct hasher {
    size_t operator()(const A &a) const {
      size_t res = 17;
      res = res * 31 + std::hash<std::string>()(a.m_first);
      return res;
    }
  };

  lite::hm<A, std::string, hasher> hm_{};
  A a{"key1"};
  hm_.insert(a, "hello world!!!");
  ASSERT_EQ(hm_.exists("key1"), true);
  ASSERT_EQ(hm_.exists("key2"), false);
}

TEST(hm, Get) {
  struct A {
    A() {}
    A(const char *first) : m_first(first) {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
    }
    A(const A &other) {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
      if (this != &other) {
        m_first = other.m_first;
      }
    }
    A(A &&other) {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
      m_first = std::move(other.m_first);
    }
    A &operator=(const A &other) {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
      m_first = other.m_first;
      return *this;
    }
    A &operator=(A &&other) {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
      m_first = std::move(other.m_first);
      return *this;
    }
    bool operator==(const A &other) const {
      fmt::print("{}\n", __PRETTY_FUNCTION__);
      if (this == &other) {
        return true;
      }
      if (m_first == other.m_first) {
        return true;
      }
      return false;
    }
    ~A() { fmt::print("{}\n", __PRETTY_FUNCTION__); }
    std::string m_first{};
  };
  struct hasher {
    size_t operator()(const A &a) const {
      size_t res = 17;
      res = res * 31 + std::hash<std::string>()(a.m_first);
      return res;
    }
  };

  lite::hm<A, std::string, hasher> m{};
  m.insert("key1", "hello world");
  auto v1 = m.get("key1");
  ASSERT_EQ(v1.has_value(), true);
  ASSERT_EQ(v1.value().get().second, "hello world");
  ASSERT_EQ(m.get("key2").has_value(), false);
  auto v2 = m.get("key2");
  ASSERT_EQ(v2.value().get().second, "hello world");
}